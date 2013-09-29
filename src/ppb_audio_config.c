#include <ppapi/c/ppb_audio_config.h>
#include <stddef.h>


const struct PPB_AudioConfig_1_1 ppb_audio_config_interface_1_1 = {
    .CreateStereo16Bit = NULL,
    .RecommendSampleFrameCount = NULL,
    .IsAudioConfig = NULL,
    .GetSampleRate = NULL,
    .GetSampleFrameCount = NULL,
    .RecommendSampleRate = NULL
};
