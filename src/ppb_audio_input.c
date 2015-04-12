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

#include "ppb_audio_input.h"
#include "ppb_device_ref.h"
#include "ppb_var.h"
#include <stdlib.h>
#include <ppapi/c/pp_errors.h>
#include "ppb_core.h"
#include "pp_resource.h"
#include "trace.h"
#include "tables.h"


PP_Resource
ppb_audio_input_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource audio_input = pp_resource_allocate(PP_RESOURCE_AUDIO_INPUT, pp_i);
    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, resource allocation failed\n", __func__);
        return 0;
    }

    ai->stream_ops = audio_select_implementation();

    pp_resource_release(audio_input);
    return audio_input;
}

void
ppb_audio_input_destroy(void *ptr)
{
}

PP_Bool
ppb_audio_input_is_audio_input(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_AUDIO_INPUT;
}

int32_t
ppb_audio_input_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                  struct PP_CompletionCallback callback)
{
    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_FAILED;
    }

    char **list = ai->stream_ops->enumerate_capture_devices();
    size_t cnt = 0;
    while (list && list[cnt])
        cnt ++;

    PP_Resource *refs = output.GetDataBuffer(output.user_data, cnt, sizeof(PP_Resource));
    for (uintptr_t k = 0; k < cnt; k ++) {
        refs[k] = ppb_device_ref_create(ai->instance->id, ppb_var_var_from_utf8_z(list[k]),
                                        PP_DEVICETYPE_DEV_AUDIOCAPTURE);
    }

    audio_capture_device_list_free(list);
    ppb_core_call_on_main_thread2(0, callback, PP_OK, __func__);
    pp_resource_release(audio_input);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_audio_input_monitor_device_change(PP_Resource audio_input,
                                      PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    return 0;
}

int32_t
ppb_audio_input_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback_0_3 audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                     PPB_AudioInput_Callback audio_input_callback, void *user_data,
                     struct PP_CompletionCallback callback)
{
    return 0;
}

PP_Resource
ppb_audio_input_get_current_config(PP_Resource audio_input)
{
    return 123005;
}

PP_Bool
ppb_audio_input_start_capture(PP_Resource audio_input)
{
    return 0;
}

PP_Bool
ppb_audio_input_stop_capture(PP_Resource audio_input)
{
    return 0;
}

void
ppb_audio_input_close(PP_Resource audio_input)
{
    ppb_core_release_resource(audio_input);
    return;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_audio_input_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_input_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_is_audio_input(PP_Resource resource)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_input_is_audio_input(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {fake} %s\n", __func__+6);
    return ppb_audio_input_enumerate_devices(audio_input, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_monitor_device_change(PP_Resource audio_input,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_monitor_device_change(audio_input, callback, user_data);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                               PPB_AudioInput_Callback_0_3 audio_input_callback, void *user_data,
                               struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_open_0_3(audio_input, device_ref, config, audio_input_callback,
                                    user_data, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                           PPB_AudioInput_Callback audio_input_callback, void *user_data,
                           struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_open(audio_input, device_ref, config, audio_input_callback, user_data,
                                callback);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_audio_input_get_current_config(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_get_current_config(audio_input);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_start_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_start_capture(audio_input);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_stop_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_input_stop_capture(audio_input);
}

TRACE_WRAPPER
void
trace_ppb_audio_input_close(PP_Resource audio_input)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    ppb_audio_input_close(audio_input);
}


const struct PPB_AudioInput_Dev_0_3 ppb_audio_input_dev_interface_0_3 = {
    .Create =               TWRAPF(ppb_audio_input_create),
    .IsAudioInput =         TWRAPF(ppb_audio_input_is_audio_input),
    .EnumerateDevices =     TWRAPF(ppb_audio_input_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_audio_input_monitor_device_change),
    .Open =                 TWRAPZ(ppb_audio_input_open_0_3),
    .GetCurrentConfig =     TWRAPZ(ppb_audio_input_get_current_config),
    .StartCapture =         TWRAPZ(ppb_audio_input_start_capture),
    .StopCapture =          TWRAPZ(ppb_audio_input_stop_capture),
    .Close =                TWRAPF(ppb_audio_input_close),
};

const struct PPB_AudioInput_Dev_0_4 ppb_audio_input_dev_interface_0_4 = {
    .Create =               TWRAPF(ppb_audio_input_create),
    .IsAudioInput =         TWRAPF(ppb_audio_input_is_audio_input),
    .EnumerateDevices =     TWRAPF(ppb_audio_input_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_audio_input_monitor_device_change),
    .Open =                 TWRAPZ(ppb_audio_input_open),
    .GetCurrentConfig =     TWRAPZ(ppb_audio_input_get_current_config),
    .StartCapture =         TWRAPZ(ppb_audio_input_start_capture),
    .StopCapture =          TWRAPZ(ppb_audio_input_stop_capture),
    .Close =                TWRAPF(ppb_audio_input_close),
};
