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

#ifndef FPP__PPB_VIDEO_CAPTURE_DEV_H
#define FPP__PPB_VIDEO_CAPTURE_DEV_H

#include <ppapi/c/dev/ppb_video_capture_dev.h>


PP_Resource
ppb_video_capture_dev_create(PP_Instance instance);

void
ppb_video_capture_dev_destroy(void *p);

PP_Bool
ppb_video_capture_dev_is_video_capture(PP_Resource video_capture);

int32_t
ppb_video_capture_dev_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback);

int32_t
ppb_video_capture_dev_monitor_device_change(PP_Resource video_capture,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data);

int32_t
ppb_video_capture_dev_open(PP_Resource video_capture, PP_Resource device_ref,
                           const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                           uint32_t buffer_count, struct PP_CompletionCallback callback);

int32_t
ppb_video_capture_dev_start_capture(PP_Resource video_capture);

int32_t
ppb_video_capture_dev_reuse_buffer(PP_Resource video_capture, uint32_t buffer);

int32_t
ppb_video_capture_dev_stop_capture(PP_Resource video_capture);

void
ppb_video_capture_dev_close(PP_Resource video_capture);


#endif // FPP__PPB_VIDEO_CAPTURE_DEV_H
