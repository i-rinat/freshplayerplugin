/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include "audio_thread.h"
#include <pthread.h>
#include <unistd.h>
#include <glib.h>
#include "trace.h"
#include "config.h"
#include "utils.h"
#include "eintr_retry.h"
#include "ppb_message_loop.h"


struct audio_stream_s {
    size_t                      sample_frame_count;
    size_t                      sample_rate;
    size_t                      samples_wanted;
    audio_stream_playback_cb_f *playback_cb;
    void                       *cb_user_data;
    volatile int                paused;
    volatile int                alive;
};

static GHashTable      *active_streams_ht = NULL;
static volatile int     audio_thread_started = 0;
static volatile int     terminate_thread = 0;
static pthread_t        audio_thread_id;


static
void *
audio_thread(void *param)
{
    static char buf[16 * 1024];
    const size_t delay_ms = 20;

    ppb_message_loop_mark_thread_unsuitable();

    while (1) {
        if (g_atomic_int_get(&terminate_thread))
            goto quit;

        GList *streams = g_hash_table_get_keys(active_streams_ht);
        GList *ll = streams;
        while (ll) {
            struct audio_stream_s *as = ll->data;

            if (g_atomic_int_get(&as->alive)) {
                const size_t sample_size = 2 * sizeof(int16_t); // signed 16-bit stereo
                const size_t buf_size = MIN(as->sample_frame_count * sample_size, sizeof(buf));

                as->samples_wanted += as->sample_rate * delay_ms / 1000;

                while (as->samples_wanted * sample_size > buf_size) {
                    as->playback_cb(buf, buf_size, 0, as->cb_user_data);
                    as->samples_wanted -= buf_size / sample_size;
                }

            } else {
                g_hash_table_remove(active_streams_ht, as);
                free(as);
            }

            ll = g_list_next(ll);
        }

        g_list_free(streams);

        usleep(delay_ms * 1000);
    }

quit:
    return NULL;
}

static
void
__attribute__((constructor))
constructor_audio_thread_noaudio(void)
{
    active_streams_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
}

static
void
__attribute__((destructor))
destructor_audio_thread_noaudio(void)
{
    if (g_atomic_int_get(&audio_thread_started)) {
        g_atomic_int_set(&terminate_thread, 1);
        pthread_join(audio_thread_id, NULL);
    }

    g_hash_table_unref(active_streams_ht);
}

static
audio_stream *
noaudio_create_stream(unsigned int sample_rate, unsigned int sample_frame_count)
{
    audio_stream *as;

    if (!g_atomic_int_get(&audio_thread_started)) {
        pthread_create(&audio_thread_id, NULL, audio_thread, NULL);
        g_atomic_int_set(&audio_thread_started, 1);
    }

    as = calloc(1, sizeof(*as));
    if (!as)
        goto err;

    as->sample_rate = sample_rate;
    as->sample_frame_count = sample_frame_count;
    g_atomic_int_set(&as->paused, 1);
    g_atomic_int_set(&as->alive, 1);

    g_hash_table_insert(active_streams_ht, as, GINT_TO_POINTER(1));

    return as;

err:
    free(as);
    return NULL;
}

static
audio_stream *
noaudio_create_playback_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                               audio_stream_playback_cb_f *cb, void *cb_user_data)
{
    audio_stream *as = noaudio_create_stream(sample_rate, sample_frame_count);
    if (!as)
        return NULL;

    as->playback_cb = cb;
    as->cb_user_data = cb_user_data;
    return as;
}

static
audio_stream *
noaudio_create_capture_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                              audio_stream_capture_cb_f *cb, void *cb_user_data,
                              const char *longname)
{
    return NULL;
}

static
audio_device_name *
noaudio_enumerate_capture_devices(void)
{
    // empty list, with ending NULL
    return calloc(sizeof(audio_device_name), 1);
}

static
void
noaudio_pause_stream(audio_stream *as, int enabled)
{
    g_atomic_int_set(&as->paused, enabled);
}

static
void
noaudio_destroy_stream(audio_stream *as)
{
    // mark as non-alive. Worker thread will free memory
    g_atomic_int_set(&as->alive, 0);
}

static
int
noaudio_available(void)
{
    // since noaudio is an artificial backend, it's always available
    return 1;
}

audio_stream_ops audio_noaudio = {
    .available =                    noaudio_available,
    .create_playback_stream =       noaudio_create_playback_stream,
    .create_capture_stream =        noaudio_create_capture_stream,
    .enumerate_capture_devices =    noaudio_enumerate_capture_devices,
    .pause =                        noaudio_pause_stream,
    .destroy =                      noaudio_destroy_stream,
};
