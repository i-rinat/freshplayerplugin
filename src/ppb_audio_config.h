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

#pragma once

#include "pp_resource.h"
#include <ppapi/c/ppb_audio_config.h>
#include <stdint.h>

struct pp_audio_config_s {
    COMMON_STRUCTURE_FIELDS
    PP_AudioSampleRate      sample_rate;
    uint32_t                sample_frame_count;
};

PP_Resource
ppb_audio_config_create_stereo_16_bit(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                      uint32_t sample_frame_count);

uint32_t
ppb_audio_config_recommend_sample_frame_count(PP_Instance instance, PP_AudioSampleRate sample_rate,
                                              uint32_t requested_sample_frame_count);

PP_Bool
ppb_audio_config_is_audio_config(PP_Resource resource);

PP_AudioSampleRate
ppb_audio_config_get_sample_rate(PP_Resource config);

uint32_t
ppb_audio_config_get_sample_frame_count(PP_Resource config);

PP_AudioSampleRate
ppb_audio_config_recommend_sample_rate(PP_Instance instance);
