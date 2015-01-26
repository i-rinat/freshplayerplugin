/*
 * Copyright © 2013-2014  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <pthread.h>
#include "audio_thread.h"
#include <asoundlib.h>
#include <glib.h>
#include <unistd.h>
#include "trace.h"
#include "config.h"
#include "eintr_retry.h"
#include "ppb_message_loop.h"


struct audio_stream_s {
    audio_stream_direction      direction;
    snd_pcm_t                  *pcm;
    struct pollfd              *fds;
    size_t                      nfds;
    audio_stream_capture_cb_f  *capture_cb;
    audio_stream_playback_cb_f *playback_cb;
    void                       *cb_user_data;
    volatile int                paused;
};

static GHashTable      *active_streams_ht = NULL;
static GHashTable      *stream_by_fd_ht = NULL;
static GList           *streams_to_delete = NULL;
static volatile int     rebuild_fds = 0;
static volatile int     audio_thread_started = 0;
static volatile int     terminate_thread = 0;
static int              notification_pipe[2];
static pthread_t        audio_thread_id;
static pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;


static
void
recover_pcm(snd_pcm_t *pcm)
{
    switch (snd_pcm_state(pcm)) {
    case SND_PCM_STATE_XRUN:
        snd_pcm_recover(pcm, -EPIPE, 1);
        break;
    case SND_PCM_STATE_SUSPENDED:
        snd_pcm_recover(pcm, -ESTRPIPE, 1);
        break;
    default:
        snd_pcm_drop(pcm);
        snd_pcm_prepare(pcm);
        break;
    }
}

static
void
drain_wakeup_pipe(int fd)
{
    char buf[8];
    while (RETRY_ON_EINTR(read(fd, buf, sizeof(buf))) > 0) {
        // cycle here doing nothing
    }
}

static
void
process_stream_deletions_unlocked(void)
{
    GList *ll = streams_to_delete;
    while (ll) {
        struct audio_stream_s *as = ll->data;

        g_hash_table_remove(active_streams_ht, as);
        for (uintptr_t k = 0; k < as->nfds; k ++)
            g_hash_table_remove(stream_by_fd_ht, GINT_TO_POINTER(as->fds[k].fd));
        snd_pcm_close(as->pcm);
        free(as);

        ll = g_list_next(ll);
    }
    g_list_free(streams_to_delete);
    streams_to_delete = NULL;
}

static
nfds_t
do_rebuild_fds(struct pollfd **out_fds)
{
    GHashTableIter  iter;
    gpointer        key, val;
    void           *tmp;
    nfds_t          nfds;
    struct pollfd  *fds;

    pthread_mutex_lock(&lock);
    process_stream_deletions_unlocked();

    fds = *out_fds;
    nfds = 1;
    tmp = realloc(fds, sizeof(fds[0]));
    if (!tmp) {
        free(fds);
        fds = NULL;
        trace_error("%s, memory allocation failed\n", __func__);
        goto err;
    }
    fds = tmp;

    // wakeup pipe
    fds[0].fd = notification_pipe[0];
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    // audio file handles
    g_hash_table_iter_init(&iter, active_streams_ht);
    while (g_hash_table_iter_next(&iter, &key, &val)) {
        audio_stream *as = key;

        if (!as) {
            trace_error("%s, NULL key in active_streams_ht\n", __func__);
            nfds = 0;
            goto err;
        }

        tmp = realloc(fds, (nfds + as->nfds) * sizeof(as->fds[0]));
        if (!tmp) {
            free(fds);
            fds = NULL;
            trace_error("%s, memory allocation failed\n", __func__);
            nfds = 0;
            goto err;
        }
        fds = tmp;
        for (uintptr_t k = 0; k < as->nfds; k ++)
            fds[nfds + k] = as->fds[k];

        nfds += as->nfds;
    }

err:
    g_atomic_int_set(&rebuild_fds, 0);
    pthread_mutex_unlock(&lock);
    *out_fds = fds;
    return nfds;
}

static
void *
audio_thread(void *param)
{
    struct pollfd  *fds = NULL;
    nfds_t          nfds = 0;
    static char     buf[16 * 1024];

    ppb_message_loop_mark_thread_unsuitable();

    while (1) {
        if (g_atomic_int_get(&terminate_thread))
            goto quit;

        if (g_atomic_int_get(&rebuild_fds)) {
            nfds = do_rebuild_fds(&fds);
            if (nfds == 0)
                goto quit;
        }

        int res = poll(fds, nfds, 10 * 1000);
        if (res == -1) {
            if (errno == EINTR)
                continue;
            trace_error("%s, poll, errno=%d\n", __func__, errno);
            continue;
        }

        if (res == 0 || fds == NULL)
            continue;

        if (fds[0].revents)
            drain_wakeup_pipe(fds[0].fd);

        for (uintptr_t k = 1; k < nfds; k ++) {
            unsigned short revents = 0;
            audio_stream *as = g_hash_table_lookup(stream_by_fd_ht, GINT_TO_POINTER(fds[k].fd));

            // check if stream was deleted
            if (!as)
                continue;

            snd_pcm_poll_descriptors_revents(as->pcm, &fds[k], 1, &revents);

            if (revents & (~(POLLIN | POLLOUT))) {
                trace_warning("%s, revents have unexpected flags set (%u)\n", __func__,
                              (unsigned int)revents);
                recover_pcm(as->pcm);
            }

            if (revents & (POLLIN | POLLOUT)) {
                int                 paused = g_atomic_int_get(&as->paused);
                const size_t        frame_size = 2 * sizeof(int16_t);

                snd_pcm_sframes_t frame_count = snd_pcm_avail(as->pcm);
                size_t sz = MIN(frame_count * frame_size, sizeof(buf));

                if (revents & POLLIN) {
                    // TODO: implement POLLIN
                } else {
                    // POLLOUT
                    if (paused || !as->playback_cb)
                        memset(buf, 0, sz);
                    else
                        as->playback_cb(buf, sz, 0, as->cb_user_data);

                    snd_pcm_sframes_t written = snd_pcm_writei(as->pcm, buf, sz / frame_size);
                    if (written < 0) {
                        trace_warning("%s, snd_pcm_writei error %d\n", __func__, (int)written);
                        recover_pcm(as->pcm);
                    }
                }
            }
        }
    }

quit:
    free(fds);
    return NULL;
}

static
void
make_nonblock(int fd)
{
    int flags;
    flags = fcntl(fd, F_GETFL, 0) | O_NONBLOCK;
    (void)fcntl(fd, F_SETFL, flags);
}

static
void
__attribute__((constructor))
constructor_audio_thread_alsa(void)
{
    active_streams_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    stream_by_fd_ht = g_hash_table_new(g_direct_hash, g_direct_equal);

    if (pipe(notification_pipe) != 0) {
        trace_error("%s, pipe creation failed\n", __func__);
        notification_pipe[0] = -1;
        notification_pipe[1] = -1;
        return;
    }

    make_nonblock(notification_pipe[0]);
    make_nonblock(notification_pipe[1]);

    g_atomic_int_set(&rebuild_fds, 1);
}

static
void
__attribute__((destructor))
destructor_audio_thread_alsa(void)
{
    if (g_atomic_int_get(&audio_thread_started)) {
        g_atomic_int_set(&terminate_thread, 1);
        pthread_join(audio_thread_id, NULL);
    }

    g_hash_table_unref(active_streams_ht);
    g_hash_table_unref(stream_by_fd_ht);

    close(notification_pipe[0]);
    close(notification_pipe[1]);
}

static
void
wakeup_audio_thread(void)
{
    g_atomic_int_set(&rebuild_fds, 1);
    RETRY_ON_EINTR(write(notification_pipe[1], "+", 1));
}

static
audio_stream *
alsa_create_stream(audio_stream_direction type, unsigned int sample_rate,
                    unsigned int sample_frame_count)
{
    audio_stream *as;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;
    int dir;

    if (!g_atomic_int_get(&audio_thread_started)) {
        pthread_create(&audio_thread_id, NULL, audio_thread, NULL);
        g_atomic_int_set(&audio_thread_started, 1);
    }

    as = calloc(1, sizeof(*as));
    if (!as)
        goto err;

#define CHECK_A(funcname, params)                                                       \
    do {                                                                                \
        int errcode___ = funcname params;                                               \
        if (errcode___ < 0) {                                                           \
            trace_error("%s, " #funcname ", %s\n", __func__, snd_strerror(errcode___)); \
            goto err;                                                                   \
        }                                                                               \
    } while (0)

    // TODO: select device
    // TODO: create capture stream
    CHECK_A(snd_pcm_open, (&as->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0));
    CHECK_A(snd_pcm_hw_params_malloc, (&hw_params));
    CHECK_A(snd_pcm_hw_params_any, (as->pcm, hw_params));
    CHECK_A(snd_pcm_hw_params_set_access, (as->pcm, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED));
    CHECK_A(snd_pcm_hw_params_set_format, (as->pcm, hw_params, SND_PCM_FORMAT_S16_LE));
    dir = 0;
    unsigned int rate = sample_rate;
    CHECK_A(snd_pcm_hw_params_set_rate_near, (as->pcm, hw_params, &rate, &dir));
    CHECK_A(snd_pcm_hw_params_set_channels, (as->pcm, hw_params, 2));

    unsigned int period_time = (long long)sample_frame_count * 1000 * 1000 / sample_rate;
    period_time = CLAMP(period_time,
                        1000 * config.audio_buffer_min_ms,
                        1000 * config.audio_buffer_max_ms);
    dir = 1;
    CHECK_A(snd_pcm_hw_params_set_period_time_near, (as->pcm, hw_params, &period_time, &dir));

    unsigned int buffer_time = 4 * period_time;
    dir = 1;
    CHECK_A(snd_pcm_hw_params_set_buffer_time_near, (as->pcm, hw_params, &buffer_time, &dir));

    dir = 0;
    CHECK_A(snd_pcm_hw_params_get_buffer_time, (hw_params, &buffer_time, &dir));
    CHECK_A(snd_pcm_hw_params, (as->pcm, hw_params));
    snd_pcm_hw_params_free(hw_params);

    CHECK_A(snd_pcm_sw_params_malloc, (&sw_params));
    CHECK_A(snd_pcm_sw_params_current, (as->pcm, sw_params));
    CHECK_A(snd_pcm_sw_params, (as->pcm, sw_params));
    CHECK_A(snd_pcm_prepare, (as->pcm));
    snd_pcm_sw_params_free(sw_params);

    CHECK_A(snd_pcm_prepare, (as->pcm));

    as->nfds = snd_pcm_poll_descriptors_count(as->pcm);
    as->fds = calloc(as->nfds, sizeof(struct pollfd));
    if (!as->fds) {
        trace_error("%s, memory allocation failure\n", __func__);
        goto err;
    }
    snd_pcm_poll_descriptors(as->pcm, as->fds, as->nfds);

    g_hash_table_insert(active_streams_ht, as, GINT_TO_POINTER(1));
    for (uintptr_t k = 0; k < as->nfds; k ++)
        g_hash_table_insert(stream_by_fd_ht, GINT_TO_POINTER(as->fds[k].fd), as);

    wakeup_audio_thread();

#undef CHECK_A

    return as;
err:
    free(as);
    return NULL;
}

static
audio_stream *
alsa_create_playback_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                            audio_stream_playback_cb_f *cb, void *cb_user_data)
{
    audio_stream *as = alsa_create_stream(STREAM_PLAYBACK, sample_rate, sample_frame_count);
    if (!as)
        return NULL;

    as->playback_cb = cb;
    as->cb_user_data = cb_user_data;
    return as;
}

static
audio_stream *
alsa_create_capture_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                           audio_stream_capture_cb_f *cb, void *cb_user_data)
{
    audio_stream *as = alsa_create_stream(STREAM_CAPTURE, sample_rate, sample_frame_count);

    if (!as)
        return NULL;

    as->capture_cb = cb;
    as->cb_user_data = cb_user_data;
    return as;
}

static
void
alsa_pause_stream(audio_stream *as, int enabled)
{
    g_atomic_int_set(&as->paused, enabled);
}

static
void
alsa_destroy_stream(audio_stream *as)
{
    pthread_mutex_lock(&lock);
    streams_to_delete = g_list_prepend(streams_to_delete, as);
    wakeup_audio_thread();
    pthread_mutex_unlock(&lock);
}

static
int
alsa_available(void)
{
    return 1;
}

audio_stream_ops audio_alsa = {
    .available =                alsa_available,
    .create_playback_stream =   alsa_create_playback_stream,
    .create_capture_stream =    alsa_create_capture_stream,
    .pause =                    alsa_pause_stream,
    .destroy =                  alsa_destroy_stream,
};
