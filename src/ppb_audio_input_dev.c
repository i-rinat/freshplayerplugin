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

#include "ppb_audio_input_dev.h"
#include <stdlib.h>
#include "pp_resource.h"
#include "trace.h"


PP_Resource
ppb_audio_input_dev_create(PP_Instance instance)
{
    PP_Resource audio_input = pp_resource_allocate(PP_RESOURCE_AUDIO_INPUT, instance);
    return audio_input;
}

void
ppb_audio_input_dev_destroy(void *ptr)
{
}

PP_Bool
ppb_audio_input_dev_is_audio_input(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_AUDIO_INPUT;
}

int32_t
ppb_audio_input_dev_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                      struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_audio_input_dev_monitor_device_change(PP_Resource audio_input,
                                          PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    return 0;
}

int32_t
ppb_audio_input_dev_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback_0_2 audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_audio_input_dev_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback)
{
    return 0;
}

PP_Resource
ppb_audio_input_dev_get_current_config(PP_Resource audio_input)
{
    return 123005;
}

PP_Bool
ppb_audio_input_dev_start_capture(PP_Resource audio_input)
{
    return 0;
}

PP_Bool
ppb_audio_input_dev_stop_capture(PP_Resource audio_input)
{
    return 0;
}

void
ppb_audio_input_dev_close(PP_Resource audio_input)
{
    return;
}

// trace wrappers
static
PP_Resource
trace_ppb_audio_input_dev_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_input_dev_create(instance);
}

static
PP_Bool
trace_ppb_audio_input_dev_is_audio_input(PP_Resource resource)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_input_dev_is_audio_input(resource);
}

static
int32_t
trace_ppb_audio_input_dev_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                            struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_enumerate_devices(audio_input, output, callback);
}

static
int32_t
trace_ppb_audio_input_dev_monitor_device_change(PP_Resource audio_input,
                                                PP_MonitorDeviceChangeCallback callback,
                                                void *user_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_monitor_device_change(audio_input, callback, user_data);
}

static
int32_t
trace_ppb_audio_input_dev_open_0_3(PP_Resource audio_input, PP_Resource device_ref,
                                   PP_Resource config,
                                   PPB_AudioInput_Callback_0_2 audio_input_callback,
                                   void *user_data, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_open_0_3(audio_input, device_ref, config, audio_input_callback,
                                        user_data, callback);
}

static
int32_t
trace_ppb_audio_input_dev_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                               PPB_AudioInput_Callback audio_input_callback, void *user_data,
                               struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_open(audio_input, device_ref, config, audio_input_callback,
                                    user_data, callback);
}

static
PP_Resource
trace_ppb_audio_input_dev_get_current_config(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_get_current_config(audio_input);
}

static
PP_Bool
trace_ppb_audio_input_dev_start_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_start_capture(audio_input);
}

static
PP_Bool
trace_ppb_audio_input_dev_stop_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_dev_stop_capture(audio_input);
}

static
void
trace_ppb_audio_input_dev_close(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_audio_input_dev_close(audio_input);
}


const struct PPB_AudioInput_Dev_0_3 ppb_audio_input_dev_interface_0_3 = {
    .Create =           trace_ppb_audio_input_dev_create,
    .IsAudioInput =     trace_ppb_audio_input_dev_is_audio_input,
    .EnumerateDevices = trace_ppb_audio_input_dev_enumerate_devices,
    .MonitorDeviceChange = trace_ppb_audio_input_dev_monitor_device_change,
    .Open =             trace_ppb_audio_input_dev_open_0_3,
    .GetCurrentConfig = trace_ppb_audio_input_dev_get_current_config,
    .StartCapture =     trace_ppb_audio_input_dev_start_capture,
    .StopCapture =      trace_ppb_audio_input_dev_stop_capture,
    .Close =            trace_ppb_audio_input_dev_close
};

const struct PPB_AudioInput_Dev_0_4 ppb_audio_input_dev_interface_0_4 = {
    .Create =           trace_ppb_audio_input_dev_create,
    .IsAudioInput =     trace_ppb_audio_input_dev_is_audio_input,
    .EnumerateDevices = trace_ppb_audio_input_dev_enumerate_devices,
    .MonitorDeviceChange = trace_ppb_audio_input_dev_monitor_device_change,
    .Open =             trace_ppb_audio_input_dev_open,
    .GetCurrentConfig = trace_ppb_audio_input_dev_get_current_config,
    .StartCapture =     trace_ppb_audio_input_dev_start_capture,
    .StopCapture =      trace_ppb_audio_input_dev_stop_capture,
    .Close =            trace_ppb_audio_input_dev_close
};
