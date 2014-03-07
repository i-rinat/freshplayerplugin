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

#include "ppb_audio.h"
#include <stddef.h>
#include "trace.h"

PP_Resource
ppb_audio_create(PP_Instance instance, PP_Resource config,
                 PPB_Audio_Callback_1_0 audio_callback, void *user_data)
{
    return 0;
}

PP_Bool
ppb_audio_is_audio(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Resource
ppb_audio_get_current_config(PP_Resource audio)
{
    return 0;
}

PP_Bool
ppb_audio_start_playback(PP_Resource audio)
{
    return PP_FALSE;
}

PP_Bool
ppb_audio_stop_playback(PP_Resource audio)
{
    return PP_FALSE;
}


// trace wrappers
static
PP_Resource
trace_ppb_audio_create(PP_Instance instance, PP_Resource config,
                       PPB_Audio_Callback_1_0 audio_callback, void *user_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_create(instance, config, audio_callback, user_data);
}

static
PP_Bool
trace_ppb_audio_is_audio(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_is_audio(resource);
}

static
PP_Resource
trace_ppb_audio_get_current_config(PP_Resource audio)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_get_current_config(audio);
}

static
PP_Bool
trace_ppb_audio_start_playback(PP_Resource audio)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_start_playback(audio);
}

static
PP_Bool
trace_ppb_audio_stop_playback(PP_Resource audio)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_stop_playback(audio);
}

const struct PPB_Audio_1_0 ppb_audio_interface_1_0 = {
    .Create =           trace_ppb_audio_create,
    .IsAudio =          trace_ppb_audio_is_audio,
    .GetCurrentConfig = trace_ppb_audio_get_current_config,
    .StartPlayback =    trace_ppb_audio_start_playback,
    .StopPlayback =     trace_ppb_audio_stop_playback,
};
