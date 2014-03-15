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
#include "pp_resource.h"


static
uint32_t
clamp_value(uint32_t value, uint32_t v_min, uint32_t v_max)
{
    if (value < v_min) {
        value = v_min;
    }
    if (value > v_max) {
        value = v_max;
    }
    return value;
}

PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count)
{
    PP_Resource audio_config = pp_resource_allocate(PP_RESOURCE_AUDIO_CONFIG, instance);
    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);

    ac->sample_rate = sample_rate;
    ac->sample_frame_count =
        clamp_value(sample_frame_count, PP_AUDIOMINSAMPLEFRAMECOUNT, PP_AUDIOMAXSAMPLEFRAMECOUNT);

    pp_resource_release(audio_config);
    return audio_config;
}

void
ppb_audio_config_destroy(void *ptr)
{
    (void)ptr;
}

uint32_t
ppb_audio_config_recommend_sample_frame_count(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                              uint32_t requested_sample_frame_count)
{
    (void)instance;
    (void)sample_rate;

    return clamp_value(requested_sample_frame_count, PP_AUDIOMINSAMPLEFRAMECOUNT,
                       PP_AUDIOMAXSAMPLEFRAMECOUNT);
}

PP_Bool
ppb_audio_config_is_audio_config(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_AUDIO_CONFIG;
}

PP_AudioSampleRate
ppb_audio_config_get_sample_rate(PP_Resource config)
{
    struct pp_audio_config_s *ac = pp_resource_acquire(config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac)
        return PP_AUDIOSAMPLERATE_NONE;
    PP_AudioSampleRate sample_rate = ac->sample_rate;
    pp_resource_release(config);
    return sample_rate;
}

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    struct pp_audio_config_s *ac = pp_resource_acquire(config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac)
        return 0;
    uint32_t sample_frame_count = ac->sample_frame_count;
    pp_resource_release(config);
    return sample_frame_count;
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
    trace_info("[PPB] {full} %s\n", __func__+6);
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
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_is_audio_config(resource);
}

static
PP_AudioSampleRate
trace_ppb_audio_config_get_sample_rate(PP_Resource config)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_get_sample_rate(config);
}

static
uint32_t
trace_ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
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
