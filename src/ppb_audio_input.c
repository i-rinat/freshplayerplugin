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

#include "audio_thread.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_audio_config.h"
#include "ppb_audio_input.h"
#include "ppb_core.h"
#include "ppb_device_ref.h"
#include "ppb_instance.h"
#include "ppb_message_loop.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>

struct pp_audio_input_s {
    COMMON_STRUCTURE_FIELDS
    uint32_t                    sample_rate;
    uint32_t                    sample_frame_count;
    PPB_AudioInput_Callback_0_3 cb_0_3;
    PPB_AudioInput_Callback     cb_0_4;
    void                       *cb_user_data;
    audio_stream_ops           *stream_ops;
    audio_stream               *stream;
};

STATIC_ASSERT(sizeof(struct pp_audio_input_s) <= LARGEST_RESOURCE_SIZE);

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

static
void
ppb_audio_input_destroy(void *ptr)
{
    struct pp_audio_input_s *ai = ptr;
    if (ai->stream)
        ai->stream_ops->destroy(ai->stream);
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

    audio_device_name *list = ai->stream_ops->enumerate_capture_devices();
    size_t cnt = 0;
    while (list && list[cnt].name)
        cnt ++;

    PP_Resource *refs = output.GetDataBuffer(output.user_data, cnt, sizeof(PP_Resource));
    for (uintptr_t k = 0; k < cnt; k ++) {
        struct PP_Var name =     ppb_var_var_from_utf8_z(list[k].name);
        struct PP_Var longname = ppb_var_var_from_utf8_z(list[k].longname);
        refs[k] = ppb_device_ref_create(ai->instance->id, name, longname,
                                        PP_DEVICETYPE_DEV_AUDIOCAPTURE);
        ppb_var_release(name);
        ppb_var_release(longname);
    }

    audio_capture_device_list_free(list);
    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, PP_OK, 0,
                                           __func__);
    pp_resource_release(audio_input);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_audio_input_monitor_device_change(PP_Resource audio_input,
                                      PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    return 0;
}

static
void
capture_cb(const void *buf, uint32_t sz, double latency, void *user_data)
{
    struct pp_audio_input_s *ai = user_data;

    if (ai->cb_0_3) {
        ai->cb_0_3(buf, sz, ai->cb_user_data);
    } else if (ai->cb_0_4) {
        ai->cb_0_4(buf, sz, latency, ai->cb_user_data);
    }
}

static
int32_t
do_ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                        PPB_AudioInput_Callback_0_3 audio_input_callback_0_3,
                        PPB_AudioInput_Callback audio_input_callback_0_4, void *user_data,
                        struct PP_CompletionCallback callback)
{
    int32_t         retval = PP_ERROR_FAILED;
    const char     *longname = NULL;
    struct PP_Var   longname_var = ppb_device_ref_get_longname(device_ref);

    if (longname_var.type == PP_VARTYPE_STRING)
        longname = ppb_var_var_to_utf8(longname_var, NULL);

    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, bad resource\n", __func__);
        goto err_1;
    }

    struct pp_audio_config_s *ac = pp_resource_acquire(config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, bad audio config\n", __func__);
        goto err_2;
    }

    ai->sample_rate = ac->sample_rate;
    ai->sample_frame_count = ac->sample_frame_count;

    ai->cb_0_3 = audio_input_callback_0_3;
    ai->cb_0_4 = audio_input_callback_0_4;
    ai->cb_user_data = user_data;

    ai->stream = ai->stream_ops->create_capture_stream(ai->sample_rate, ai->sample_frame_count,
                                                       capture_cb, ai, longname);
    if (!ai->stream) {
        trace_error("%s, can't create capture stream\n", __func__);
        goto err_3;
    }

    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, PP_OK, 0,
                                           __func__);
    retval = PP_OK_COMPLETIONPENDING;

err_3:
    pp_resource_release(config);
err_2:
    pp_resource_release(audio_input);
err_1:
    return retval;
}

int32_t
ppb_audio_input_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback_0_3 audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback)
{
    return do_ppb_audio_input_open(audio_input, device_ref, config, audio_input_callback, NULL,
                                   user_data, callback);
}

int32_t
ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                     PPB_AudioInput_Callback audio_input_callback, void *user_data,
                     struct PP_CompletionCallback callback)
{
    return do_ppb_audio_input_open(audio_input, device_ref, config, NULL, audio_input_callback,
                                   user_data, callback);
}

PP_Resource
ppb_audio_input_get_current_config(PP_Resource audio_input)
{
    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }

    PP_Resource audio_config = pp_resource_allocate(PP_RESOURCE_AUDIO_CONFIG, ai->instance);
    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, resource allocation failure\n", __func__);
        audio_config = 0;
        goto err;
    }

    ac->sample_rate =        ai->sample_rate;
    ac->sample_frame_count = ai->sample_frame_count;

    pp_resource_release(audio_config);

err:
    pp_resource_release(audio_input);
    return audio_config;
}

PP_Bool
ppb_audio_input_start_capture(PP_Resource audio_input)
{
    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (ai->stream)
        ai->stream_ops->pause(ai->stream, 0);
    pp_resource_release(audio_input);
    return PP_TRUE;
}

PP_Bool
ppb_audio_input_stop_capture(PP_Resource audio_input)
{
    struct pp_audio_input_s *ai = pp_resource_acquire(audio_input, PP_RESOURCE_AUDIO_INPUT);
    if (!ai) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (ai->stream)
        ai->stream_ops->pause(ai->stream, 1);
    pp_resource_release(audio_input);
    return PP_TRUE;
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
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_audio_input_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_is_audio_input(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_audio_input_is_audio_input(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s audio_input=%d, output={.GetDataBuffer=%p, .user_data=%p}, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, audio_input,
               output.GetDataBuffer, output.user_data, callback.func, callback.user_data,
               callback.flags);
    return ppb_audio_input_enumerate_devices(audio_input, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_monitor_device_change(PP_Resource audio_input,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data)
{
    trace_info("[PPB] {zilch} %s audio_input=%d, callback=%p, user_data=%p\n", __func__+6,
               audio_input, callback, user_data);
    return ppb_audio_input_monitor_device_change(audio_input, callback, user_data);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                               PPB_AudioInput_Callback_0_3 audio_input_callback, void *user_data,
                               struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s audio_input=%d, device_ref=%d, config=%d, audio_input_callback=%p, "
               "user_data=%p, callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6,
               audio_input, device_ref, config, audio_input_callback, user_data, callback.func,
               callback.user_data, callback.flags);
    return ppb_audio_input_open_0_3(audio_input, device_ref, config, audio_input_callback,
                                    user_data, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                           PPB_AudioInput_Callback audio_input_callback, void *user_data,
                           struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s audio_input=%d, device_ref=%d, config=%d, audio_input_callback=%p, "
               "user_data=%p, callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6,
               audio_input, device_ref, config, audio_input_callback, user_data, callback.func,
               callback.user_data, callback.flags);
    return ppb_audio_input_open(audio_input, device_ref, config, audio_input_callback, user_data,
                                callback);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_audio_input_get_current_config(PP_Resource audio_input)
{
    trace_info("[PPB] {full} %s audio_input=%d\n", __func__+6, audio_input);
    return ppb_audio_input_get_current_config(audio_input);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_start_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {full} %s audio_input=%d\n", __func__+6, audio_input);
    return ppb_audio_input_start_capture(audio_input);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_input_stop_capture(PP_Resource audio_input)
{
    trace_info("[PPB] {full} %s audio_input=%d\n", __func__+6, audio_input);
    return ppb_audio_input_stop_capture(audio_input);
}

TRACE_WRAPPER
void
trace_ppb_audio_input_close(PP_Resource audio_input)
{
    trace_info("[PPB] {full} %s audio_input=%d\n", __func__+6, audio_input);
    ppb_audio_input_close(audio_input);
}


const struct PPB_AudioInput_Dev_0_3 ppb_audio_input_dev_interface_0_3 = {
    .Create =               TWRAPF(ppb_audio_input_create),
    .IsAudioInput =         TWRAPF(ppb_audio_input_is_audio_input),
    .EnumerateDevices =     TWRAPF(ppb_audio_input_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_audio_input_monitor_device_change),
    .Open =                 TWRAPF(ppb_audio_input_open_0_3),
    .GetCurrentConfig =     TWRAPF(ppb_audio_input_get_current_config),
    .StartCapture =         TWRAPF(ppb_audio_input_start_capture),
    .StopCapture =          TWRAPF(ppb_audio_input_stop_capture),
    .Close =                TWRAPF(ppb_audio_input_close),
};

const struct PPB_AudioInput_Dev_0_4 ppb_audio_input_dev_interface_0_4 = {
    .Create =               TWRAPF(ppb_audio_input_create),
    .IsAudioInput =         TWRAPF(ppb_audio_input_is_audio_input),
    .EnumerateDevices =     TWRAPF(ppb_audio_input_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_audio_input_monitor_device_change),
    .Open =                 TWRAPF(ppb_audio_input_open),
    .GetCurrentConfig =     TWRAPF(ppb_audio_input_get_current_config),
    .StartCapture =         TWRAPF(ppb_audio_input_start_capture),
    .StopCapture =          TWRAPF(ppb_audio_input_stop_capture),
    .Close =                TWRAPF(ppb_audio_input_close),
};

static
void
__attribute__((constructor))
constructor_ppb_audio_input(void)
{
    register_interface(PPB_AUDIO_INPUT_DEV_INTERFACE_0_3, &ppb_audio_input_dev_interface_0_3);
    register_interface(PPB_AUDIO_INPUT_DEV_INTERFACE_0_4, &ppb_audio_input_dev_interface_0_4);
    register_resource(PP_RESOURCE_AUDIO_INPUT, ppb_audio_input_destroy);
}
