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
#include <soxr.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include "trace.h"
#include "config.h"

#define CLIENT_NAME     "freshwrapper"


struct audio_stream_s {
    audio_stream_playback_cb_f *playback_cb;
    void                       *cb_user_data;

    jack_client_t  *client;
    jack_port_t    *output_port_1;
    jack_port_t    *output_port_2;
    size_t          sample_rate;
    size_t          sample_frame_count;
    size_t          jack_sample_rate;
    volatile int    paused;
    soxr_t          resampler;
    char           *input_buf;
    size_t          input_buf_available_frames;
};

static
int
ja_available(void)
{
    return config.audio_use_jack;
}

static
int
ja_process_cb(jack_nframes_t nframes, void *param)
{
    audio_stream   *as = param;

    jack_default_audio_sample_t *out[2] = {
        jack_port_get_buffer(as->output_port_1, nframes),
        jack_port_get_buffer(as->output_port_2, nframes),
    };

    if (g_atomic_int_get(&as->paused)) {
        memset(out[0], 0, nframes * sizeof(jack_default_audio_sample_t));
        memset(out[1], 0, nframes * sizeof(jack_default_audio_sample_t));
        return 0;
    }

    size_t frames_to_write = nframes;
    while (frames_to_write > 0) {
        const size_t frame_size = 2 * sizeof(int16_t);

        // ensure input buffer is at least half-filled
        if (as->input_buf_available_frames <= as->sample_frame_count) {
            // amount of data in the buffer will always be smaller than (2 * sample_frame_count)
            as->playback_cb(as->input_buf + frame_size * as->input_buf_available_frames,
                            frame_size * as->sample_frame_count, 0, as->cb_user_data);
            as->input_buf_available_frames += as->sample_frame_count;
        }

        // resample and deinterleave
        size_t idone = 0;
        size_t odone = 0;
        soxr_process(as->resampler, as->input_buf, as->input_buf_available_frames, &idone,
                     out, frames_to_write, &odone);

        // move remaining data in input buffer to its beginning
        as->input_buf_available_frames -= idone;
        memmove(as->input_buf, as->input_buf + frame_size * idone,
                frame_size * as->input_buf_available_frames);

        // advance pointers
        out[0] += odone;
        out[1] += odone;
        frames_to_write -= odone;
    }

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
    if (!as)
        goto err_1;

    // allocate buffer for (2 * sample_frame_count) frames
    as->input_buf =  malloc(2 * sizeof(int16_t) * 2 * sample_frame_count);
    as->input_buf_available_frames = 0;

    if (!as->input_buf)
        goto err_2;

    as->playback_cb =   cb;
    as->cb_user_data =  cb_user_data;
    as->sample_frame_count = sample_frame_count;

    as->client = jack_client_open(CLIENT_NAME, options, &status, server_name);
    if (!as->client) {
        trace_error("%s, jack_client_open() failed with status=0x%x\n", __func__, status);
        if (status & JackServerFailed) {
            trace_error("%s, can't connect to JACK server\n", __func__);
        }

        goto err_3;
    }

    as->sample_rate =      sample_rate;
    as->jack_sample_rate = jack_get_sample_rate(as->client);

    soxr_quality_spec_t quality_spec = soxr_quality_spec(SOXR_QQ, 0);
    soxr_io_spec_t      io_spec = soxr_io_spec(SOXR_INT16_I, SOXR_FLOAT32_S);

    if (as->sample_rate == as->jack_sample_rate) {
        // workaround issue with same input and output rate
        quality_spec = soxr_quality_spec(SOXR_QQ, 0);
    }

    as->resampler = soxr_create(as->sample_rate, as->jack_sample_rate, 2, NULL, &io_spec,
                                &quality_spec, NULL);

    jack_set_process_callback(as->client, ja_process_cb, as);

    as->output_port_1 = jack_port_register(as->client, "output1", JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput, 0);
    as->output_port_2 = jack_port_register(as->client, "output2", JACK_DEFAULT_AUDIO_TYPE,
                                           JackPortIsOutput, 0);

    if (!as->output_port_1 || !as->output_port_2) {
        trace_error("%s, can't register output ports\n", __func__);
        goto err_4;
    }

    if (jack_activate(as->client) != 0) {
        trace_error("%s, can't activate client\n", __func__);
        goto err_4;
    }

    const char **ports = jack_get_ports(as->client, NULL, NULL,
                                        JackPortIsPhysical | JackPortIsInput);
    if (!ports) {
        trace_error("%s, no physical playback ports\n", __func__);
        goto err_4;
    }

    if (jack_connect(as->client, jack_port_name(as->output_port_1), ports[0]) != 0)
        trace_error("%s, can't connect output port 1\n", __func__);
    if (jack_connect(as->client, jack_port_name(as->output_port_2), ports[1]) != 0)
        trace_error("%s, can't connect output port 2\n", __func__);

    jack_free(ports);
    return as;

err_4:
    jack_client_close(as->client);
err_3:
    free(as->input_buf);
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
