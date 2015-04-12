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
#include <jack/jack.h>
#include <jack/ringbuffer.h>
#include <soxr.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "trace.h"
#include "config.h"

#define CLIENT_NAME     "freshwrapper"

#define CMD_RESAMPLE_NEXT_CHUNK     ((void *)1)
#define CMD_TERMINATE               ((void *)2)


struct audio_stream_s {
    audio_stream_playback_cb_f *playback_cb;
    void                       *cb_user_data;

    GAsyncQueue        *async_q;
    pthread_t           resampler_thread;
    jack_client_t      *client;
    jack_port_t        *output_port_1;
    jack_port_t        *output_port_2;
    size_t              sample_rate;
    size_t              sample_frame_count;
    size_t              jack_sample_rate;
    size_t              jack_sample_frame_count;
    void               *input_buf;
    size_t              input_buf_size;
    void               *output_bufs[2];
    size_t              output_buf_size;
    volatile int        paused;
    soxr_t              resampler;
    jack_ringbuffer_t  *rb_out[2];
};

static
int
ja_available(void)
{
    return config.audio_use_jack;
}

static
void *
ja_resampler_thread_func(void *param)
{
    audio_stream *as = param;

    while (1) {
        while (jack_ringbuffer_read_space(as->rb_out[0]) < as->output_buf_size / 2) {
            if (g_atomic_int_get(&as->paused)) {
                memset(as->input_buf, 0, as->input_buf_size);
            } else {
                as->playback_cb(as->input_buf, as->input_buf_size, 0, as->cb_user_data);
            }

            size_t idone = 0, odone = 0;
            soxr_process(as->resampler, as->input_buf, as->sample_frame_count, &idone,
                         as->output_bufs, as->output_buf_size / sizeof(float), &odone);

            size_t wr1, wr2;
            wr1 = jack_ringbuffer_write(as->rb_out[0], as->output_bufs[0], odone * sizeof(float));
            wr2 = jack_ringbuffer_write(as->rb_out[1], as->output_bufs[1], odone * sizeof(float));
            if (wr1 != odone * sizeof(float) || wr2 != odone * sizeof(float))
                trace_error("%s, ringbuffer overrun\n", __func__);
        }

        void *ptr = NULL;
        while (ptr == NULL)
            ptr = g_async_queue_timeout_pop(as->async_q, 500 * 1000 /* ms */);

        // termination condition
        if (ptr == CMD_TERMINATE)
            break;
    }

    return NULL;
}

static
int
ja_process_cb(jack_nframes_t nframes, void *param)
{
    audio_stream *as = param;

    void *out[2] = {
        jack_port_get_buffer(as->output_port_1, nframes),
        jack_port_get_buffer(as->output_port_2, nframes),
    };

    size_t wr1 = jack_ringbuffer_read(as->rb_out[0], out[0], nframes * sizeof(float));
    size_t wr2 = jack_ringbuffer_read(as->rb_out[1], out[1], nframes * sizeof(float));

    if (wr1 != nframes * sizeof(float) || wr2 != nframes * sizeof(float))
        trace_error("%s, ringbuffer underrun\n", __func__);

    g_async_queue_push(as->async_q, CMD_RESAMPLE_NEXT_CHUNK);
    return 0;
}

static
audio_stream *
ja_create_playback_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                          audio_stream_playback_cb_f *cb, void *cb_user_data)
{
    jack_options_t  options = JackNullOption;
    jack_status_t   status;
    const char     *server_name = NULL; // TODO: make server name configurable

    audio_stream *as = calloc(1, sizeof(*as));
    if (!as) {
        trace_error("%s, memory allocation failure, point 1\n", __func__);
        goto err_1;
    }

    as->playback_cb = cb;
    as->cb_user_data = cb_user_data;
    g_atomic_int_set(&as->paused, 1);

    as->client = jack_client_open(CLIENT_NAME, options, &status, server_name);
    if (!as->client) {
        trace_error("%s, jack_client_open() failed with status=0x%x\n", __func__, status);
        if (status & JackServerFailed)
            trace_error("%s, can't connect to JACK server\n", __func__);

        goto err_2;
    }

    as->sample_rate =             sample_rate;
    as->sample_frame_count =      sample_frame_count;
    as->jack_sample_rate =        jack_get_sample_rate(as->client);
    as->jack_sample_frame_count = ceil(1.0 * as->jack_sample_rate / as->sample_rate *
                                       as->sample_frame_count);

    // allocate buffer for sample_frame_count frames
    as->input_buf_size = 2 * sizeof(int16_t) * sample_frame_count;
    as->input_buf = malloc(as->input_buf_size);

    // allocate room for twice as long (in seconds) buffer as as->input_buf
    as->output_buf_size = (2 * as->jack_sample_frame_count) * sizeof(float);
    as->output_bufs[0] = malloc(as->output_buf_size);
    as->output_bufs[1] = malloc(as->output_buf_size);
    as->rb_out[0] = jack_ringbuffer_create(as->output_buf_size);
    as->rb_out[1] = jack_ringbuffer_create(as->output_buf_size);

    if (!as->input_buf || !as->output_bufs[0] || !as->output_bufs[1]
        || !as->rb_out[0] || !as->rb_out[1])
    {
        trace_error("%s, memory allocation failure, point 2\n", __func__);
        goto err_3;
    }

    soxr_quality_spec_t quality_spec = soxr_quality_spec(SOXR_QQ, 0);
    soxr_io_spec_t      io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_FLOAT32_S);

    if (as->sample_rate == as->jack_sample_rate) {
        // workaround issue with same input and output rate
        quality_spec = soxr_quality_spec(SOXR_QQ, 0);
    }

    soxr_error_t soxr_err;
    as->resampler = soxr_create(as->sample_rate, as->jack_sample_rate, 2, &soxr_err, &io_spec,
                                &quality_spec, NULL);
    if (soxr_err != NULL) {
        trace_error("%s, can't create resampler: %s\n", __func__, soxr_strerror(soxr_err));
        goto err_3;
    }

    as->async_q = g_async_queue_new();
    if (!as->async_q) {
        trace_error("%s, can't create GAsyncQueue\n", __func__);
        goto err_4;
    }

    jack_set_process_callback(as->client, ja_process_cb, as);

    as->output_port_1 = jack_port_register(as->client, "output1", JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput, 0);
    as->output_port_2 = jack_port_register(as->client, "output2", JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput, 0);

    if (!as->output_port_1 || !as->output_port_2) {
        trace_error("%s, can't register output ports\n", __func__);
        goto err_5;
    }

    pthread_create(&as->resampler_thread, NULL, ja_resampler_thread_func, as);

    if (jack_activate(as->client) != 0) {
        trace_error("%s, can't activate client\n", __func__);
        goto err_6;
    }

    const char **ports = jack_get_ports(as->client, NULL, NULL,
                                        JackPortIsPhysical | JackPortIsInput);
    if (!ports) {
        trace_error("%s, no physical playback ports\n", __func__);
        goto err_6;
    }

    if (jack_connect(as->client, jack_port_name(as->output_port_1), ports[0]) != 0)
        trace_error("%s, can't connect output port 1\n", __func__);
    if (jack_connect(as->client, jack_port_name(as->output_port_2), ports[1]) != 0)
        trace_error("%s, can't connect output port 2\n", __func__);

    jack_free(ports);
    return as;

err_6:
    g_async_queue_push(as->async_q, CMD_TERMINATE);
    pthread_join(as->resampler_thread, NULL);
err_5:
    g_async_queue_unref(as->async_q);
err_4:
    soxr_delete(as->resampler);
err_3:
    if (as->rb_out[0])
        jack_ringbuffer_free(as->rb_out[0]);
    if (as->rb_out[1])
        jack_ringbuffer_free(as->rb_out[1]);
    free(as->input_buf);
    free(as->output_bufs[0]);
    free(as->output_bufs[1]);

    jack_client_close(as->client);
err_2:
    free(as);
err_1:
    return NULL;
}

static
audio_stream *
ja_create_capture_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                         audio_stream_capture_cb_f *cb, void *cb_user_data)
{
    // TODO: create capture stream
    return NULL;
}

static
char **
ja_enumerate_capture_devices(void)
{
    char **list = malloc(sizeof(char *) * 2);
    if (!list)
        return NULL;
    list[0] = strdup("JACK capture device");
    list[1] = NULL;
    return list;
}

static
void
ja_pause_stream(audio_stream *as, int enabled)
{
    g_atomic_int_set(&as->paused, enabled);
}

static
void
ja_destroy_stream(audio_stream *as)
{
    jack_client_close(as->client);
    g_async_queue_push(as->async_q, CMD_TERMINATE);
    pthread_join(as->resampler_thread, NULL);
    g_async_queue_unref(as->async_q);
    soxr_delete(as->resampler);
    free(as->input_buf);
}


audio_stream_ops audio_jack = {
    .available =                    ja_available,
    .create_playback_stream =       ja_create_playback_stream,
    .create_capture_stream =        ja_create_capture_stream,
    .enumerate_capture_devices =    ja_enumerate_capture_devices,
    .pause =                        ja_pause_stream,
    .destroy =                      ja_destroy_stream,
};
