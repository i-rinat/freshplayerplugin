#include <ppapi/c/ppb_audio_config.h>
#include <stddef.h>
#include "trace.h"


PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return 0;
}

uint32_t
ppb_audio_config_recommend_sample_frame_count(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                              uint32_t requested_sample_frame_count)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return 0;
}

PP_Bool
ppb_audio_config_is_audio_config(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return PP_TRUE;
}

PP_AudioSampleRate
ppb_audio_config_get_sample_rate(PP_Resource config)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return 0;
}

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return 0;
}

PP_AudioSampleRate
ppb_audio_config_recommend_sample_rate(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    return 0;
}


const struct PPB_AudioConfig_1_1 ppb_audio_config_interface_1_1 = {
    .CreateStereo16Bit = ppb_audio_config_create_stereo_16_bit,
    .RecommendSampleFrameCount = ppb_audio_config_recommend_sample_frame_count,
    .IsAudioConfig = ppb_audio_config_is_audio_config,
    .GetSampleRate = ppb_audio_config_get_sample_rate,
    .GetSampleFrameCount = ppb_audio_config_get_sample_frame_count,
    .RecommendSampleRate = ppb_audio_config_recommend_sample_rate
};
