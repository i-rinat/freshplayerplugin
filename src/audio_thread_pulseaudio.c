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
#include <pulse/pulseaudio.h>
#include <pulse/mainloop.h>
#include <glib.h>
#include <string.h>
#include "trace.h"


#define CLIENT_NAME     "freshwrapper"


struct audio_stream_s {
    struct pa_sample_spec       sample_spec;
    audio_stream_direction      direction;
    size_t                      sample_frame_count;
    pa_stream                  *stream;
    audio_stream_playback_cb_f *playback_cb;
    audio_stream_capture_cb_f  *capture_cb;
    void                       *cb_user_data;
    volatile int                paused;
};


static pthread_mutex_t              lock = PTHREAD_MUTEX_INITIALIZER;
static int                          probed = 0;
static int                          available = 0;
static struct pa_threaded_mainloop *mainloop;
static struct pa_context           *context;


static
void
pulse_context_state_cb(pa_context *c, void *user_data)
{
    switch (pa_context_get_state(c)) {
    case PA_CONTEXT_READY:
    case PA_CONTEXT_TERMINATED:
    case PA_CONTEXT_FAILED:
        pa_threaded_mainloop_signal(mainloop, 0);
        break;
    default:
        break;
    }
}

static
void
__attribute__((destructor))
destructor_audio_thread_pulseaudio(void)
{
    pthread_mutex_lock(&lock);
    if (probed && available) {
        pa_threaded_mainloop_stop(mainloop);
        pa_context_disconnect(context);
        pa_context_unref(context);
        context = NULL;
        pa_threaded_mainloop_free(mainloop);
        mainloop = NULL;
        probed = available = 0;
    }
    pthread_mutex_unlock(&lock);
}

static
int
pulse_available(void)
{
    int ret;
    pthread_mutex_lock(&lock);
    if (probed) {
        ret = available;
        pthread_mutex_unlock(&lock);
        return ret;
    }

    probed = 1;
    available = 0;

    mainloop = pa_threaded_mainloop_new();
    if (!mainloop) {
        trace_error("%s, can't create mainloop object\n", __func__);
        goto err_1;
    }

    context = pa_context_new(pa_threaded_mainloop_get_api(mainloop), CLIENT_NAME);
    if (!context) {
        trace_error("%s, can't create context\n", __func__);
        goto err_2;
    }

    pa_context_set_state_callback(context, pulse_context_state_cb, NULL);

    if (pa_context_connect(context, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        trace_info_f("%s, no PulseAudio server available\n", __func__);
        goto err_3;
    }

    pa_threaded_mainloop_lock(mainloop);

    if (pa_threaded_mainloop_start(mainloop) < 0) {
        trace_error("%s, can't start mainloop\n", __func__);
        goto err_4;
    }

    pa_threaded_mainloop_wait(mainloop);

    if (pa_context_get_state(context) != PA_CONTEXT_READY) {
        trace_error("%s, context not ready\n", __func__);
        goto err_4;
    }

    pa_threaded_mainloop_unlock(mainloop);
    available = 1;
    pthread_mutex_unlock(&lock);
    return 1;

err_4:
    pa_threaded_mainloop_unlock(mainloop);
err_3:
    pa_context_unref(context);
err_2:
    pa_threaded_mainloop_free(mainloop);
err_1:
    pthread_mutex_unlock(&lock);
    return 0;
}

static
void
pulse_stream_state_cb(pa_stream *s, void *user_data)
{
    switch (pa_stream_get_state(s)) {
    case PA_STREAM_READY:
    case PA_STREAM_FAILED:
    case PA_STREAM_TERMINATED:
        pa_threaded_mainloop_signal(mainloop, 0);
        break;
    default:
        break;
    }
}

static
void
pulse_stream_write_cb(pa_stream *s, size_t length, void *user_data)
{
    audio_stream   *as = user_data;
    char           *buf;

    pa_stream_begin_write(as->stream, (void **)&buf, &length);

    if (g_atomic_int_get(&as->paused) || !as->playback_cb) {
        memset(buf, 0, length);
    } else {
        const size_t max_segment_length = as->sample_frame_count * pa_frame_size(&as->sample_spec);
        size_t to_process = length;
        size_t ofs = 0;

        while (to_process > 0) {
            const size_t segment_length = MIN(to_process, max_segment_length);

            as->playback_cb(buf + ofs, segment_length, 0, as->cb_user_data);

            to_process -= segment_length;
            ofs += segment_length;
        }
    }

    pa_stream_write(as->stream, buf, length, NULL, 0, PA_SEEK_RELATIVE);
}

static
void
pulse_stream_read_cb(pa_stream *s, size_t length, void *user_data)
{
    audio_stream   *as = user_data;
    const char     *data;

    if (pa_stream_peek(s, (const void **)&data, &length) < 0) {
        trace_error("%s, pa_stream_peek failed\n", __func__);
        return;
    }

    if (!g_atomic_int_get(&as->paused)) {
        const size_t max_segment_length = as->sample_frame_count * pa_frame_size(&as->sample_spec);
        size_t to_process = length;
        size_t ofs = 0;

        while (to_process > 0) {
            const size_t segment_length = MIN(to_process, max_segment_length);

            as->capture_cb(data + ofs, segment_length, 0, as->cb_user_data);

            to_process -= segment_length;
            ofs += segment_length;
        }
    }

    pa_stream_drop(s);
}

static
void
pulse_stream_latency_update_cb(pa_stream *s, void *user_data)
{
    pa_threaded_mainloop_signal(mainloop, 0);
}

static
audio_stream *
pulse_do_create_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                       audio_stream_playback_cb_f *playback_cb,
                       audio_stream_capture_cb_f  *capture_cb, void *cb_user_data,
                       audio_stream_direction direction)
{
    // ensure main loop is running
    if (!pulse_available()) {
        trace_error("%s, no PulseAudio server available\n", __func__);
        return NULL;
    }

    audio_stream *as = calloc(1, sizeof(*as));
    if (!as)
        return NULL;

    as->playback_cb = playback_cb;
    as->capture_cb =  capture_cb;

    as->cb_user_data =  cb_user_data;

    // capture streams are monophonic
    as->sample_spec.channels =  (direction == STREAM_PLAYBACK) ? 2 : 1;
    as->sample_spec.rate =      sample_rate;
    as->sample_spec.format =    PA_SAMPLE_S16LE;

    as->direction =          direction;
    as->sample_frame_count = sample_frame_count;
    g_atomic_int_set(&as->paused, 1);

    pa_threaded_mainloop_lock(mainloop);

    const char *stream_name = (direction == STREAM_PLAYBACK) ? "playback" : "capture";
    as->stream = pa_stream_new(context, stream_name, &as->sample_spec, NULL);
    if (!as->stream) {
        if (direction == STREAM_PLAYBACK)
            trace_error("%s, can't create playback stream\n", __func__);
        else
            trace_error("%s, can't create capture stream\n", __func__);
        goto err_1;
    }

    pa_stream_set_state_callback(as->stream, pulse_stream_state_cb, as);
    pa_stream_set_read_callback(as->stream, pulse_stream_read_cb, as);
    pa_stream_set_write_callback(as->stream, pulse_stream_write_cb, as);
    pa_stream_set_latency_update_callback(as->stream, pulse_stream_latency_update_cb, as);

    const size_t frame_size = pa_frame_size(&as->sample_spec);
    pa_buffer_attr buf_attr = {
        .maxlength =    (uint32_t)-1,
        .tlength =      sample_frame_count * frame_size * 2,
        .prebuf =       (uint32_t)-1,
        .minreq =       sample_frame_count * frame_size / 2,
        .fragsize =     sample_frame_count * frame_size,
    };

    if (direction == STREAM_PLAYBACK) {
        if (pa_stream_connect_playback(as->stream, NULL, &buf_attr, 0, NULL, NULL) < 0) {
            trace_error("%s, can't connect playback stream\n", __func__);
            goto err_2;
        }
    } else {
        if (pa_stream_connect_record(as->stream, NULL, &buf_attr, 0) < 0) {
            trace_error("%s, can't connect capture stream\n", __func__);
            goto err_2;
        }
    }

    while (1) {
        pa_stream_state_t state = pa_stream_get_state(as->stream);
        if (state == PA_STREAM_READY)
            break;
        if (!PA_STREAM_IS_GOOD(state)) {
            trace_error("%s, stream is not ready\n", __func__);
            goto err_2;
        }
        pa_threaded_mainloop_wait(mainloop);
    }

    pa_threaded_mainloop_unlock(mainloop);
    return as;

err_2:
    pa_stream_unref(as->stream);
err_1:
    pa_threaded_mainloop_unlock(mainloop);
    free(as);
    return NULL;
}


static
audio_stream *
pulse_create_playback_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                             audio_stream_playback_cb_f *cb, void *cb_user_data)
{
    return pulse_do_create_stream(sample_rate, sample_frame_count, cb, NULL, cb_user_data,
                                  STREAM_PLAYBACK);
}

static
audio_stream *
pulse_create_capture_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                            audio_stream_capture_cb_f *cb, void *cb_user_data,
                            const char *longname)
{
    (void)longname; // have no meaning here. PulseAudio itself selects appropriate sound device
    return pulse_do_create_stream(sample_rate, sample_frame_count, NULL, cb, cb_user_data,
                                  STREAM_CAPTURE);
}

static
audio_device_name *
pulse_enumerate_capture_devices(void)
{
    audio_device_name *list = malloc(sizeof(audio_device_name) * 2);
    if (!list)
        return NULL;

    list[0].name = strdup("PulseAudio capture device");
    list[0].longname = strdup(list[0].name);

    list[1].name = NULL;
    list[1].longname = NULL;

    return list;
}

static
void
pulse_pause_stream(audio_stream *as, int enabled)
{
    g_atomic_int_set(&as->paused, enabled);
}

static
void
pulse_stream_success_cb(pa_stream *s, int success, void *user_data)
{
    pa_threaded_mainloop_signal(mainloop, 0);
}

static
void
pulse_wait_for_completion(pa_operation *op, pa_threaded_mainloop *ml)
{
    if (!op) {
        trace_error("%s, operation is NULL\n", __func__);
        return;
    }

    while (pa_operation_get_state(op) == PA_OPERATION_RUNNING)
        pa_threaded_mainloop_wait(ml);
    pa_operation_unref(op);
}

static
void
pulse_destroy_stream(audio_stream *as)
{
    pa_threaded_mainloop_lock(mainloop);

    pa_operation *op = pa_stream_cork(as->stream, 1, pulse_stream_success_cb, mainloop);
    pulse_wait_for_completion(op, mainloop);

    pa_stream_disconnect(as->stream);
    pa_stream_set_state_callback(as->stream, NULL, NULL);
    pa_stream_set_write_callback(as->stream, NULL, NULL);
    pa_stream_set_latency_update_callback(as->stream, NULL, NULL);
    pa_stream_unref(as->stream);

    pa_threaded_mainloop_unlock(mainloop);
    free(as);
}


audio_stream_ops audio_pulse = {
    .available =                    pulse_available,
    .create_playback_stream =       pulse_create_playback_stream,
    .create_capture_stream =        pulse_create_capture_stream,
    .enumerate_capture_devices =    pulse_enumerate_capture_devices,
    .pause =                        pulse_pause_stream,
    .destroy =                      pulse_destroy_stream,
};
