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

#include "ppb_audio_config.h"
#include <stddef.h>
#include "trace.h"


PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count)
{
    return 123009;
}

uint32_t
ppb_audio_config_recommend_sample_frame_count(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                              uint32_t requested_sample_frame_count)
{

    (void)instance;
    (void)sample_rate;
    uint32_t sfc = requested_sample_frame_count;
    sfc = sfc < PP_AUDIOMINSAMPLEFRAMECOUNT ? PP_AUDIOMINSAMPLEFRAMECOUNT : sfc;
    sfc = sfc > PP_AUDIOMAXSAMPLEFRAMECOUNT ? PP_AUDIOMAXSAMPLEFRAMECOUNT : sfc;
    return sfc;
}

PP_Bool
ppb_audio_config_is_audio_config(PP_Resource resource)
{
    return PP_TRUE;
}

PP_AudioSampleRate
ppb_audio_config_get_sample_rate(PP_Resource config)
{
    return 123008;
}

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    return 0;
}

PP_AudioSampleRate
ppb_audio_config_recommend_sample_rate(PP_Instance instance)
{
    (void)instance;
    return PP_AUDIOSAMPLERATE_48000;
}

// trace wrappers
static
PP_Resource
trace_ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                            uint32_t sample_frame_count)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_config_create_stereo_16_bit(instance, sample_rate, sample_frame_count);
}

static
uint32_t
trace_ppb_audio_config_recommend_sample_frame_count(PP_Instance instance,
                                                    PP_AudioSampleRate sample_rate,
                                                    uint32_t requested_sample_frame_count)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_recommend_sample_frame_count(instance, sample_rate,
                                                         requested_sample_frame_count);
}

static
PP_Bool
trace_ppb_audio_config_is_audio_config(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_config_is_audio_config(resource);
}

static
PP_AudioSampleRate
trace_ppb_audio_config_get_sample_rate(PP_Resource config)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_config_get_sample_rate(config);
}

static
uint32_t
trace_ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_audio_config_get_sample_frame_count(config);
}

static
PP_AudioSampleRate
trace_ppb_audio_config_recommend_sample_rate(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_recommend_sample_rate(instance);
}


const struct PPB_AudioConfig_1_1 ppb_audio_config_interface_1_1 = {
    .CreateStereo16Bit =            trace_ppb_audio_config_create_stereo_16_bit,
    .RecommendSampleFrameCount =    trace_ppb_audio_config_recommend_sample_frame_count,
    .IsAudioConfig =                trace_ppb_audio_config_is_audio_config,
    .GetSampleRate =                trace_ppb_audio_config_get_sample_rate,
    .GetSampleFrameCount =          trace_ppb_audio_config_get_sample_frame_count,
    .RecommendSampleRate =          trace_ppb_audio_config_recommend_sample_rate
};
