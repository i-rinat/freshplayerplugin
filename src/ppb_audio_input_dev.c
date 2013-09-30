#include <ppapi/c/dev/ppb_audio_input_dev.h>
#include <stddef.h>
#include "trace.h"


static
PP_Resource
ppb_audio_input_dev_create(PP_Instance instance)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_Bool
ppb_audio_input_dev_is_audio_input(PP_Resource resource)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
int32_t
ppb_audio_input_dev_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                      struct PP_CompletionCallback callback)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_audio_input_dev_monitor_device_change(PP_Resource audio_input,
                                          PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_audio_input_dev_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_Resource
ppb_audio_input_dev_get_current_config(PP_Resource audio_input)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_Bool
ppb_audio_input_dev_start_capture(PP_Resource audio_input)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_Bool
ppb_audio_input_dev_stop_capture(PP_Resource audio_input)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
void
ppb_audio_input_dev_close(PP_Resource audio_input)
{
    trace_info("{zilch} %s\n", __func__);
}

const struct PPB_AudioInput_Dev_0_3 ppb_audio_input_dev_interface_0_3 = {
    .Create = ppb_audio_input_dev_create,
    .IsAudioInput = ppb_audio_input_dev_is_audio_input,
    .EnumerateDevices = ppb_audio_input_dev_enumerate_devices,
    .MonitorDeviceChange = ppb_audio_input_dev_monitor_device_change,
    .Open = ppb_audio_input_dev_open,
    .GetCurrentConfig = ppb_audio_input_dev_get_current_config,
    .StartCapture = ppb_audio_input_dev_start_capture,
    .StopCapture = ppb_audio_input_dev_stop_capture,
    .Close = ppb_audio_input_dev_close
};
