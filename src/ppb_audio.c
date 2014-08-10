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

#include <pthread.h>
#include "ppb_audio.h"
#include <stdlib.h>
#include <glib.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "pp_resource.h"
#include "ppb_message_loop.h"


PP_Resource
ppb_audio_create(PP_Instance instance, PP_Resource audio_config,
                 PPB_Audio_Callback_1_0 audio_callback, void *user_data)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, wrong instance\n", __func__);
        return 0;
    }
    PP_Resource audio = pp_resource_allocate(PP_RESOURCE_AUDIO, pp_i);
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);

    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, wrong audio config resource\n", __func__);
        goto err;
    }

    a->sample_rate = ac->sample_rate;
    a->sample_frame_count = ac->sample_frame_count;
    pp_resource_release(audio_config);

    snd_pcm_hw_params_t *hw_params;
    snd_pcm_sw_params_t *sw_params;

#define ERR_CHECK(errcode, funcname, jumpoutstatement)                          \
    if (errcode < 0) {                                                          \
        trace_error("%s, " #funcname ", %s\n", __func__, snd_strerror(errcode));\
        jumpoutstatement;                                                       \
    }

    int res;
    res = snd_pcm_open(&a->ph, "default", SND_PCM_STREAM_PLAYBACK, 0);
    ERR_CHECK(res, snd_pcm_open, goto err);

    res = snd_pcm_hw_params_malloc(&hw_params);
    ERR_CHECK(res, snd_pcm_hw_params_malloc, goto err);

    res = snd_pcm_hw_params_any(a->ph, hw_params);
    ERR_CHECK(res, snd_pcm_hw_params_any, goto err);

    res = snd_pcm_hw_params_set_access(a->ph, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    ERR_CHECK(res, snd_pcm_hw_params_set_access, goto err);

    res = snd_pcm_hw_params_set_format(a->ph, hw_params, SND_PCM_FORMAT_S16_LE);
    ERR_CHECK(res, snd_pcm_hw_params_set_format, goto err);

    res = snd_pcm_hw_params_set_rate_near(a->ph, hw_params, &a->sample_rate, 0);
    ERR_CHECK(res, snd_pcm_hw_params_set_rate_near, goto err);

    res = snd_pcm_hw_params_set_channels(a->ph, hw_params, 2);
    ERR_CHECK(res, snd_pcm_hw_params_set_channels, goto err);

    int dir = 1;
    unsigned int buffer_time = 2 * (long long)a->sample_frame_count * 1000 * 1000 / a->sample_rate;

    buffer_time = CLAMP(buffer_time,
                        1000 * config.audio_buffer_min_ms,
                        1000 * config.audio_buffer_max_ms);
    res = snd_pcm_hw_params_set_buffer_time_near(a->ph, hw_params, &buffer_time, &dir);
    ERR_CHECK(res, snd_pcm_hw_params_set_buffer_time_near, goto err);

    snd_pcm_uframes_t period_size = a->sample_frame_count / 4;
    dir = 1;
    res = snd_pcm_hw_params_set_period_size_near(a->ph, hw_params, &period_size, &dir);
    ERR_CHECK(res, snd_pcm_hw_params_set_period_size_near, goto err);
    a->period_size = period_size;

    res = snd_pcm_hw_params(a->ph, hw_params);
    ERR_CHECK(res, snd_pcm_hw_params, goto err);

    snd_pcm_hw_params_free(hw_params);

    res = snd_pcm_sw_params_malloc(&sw_params);
    ERR_CHECK(res, snd_pcm_sw_params_malloc, goto err);

    res = snd_pcm_sw_params_current(a->ph, sw_params);
    ERR_CHECK(res, snd_pcm_sw_params_current, goto err);

    res = snd_pcm_sw_params_set_avail_min(a->ph, sw_params, a->sample_frame_count);
    ERR_CHECK(res, snd_pcm_sw_params_set_avail_min, goto err);

    res = snd_pcm_sw_params_set_start_threshold(a->ph, sw_params, 0U);
    ERR_CHECK(res, snd_pcm_sw_params_set_start_threshold, goto err);

    res = snd_pcm_sw_params(a->ph, sw_params);
    ERR_CHECK(res, snd_pcm_sw_params, goto err);

    res = snd_pcm_prepare(a->ph);
    ERR_CHECK(res, snd_pcm_prepare, goto err);

    snd_pcm_sw_params_free(sw_params);

#undef ERR_CHECK

    a->callback = audio_callback;
    a->user_data = user_data;
    a->audio_buffer = malloc(a->sample_frame_count * 2 * sizeof(int16_t));
    if (!a->audio_buffer) {
        trace_error("%s, failed to allocate audio buffer\n", __func__);
        goto err;
    }

    pp_resource_release(audio);
    return audio;
err:
    pp_resource_release(audio);
    pp_resource_expunge(audio);
    return 0;
}

void
ppb_audio_destroy(void *p)
{
    struct pp_audio_s *a = p;

    if (a->playing) {
        a->shutdown = 1;
        while (a->playing)
            usleep(10);
    }
    snd_pcm_close(a->ph);
    free_and_nullify(a, audio_buffer);
}

PP_Bool
ppb_audio_is_audio(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_AUDIO;
}

PP_Resource
ppb_audio_get_current_config(PP_Resource audio)
{
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, wrong audio resource\n", __func__);
        return 0;
    }
    PP_Resource audio_config = pp_resource_allocate(PP_RESOURCE_AUDIO_CONFIG, a->instance);
    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);

    ac->sample_rate = a->sample_rate;
    ac->sample_frame_count = a->sample_frame_count;
    pp_resource_release(audio);
    pp_resource_release(audio_config);

    return audio_config;
}

static
void *
audio_player_thread(void *p)
{
    struct pp_audio_s *a = p;
    int error_cnt = 0;

    ppb_message_loop_mark_thread_unsuitable();
    a->playing = 1;
    while (1) {
        int ret;
        if (a->shutdown)
            break;

        snd_pcm_wait(a->ph, 1000);
        int frame_count = snd_pcm_avail_update(a->ph);
        if (frame_count < 0) {
            trace_warning("%s, snd_pcm_avail_update error %d\n", __func__, (int)frame_count);
            do {
                ret = snd_pcm_recover(a->ph, frame_count, 1);
            } while (ret == -EINTR);
            error_cnt++;
            if (error_cnt >= 5)
                trace_error("%s, too many buffer underruns (1)\n", __func__);
            continue;
        }

        frame_count = MIN(frame_count, (int)a->sample_frame_count);
        a->callback(a->audio_buffer, frame_count * 2 * sizeof(int16_t), a->user_data);

        snd_pcm_sframes_t written = snd_pcm_writei(a->ph, a->audio_buffer, frame_count);
        if (written < 0) {
            trace_warning("%s, snd_pcm_writei error %d\n", __func__, (int)written);
            do {
                ret = snd_pcm_recover(a->ph, written, 1);
            } while (ret == -EINTR);
            error_cnt++;
            if (error_cnt >= 5)
                trace_error("%s, too many buffer underruns (2)\n", __func__);
            continue;
        }

        error_cnt = 0;
    }

    a->playing = 0;
    return NULL;
}

PP_Bool
ppb_audio_start_playback(PP_Resource audio)
{
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, wrong audio resource\n", __func__);
        return PP_FALSE;
    }
    if (a->playing) {
        pp_resource_release(audio);
        return PP_TRUE;
    }

    pthread_create(&a->thread, NULL, audio_player_thread, a);
    pp_resource_release(audio);
    return PP_TRUE;
}

PP_Bool
ppb_audio_stop_playback(PP_Resource audio)
{
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, wrong audio resource\n", __func__);
        return PP_FALSE;
    }

    if (a->playing) {
        a->shutdown = 1;
        while (a->playing)
            usleep(10);
    }

    pp_resource_release(audio);
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_audio_create(PP_Instance instance, PP_Resource audio_config,
                       PPB_Audio_Callback_1_0 audio_callback, void *user_data)
{
    trace_info("[PPB] {full} %s instance=%d, audio_config=%d, audio_callback=%p, user_data=%p\n",
               __func__+6, instance, audio_config, audio_callback, user_data);
    return ppb_audio_create(instance, audio_config, audio_callback, user_data);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_is_audio(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_audio_is_audio(resource);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_audio_get_current_config(PP_Resource audio)
{
    trace_info("[PPB] {full} %s audio=%d\n", __func__+6, audio);
    return ppb_audio_get_current_config(audio);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_start_playback(PP_Resource audio)
{
    trace_info("[PPB] {full} %s audio=%d\n", __func__+6, audio);
    return ppb_audio_start_playback(audio);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_audio_stop_playback(PP_Resource audio)
{
    trace_info("[PPB] {full} %s audio=%d\n", __func__+6, audio);
    return ppb_audio_stop_playback(audio);
}


const struct PPB_Audio_1_0 ppb_audio_interface_1_0 = {
    .Create =           TWRAPF(ppb_audio_create),
    .IsAudio =          TWRAPF(ppb_audio_is_audio),
    .GetCurrentConfig = TWRAPF(ppb_audio_get_current_config),
    .StartPlayback =    TWRAPF(ppb_audio_start_playback),
    .StopPlayback =     TWRAPF(ppb_audio_stop_playback),
};
