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
#include <stdlib.h>

extern audio_stream_ops audio_alsa;
extern audio_stream_ops audio_noaudio;
#if HAVE_PULSEAUDIO
extern audio_stream_ops audio_pulse;
#endif
#if HAVE_JACK
extern audio_stream_ops audio_jack;
#endif


audio_stream_ops *
audio_select_implementation(void)
{
#if HAVE_JACK
    if (audio_jack.available())
        return &audio_jack;
#endif
#if HAVE_PULSEAUDIO
    if (audio_pulse.available())
        return &audio_pulse;
#endif

    if (audio_alsa.available())
        return &audio_alsa;

    return &audio_noaudio;
}

void
audio_capture_device_list_free(audio_device_name *list)
{
    if (!list)
        return;

    for (uintptr_t k = 0; list[k].name != NULL; k ++) {
        free(list[k].name);
        free(list[k].longname);
    }
    free(list);
}
