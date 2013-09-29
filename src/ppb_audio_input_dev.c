#include <ppapi/c/dev/ppb_audio_input_dev.h>
#include <stddef.h>


const PPB_AudioInput_Dev ppb_audio_input_dev = {
    .Create = NULL,
    .IsAudioInput = NULL,
    .EnumerateDevices = NULL,
    .MonitorDeviceChange = NULL,
    .Open = NULL,
    .GetCurrentConfig = NULL,
    .StartCapture = NULL,
    .StopCapture = NULL,
    .Close = NULL
};
