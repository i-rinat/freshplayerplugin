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

#include "ppb_video_capture.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "pp_resource.h"
#include "ppb_core.h"
#include "trace.h"
#include "tables.h"
#include "ppb_device_ref.h"
#include "ppb_var.h"



PP_Resource
ppb_video_capture_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource video_capture = pp_resource_allocate(PP_RESOURCE_VIDEO_CAPTURE, pp_i);
    return video_capture;
}

void
ppb_video_capture_destroy(void *p)
{
}

PP_Bool
ppb_video_capture_is_video_capture(PP_Resource video_capture)
{
    return pp_resource_get_type(video_capture) == PP_RESOURCE_VIDEO_CAPTURE;
}

int32_t
ppb_video_capture_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                    struct PP_CompletionCallback callback)
{
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    PP_Resource *devs = output.GetDataBuffer(output.user_data, 1, sizeof(PP_Resource));
    if (!devs) {
        pp_resource_release(video_capture);
        return PP_ERROR_FAILED;
    }

    // TODO: enumerate all capture devices
    // TODO: use real name, returned by driver; put device path into longname field
    devs[0] = ppb_device_ref_create(vc->instance->id,
                                    ppb_var_var_from_utf8_z("Default capture device"),
                                    ppb_var_var_from_utf8_z("Longname of default capture device"),
                                    PP_DEVICETYPE_DEV_VIDEOCAPTURE);

    pp_resource_release(video_capture);

    ppb_core_call_on_main_thread2(0, callback, PP_OK, __func__);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_video_capture_monitor_device_change(PP_Resource video_capture,
                                        PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    return 0;
}

int32_t
ppb_video_capture_open(PP_Resource video_capture, PP_Resource device_ref,
                       const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                       uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_video_capture_start_capture(PP_Resource video_capture)
{
    return 0;
}

int32_t
ppb_video_capture_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    return 0;
}

int32_t
ppb_video_capture_stop_capture(PP_Resource video_capture)
{
    return 0;
}

void
ppb_video_capture_close(PP_Resource video_capture)
{
    return;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_video_capture_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_video_capture_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_video_capture_is_video_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_video_capture_is_video_capture(video_capture);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_enumerate_devices(PP_Resource video_capture,
                                          struct PP_ArrayOutput output,
                                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_video_capture_enumerate_devices(video_capture, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_monitor_device_change(PP_Resource video_capture,
                                              PP_MonitorDeviceChangeCallback callback,
                                              void *user_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_monitor_device_change(video_capture, callback, user_data);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_open(PP_Resource video_capture, PP_Resource device_ref,
                             const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                             uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_open(video_capture, device_ref, requested_info, buffer_count,
                                  callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_start_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_start_capture(video_capture);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_reuse_buffer(video_capture, buffer);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_stop_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_stop_capture(video_capture);
}

TRACE_WRAPPER
void
trace_ppb_video_capture_close(PP_Resource video_capture)
{
    trace_info("[PPB] {fake} %s\n", __func__+6);
    ppb_video_capture_close(video_capture);
}


const struct PPB_VideoCapture_Dev_0_3 ppb_video_capture_dev_interface_0_3 = {
    .Create =               TWRAPF(ppb_video_capture_create),
    .IsVideoCapture =       TWRAPF(ppb_video_capture_is_video_capture),
    .EnumerateDevices =     TWRAPF(ppb_video_capture_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_video_capture_monitor_device_change),
    .Open =                 TWRAPZ(ppb_video_capture_open),
    .StartCapture =         TWRAPZ(ppb_video_capture_start_capture),
    .ReuseBuffer =          TWRAPZ(ppb_video_capture_reuse_buffer),
    .StopCapture =          TWRAPZ(ppb_video_capture_stop_capture),
    .Close =                TWRAPZ(ppb_video_capture_close),
};
