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

#ifndef FPP_AUDIO_THREAD_H
#define FPP_AUDIO_THREAD_H

#include <stdint.h>


typedef enum {
    STREAM_PLAYBACK,
    STREAM_CAPTURE,
} audio_stream_direction;

typedef struct audio_stream_s audio_stream;

typedef void (audio_stream_capture_cb_f)(const void *buf, uint32_t sz, double latency,
                                         void *user_data);
typedef void (audio_stream_playback_cb_f)(void *buf, uint32_t sz, double latency, void *user_data);


audio_stream *
audio_create_playback_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                             audio_stream_playback_cb_f *cb, void *cb_user_data);

audio_stream *
audio_create_capture_stream(unsigned int sample_rate, unsigned int sample_frame_count,
                            audio_stream_capture_cb_f *cb, void *cb_user_data);

void
audio_pause_stream(audio_stream *s, int enabled);

void
audio_destroy_stream(audio_stream *s);


#endif // FPP_AUDIO_THREAD_H
