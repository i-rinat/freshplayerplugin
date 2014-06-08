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

#include "ppb_video_capture_dev.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "pp_resource.h"
#include "trace.h"



PP_Resource
ppb_video_capture_dev_create(PP_Instance instance)
{
    PP_Resource video_capture = pp_resource_allocate(PP_RESOURCE_VIDEO_CAPTURE, instance);
    return video_capture;
}

void
ppb_video_capture_dev_destroy(void *p)
{
}

PP_Bool
ppb_video_capture_dev_is_video_capture(PP_Resource video_capture)
{
    return pp_resource_get_type(video_capture) == PP_RESOURCE_VIDEO_CAPTURE;
}

int32_t
ppb_video_capture_dev_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback)
{
    output.GetDataBuffer(output.user_data, 0, sizeof(int));

    if (callback.func)
        callback.func(callback.user_data, PP_OK);
    return PP_OK;
}

int32_t
ppb_video_capture_dev_monitor_device_change(PP_Resource video_capture,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data)
{
    return 0;
}

int32_t
ppb_video_capture_dev_open(PP_Resource video_capture, PP_Resource device_ref,
                           const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                           uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_video_capture_dev_start_capture(PP_Resource video_capture)
{
    return 0;
}

int32_t
ppb_video_capture_dev_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    return 0;
}

int32_t
ppb_video_capture_dev_stop_capture(PP_Resource video_capture)
{
    return 0;
}

void
ppb_video_capture_dev_close(PP_Resource video_capture)
{
    return;
}


#ifndef NDEBUG
// trace wrappers
static
PP_Resource
trace_ppb_video_capture_dev_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_video_capture_dev_create(instance);
}

static
PP_Bool
trace_ppb_video_capture_dev_is_video_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_video_capture_dev_is_video_capture(video_capture);
}

static
int32_t
trace_ppb_video_capture_dev_enumerate_devices(PP_Resource video_capture,
                                              struct PP_ArrayOutput output,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {fake} %s\n", __func__+6);
    return ppb_video_capture_dev_enumerate_devices(video_capture, output, callback);
}

static
int32_t
trace_ppb_video_capture_dev_monitor_device_change(PP_Resource video_capture,
                                                  PP_MonitorDeviceChangeCallback callback,
                                                  void *user_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_dev_monitor_device_change(video_capture, callback, user_data);
}

static
int32_t
trace_ppb_video_capture_dev_open(PP_Resource video_capture, PP_Resource device_ref,
                                 const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                                 uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_dev_open(video_capture, device_ref, requested_info, buffer_count,
                                      callback);
}

static
int32_t
trace_ppb_video_capture_dev_start_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_dev_start_capture(video_capture);
}

static
int32_t
trace_ppb_video_capture_dev_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_dev_reuse_buffer(video_capture, buffer);
}

static
int32_t
trace_ppb_video_capture_dev_stop_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_video_capture_dev_stop_capture(video_capture);
}

static
void
trace_ppb_video_capture_dev_close(PP_Resource video_capture)
{
    trace_info("[PPB] {fake} %s\n", __func__+6);
    ppb_video_capture_dev_close(video_capture);
}
#endif // NDEBUG


const struct PPB_VideoCapture_Dev_0_3 ppb_video_capture_dev_interface_0_3 = {
    .Create =               TWRAP(ppb_video_capture_dev_create),
    .IsVideoCapture =       TWRAP(ppb_video_capture_dev_is_video_capture),
    .EnumerateDevices =     TWRAP(ppb_video_capture_dev_enumerate_devices),
    .MonitorDeviceChange =  TWRAP(ppb_video_capture_dev_monitor_device_change),
    .Open =                 TWRAP(ppb_video_capture_dev_open),
    .StartCapture =         TWRAP(ppb_video_capture_dev_start_capture),
    .ReuseBuffer =          TWRAP(ppb_video_capture_dev_reuse_buffer),
    .StopCapture =          TWRAP(ppb_video_capture_dev_stop_capture),
    .Close =                TWRAP(ppb_video_capture_dev_close),
};
