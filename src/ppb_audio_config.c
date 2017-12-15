/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_audio_config.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include <glib.h>

STATIC_ASSERT(sizeof(struct pp_audio_config_s) <= LARGEST_RESOURCE_SIZE);

PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource audio_config = pp_resource_allocate(PP_RESOURCE_AUDIO_CONFIG, pp_i);
    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    ac->sample_rate = sample_rate;
    ac->sample_frame_count = CLAMP(sample_frame_count,
                                   PP_AUDIOMINSAMPLEFRAMECOUNT, PP_AUDIOMAXSAMPLEFRAMECOUNT);

    pp_resource_release(audio_config);
    return audio_config;
}

static
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

    return CLAMP(requested_sample_frame_count,
                 PP_AUDIOMINSAMPLEFRAMECOUNT, PP_AUDIOMAXSAMPLEFRAMECOUNT);
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
    if (!ac) {
        trace_error("%s, bad resource\n", __func__);
        return PP_AUDIOSAMPLERATE_NONE;
    }
    PP_AudioSampleRate sample_rate = ac->sample_rate;
    pp_resource_release(config);
    return sample_rate;
}

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    struct pp_audio_config_s *ac = pp_resource_acquire(config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
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
TRACE_WRAPPER
PP_Resource
trace_ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                            uint32_t sample_frame_count)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_create_stereo_16_bit(instance, sample_rate, sample_frame_count);
}

TRACE_WRAPPER
uint32_t
trace_ppb_audio_config_recommend_sample_frame_count(PP_Instance instance,
                                                    PP_AudioSampleRate sample_rate,
                                                    uint32_t requested_sample_frame_count)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_recommend_sample_frame_count(instance, sample_rate,
                                                         requested_sample_frame_count);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_config_is_audio_config(PP_Resource resource)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_is_audio_config(resource);
}

TRACE_WRAPPER
PP_AudioSampleRate
trace_ppb_audio_config_get_sample_rate(PP_Resource config)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_get_sample_rate(config);
}

TRACE_WRAPPER
uint32_t
trace_ppb_audio_config_get_sample_frame_count(PP_Resource config)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_get_sample_frame_count(config);
}

TRACE_WRAPPER
PP_AudioSampleRate
trace_ppb_audio_config_recommend_sample_rate(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_audio_config_recommend_sample_rate(instance);
}


const struct PPB_AudioConfig_1_1 ppb_audio_config_interface_1_1 = {
    .CreateStereo16Bit =            TWRAPF(ppb_audio_config_create_stereo_16_bit),
    .RecommendSampleFrameCount =    TWRAPF(ppb_audio_config_recommend_sample_frame_count),
    .IsAudioConfig =                TWRAPF(ppb_audio_config_is_audio_config),
    .GetSampleRate =                TWRAPF(ppb_audio_config_get_sample_rate),
    .GetSampleFrameCount =          TWRAPF(ppb_audio_config_get_sample_frame_count),
    .RecommendSampleRate =          TWRAPF(ppb_audio_config_recommend_sample_rate),
};

static
void
__attribute__((constructor))
constructor_ppb_audio_config(void)
{
    register_interface(PPB_AUDIO_CONFIG_INTERFACE_1_1, &ppb_audio_config_interface_1_1);
    register_resource(PP_RESOURCE_AUDIO_CONFIG, ppb_audio_config_destroy);
}
