#include <ppapi/c/dev/ppb_video_capture_dev.h>
#include <stddef.h>
#include "trace.h"


static
PP_Resource
ppb_video_capture_dev_create(PP_Instance instance)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_Bool
ppb_video_capture_dev_is_video_capture(PP_Resource video_capture)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
int32_t
ppb_video_capture_dev_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                        struct PP_CompletionCallback callback)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_video_capture_dev_monitor_device_change(PP_Resource video_capture,
                                            PP_MonitorDeviceChangeCallback callback,
                                            void *user_data)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_video_capture_dev_open(PP_Resource video_capture, PP_Resource device_ref,
                           const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                           uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_video_capture_dev_start_capture(PP_Resource video_capture)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_video_capture_dev_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_video_capture_dev_stop_capture(PP_Resource video_capture)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
void
ppb_video_capture_dev_close(PP_Resource video_capture)
{
    trace_info("{zilch} %s\n", __func__);
}

const struct PPB_VideoCapture_Dev_0_3 ppb_video_capture_dev_interface_0_3 = {
    .Create = ppb_video_capture_dev_create,
    .IsVideoCapture = ppb_video_capture_dev_is_video_capture,
    .EnumerateDevices = ppb_video_capture_dev_enumerate_devices,
    .MonitorDeviceChange = ppb_video_capture_dev_monitor_device_change,
    .Open = ppb_video_capture_dev_open,
    .StartCapture = ppb_video_capture_dev_start_capture,
    .ReuseBuffer = ppb_video_capture_dev_reuse_buffer,
    .StopCapture = ppb_video_capture_dev_stop_capture,
    .Close = ppb_video_capture_dev_close
};
