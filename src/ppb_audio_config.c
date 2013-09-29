#include <ppapi/c/ppb_audio_config.h>
#include <stddef.h>


const PPB_AudioConfig ppb_audio_config = {
    .CreateStereo16Bit = NULL,
    .RecommendSampleFrameCount = NULL,
    .IsAudioConfig = NULL,
    .GetSampleRate = NULL,
    .GetSampleFrameCount = NULL,
    .RecommendSampleRate = NULL
};
