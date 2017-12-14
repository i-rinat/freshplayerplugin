/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#pragma once

#include <stdint.h>

typedef enum {
    STREAM_PLAYBACK,
    STREAM_CAPTURE,
} audio_stream_direction;

typedef struct {
    char *name;
    char *longname;
} audio_device_name;

typedef struct audio_stream_s audio_stream;

typedef void
(audio_stream_capture_cb_f)(const void *buf, uint32_t sz, double latency, void *user_data);

typedef void
(audio_stream_playback_cb_f)(void *buf, uint32_t sz, double latency, void *user_data);

typedef int
(audio_available_f)(void);

typedef audio_stream *
(audio_create_playback_stream_f)(unsigned int sample_rate, unsigned int sample_frame_count,
                                 audio_stream_playback_cb_f *cb, void *cb_user_data);

typedef audio_stream *
(audio_create_capture_stream_f)(unsigned int sample_rate, unsigned int sample_frame_count,
                                audio_stream_capture_cb_f *cb, void *cb_user_data,
                                const char *device_longname);

/// returns NULL-terminated array of device names
///
/// caller should free memory by calling audio_capture_device_list_free()
typedef audio_device_name *
(audio_enumerate_capture_devices_f)(void);

typedef void
(audio_pause_stream_f)(audio_stream *s, int enabled);

typedef void
(audio_destroy_stream_f)(audio_stream *s);

typedef struct {
    audio_available_f                  *available;
    audio_create_playback_stream_f     *create_playback_stream;
    audio_create_capture_stream_f      *create_capture_stream;
    audio_enumerate_capture_devices_f  *enumerate_capture_devices;
    audio_pause_stream_f               *pause;
    audio_destroy_stream_f             *destroy;
} audio_stream_ops;


audio_stream_ops *
audio_select_implementation(void);

void
audio_capture_device_list_free(audio_device_name *list);
