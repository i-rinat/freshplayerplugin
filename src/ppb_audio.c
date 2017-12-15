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

#include "audio_thread.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_audio.h"
#include "ppb_audio_config.h"
#include "ppb_core.h"
#include "ppb_instance.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include <glib.h>
#include <npapi/npapi.h>
#include <npapi/npfunctions.h>
#include <ppapi/c/pp_errors.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct pp_audio_s {
    COMMON_STRUCTURE_FIELDS
    uint32_t                sample_rate;
    uint32_t                sample_frame_count;
    PPB_Audio_Callback_1_0  callback_1_0;
    PPB_Audio_Callback      callback_1_1;
    void                   *user_data;
    audio_stream_ops       *stream_ops;
    audio_stream           *stream;
    int                     is_playing;
};

STATIC_ASSERT(sizeof(struct pp_audio_s) <= LARGEST_RESOURCE_SIZE);

static
void
playback_cb(void *buf, uint32_t sz, double latency, void *user_data)
{
    struct pp_audio_s *a = user_data;

    if (a->callback_1_0) {
        a->callback_1_0(buf, sz, a->user_data);
    } else if (a->callback_1_1) {
        a->callback_1_1(buf, sz, latency,a->user_data);
    }

    // discard acquired audio data if instance is muted
    if (g_atomic_int_get(&a->instance->is_muted))
        memset(buf, 0, sz);
}

static
PP_Resource
do_ppb_audio_create(PP_Instance instance, PP_Resource audio_config,
                    PPB_Audio_Callback_1_0 audio_callback_1_0,
                    PPB_Audio_Callback     audio_callback_1_1, void *user_data)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    if (!audio_callback_1_0 && !audio_callback_1_1)
        return PP_ERROR_BADARGUMENT;

    PP_Resource audio = pp_resource_allocate(PP_RESOURCE_AUDIO, pp_i);
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, bad audio config\n", __func__);
        goto err;
    }

    a->sample_rate = ac->sample_rate;
    a->sample_frame_count = ac->sample_frame_count;
    pp_resource_release(audio_config);

    a->callback_1_0 = audio_callback_1_0;
    a->callback_1_1 = audio_callback_1_1;
    a->user_data = user_data;
    a->stream_ops = audio_select_implementation();
    if (a->stream_ops == NULL) {
        trace_error("%s, no viable audio implementation\n", __func__);
        goto err;
    }

    a->stream = a->stream_ops->create_playback_stream(a->sample_rate, a->sample_frame_count,
                                                      playback_cb, a);
    if (!a->stream) {
        trace_error("%s, can't create playback stream\n", __func__);
        goto err;
    }

    pp_resource_release(audio);
    return audio;
err:
    pp_resource_release(audio);
    pp_resource_expunge(audio);
    return 0;
}

PP_Resource
ppb_audio_create_1_0(PP_Instance instance, PP_Resource audio_config,
                     PPB_Audio_Callback_1_0 audio_callback_1_0, void *user_data)
{
    return do_ppb_audio_create(instance, audio_config, audio_callback_1_0, NULL, user_data);
}

PP_Resource
ppb_audio_create_1_1(PP_Instance instance, PP_Resource audio_config,
                     PPB_Audio_Callback audio_callback_1_1, void *user_data)
{
    return do_ppb_audio_create(instance, audio_config, NULL, audio_callback_1_1, user_data);
}

static
void
ppb_audio_destroy(void *p)
{
    struct pp_audio_s *a = p;

    if (a->is_playing) {
        g_atomic_int_add(&a->instance->audio_source_count, -1);
        a->is_playing = 0;
    }

    a->stream_ops->destroy(a->stream);
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
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }

    PP_Resource audio_config = pp_resource_allocate(PP_RESOURCE_AUDIO_CONFIG, a->instance);
    struct pp_audio_config_s *ac = pp_resource_acquire(audio_config, PP_RESOURCE_AUDIO_CONFIG);
    if (!ac) {
        trace_error("%s, resource allocation failure\n", __func__);
        audio_config = 0;
        goto err;
    }

    ac->sample_rate = a->sample_rate;
    ac->sample_frame_count = a->sample_frame_count;

    pp_resource_release(audio_config);

err:
    pp_resource_release(audio);
    return audio_config;
}

static
void
update_instance_playing_audio_status_ptac(void *p)
{
    PP_Instance instance = GPOINTER_TO_SIZE(p);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);

    if (!pp_i)
        return;

    int is_playing_audio = g_atomic_int_get(&pp_i->audio_source_count) > 0;
    NPError err = npn.setvalue(pp_i->npp, NPPVpluginIsPlayingAudio,
                               GINT_TO_POINTER(is_playing_audio));
    if (err != NPERR_NO_ERROR)
        trace_info_f("%s, failed to set NPPVpluginIsPlayingAudio\n", __func__);
}

PP_Bool
ppb_audio_start_playback(PP_Resource audio)
{
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    a->stream_ops->pause(a->stream, 0);
    if (!a->is_playing) {
        g_atomic_int_add(&a->instance->audio_source_count, 1);
        a->is_playing = 1;
    }

    pp_resource_release(audio);

    ppb_core_call_on_browser_thread(a->instance->id, update_instance_playing_audio_status_ptac,
                                    GSIZE_TO_POINTER(a->instance->id));

    return PP_TRUE;
}

PP_Bool
ppb_audio_stop_playback(PP_Resource audio)
{
    struct pp_audio_s *a = pp_resource_acquire(audio, PP_RESOURCE_AUDIO);
    if (!a) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    a->stream_ops->pause(a->stream, 1);
    if (a->is_playing) {
        g_atomic_int_add(&a->instance->audio_source_count, -1);
        a->is_playing = 0;
    }

    pp_resource_release(audio);

    ppb_core_call_on_browser_thread(a->instance->id, update_instance_playing_audio_status_ptac,
                                    GSIZE_TO_POINTER(a->instance->id));

    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_audio_create_1_0(PP_Instance instance, PP_Resource audio_config,
                           PPB_Audio_Callback_1_0 audio_callback, void *user_data)
{
    trace_info("[PPB] {full} %s instance=%d, audio_config=%d, audio_callback=%p, user_data=%p\n",
               __func__+6, instance, audio_config, audio_callback, user_data);
    return ppb_audio_create_1_0(instance, audio_config, audio_callback, user_data);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_audio_create_1_1(PP_Instance instance, PP_Resource audio_config,
                           PPB_Audio_Callback audio_callback, void *user_data)
{
    trace_info("[PPB] {full} %s instance=%d, audio_config=%d, audio_callback=%p, user_data=%p\n",
               __func__+6, instance, audio_config, audio_callback, user_data);
    return ppb_audio_create_1_1(instance, audio_config, audio_callback, user_data);
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
    .Create =           TWRAPF(ppb_audio_create_1_0),
    .IsAudio =          TWRAPF(ppb_audio_is_audio),
    .GetCurrentConfig = TWRAPF(ppb_audio_get_current_config),
    .StartPlayback =    TWRAPF(ppb_audio_start_playback),
    .StopPlayback =     TWRAPF(ppb_audio_stop_playback),
};

const struct PPB_Audio_1_1 ppb_audio_interface_1_1 = {
    .Create =           TWRAPF(ppb_audio_create_1_1),
    .IsAudio =          TWRAPF(ppb_audio_is_audio),
    .GetCurrentConfig = TWRAPF(ppb_audio_get_current_config),
    .StartPlayback =    TWRAPF(ppb_audio_start_playback),
    .StopPlayback =     TWRAPF(ppb_audio_stop_playback),
};

static
void
__attribute__((constructor))
constructor_ppb_audio(void)
{
    register_interface(PPB_AUDIO_INTERFACE_1_0, &ppb_audio_interface_1_0);
    register_interface(PPB_AUDIO_INTERFACE_1_1, &ppb_audio_interface_1_1);
    register_resource(PP_RESOURCE_AUDIO, ppb_audio_destroy);
}
