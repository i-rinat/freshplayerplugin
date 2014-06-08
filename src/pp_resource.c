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

#include "pp_resource.h"
#include "tables.h"
#include "trace.h"
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include "ppb_graphics2d.h"
#include "ppb_graphics3d.h"
#include "ppb_image_data.h"
#include "ppb_url_loader.h"
#include "ppb_url_request_info.h"
#include "ppb_browser_font_trusted.h"
#include "ppb_audio_config.h"
#include "ppb_audio.h"
#include "ppb_input_event.h"
#include "ppb_flash_font_file.h"
#include "ppb_flash_menu.h"
#include "ppb_video_capture_dev.h"
#include "ppb_audio_input_dev.h"
#include "ppb_flash_message_loop.h"
#include "ppb_tcp_socket_private.h"


#define FREE_IF_NOT_NULL(ptr)   if (ptr) { free(ptr); ptr = NULL; }

static GHashTable      *res_tbl;
static int              res_tbl_next = 0;
static pthread_mutex_t  res_tbl_lock = PTHREAD_MUTEX_INITIALIZER;

static
__attribute__((constructor))
void
pp_resource_constructor(void)
{
    res_tbl = g_hash_table_new(g_direct_hash, g_direct_equal);
    res_tbl_next = 1;
}

PP_Resource
pp_resource_allocate(enum pp_resource_type_e type, PP_Instance instance)
{
    struct pp_resource_generic_s *res = g_slice_alloc0(sizeof(union pp_largest_u));
    res->type = type;
    res->ref_cnt = 1;
    pthread_mutex_init(&res->lock, NULL);
    res->instance = instance;

    pthread_mutex_lock(&res_tbl_lock);
    res->self_id = res_tbl_next ++;
    g_hash_table_insert(res_tbl, GINT_TO_POINTER(res->self_id), res);
    pthread_mutex_unlock(&res_tbl_lock);

    return res->self_id;
}

void
pp_resource_expunge(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    void *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        g_slice_free(union pp_largest_u, ptr);
        g_hash_table_remove(res_tbl, GINT_TO_POINTER(resource));
    }
    pthread_mutex_unlock(&res_tbl_lock);
}

void *
pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type)
{
    struct pp_resource_generic_s *gr = NULL;
    while (1) {
        pthread_mutex_lock(&res_tbl_lock);
        gr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
        if (!gr || gr->type != type) {
            gr = NULL;
            break;
        }
        if (pthread_mutex_trylock(&gr->lock) == 0)
            break;
        pthread_mutex_unlock(&res_tbl_lock);
        usleep(1);
    }

    pthread_mutex_unlock(&res_tbl_lock);
    return gr;
}

void
pp_resource_release(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *gr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (gr) {
        pthread_mutex_unlock(&gr->lock);
    }
    pthread_mutex_unlock(&res_tbl_lock);
}

enum pp_resource_type_e
pp_resource_get_type(PP_Resource resource)
{
    enum pp_resource_type_e type = PP_RESOURCE_UNKNOWN;
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        type = ptr->type;
    }
    pthread_mutex_unlock(&res_tbl_lock);
    return type;
}

void
pp_resource_ref(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        ptr->ref_cnt ++;
    } else {
        trace_warning("%s, no such resource %d\n", __func__, resource);
    }
    pthread_mutex_unlock(&res_tbl_lock);
}

static
void
_count_resources(gpointer key, gpointer value, gpointer user_data)
{
    int *counts = user_data;
    struct pp_resource_generic_s *r = value;

    if (0 <= r->type && r->type < PP_RESOURCE_TYPES_COUNT)
        counts[r->type] ++;
    else
        counts[PP_RESOURCE_TYPES_COUNT] ++;
}

void
pp_resource_unref(PP_Resource resource)
{
    PP_Resource parent = 0;
    int ref_cnt = 0;

    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr)
        ref_cnt = --ptr->ref_cnt;
    pthread_mutex_unlock(&res_tbl_lock);

    if (!ptr)
        return;

    if (ref_cnt <= 0) {
        switch (ptr->type) {
        case PP_RESOURCE_URL_LOADER:
            ppb_url_loader_destroy(ptr);
            break;
        case PP_RESOURCE_URL_RESPONSE_INFO:
            parent = ((struct pp_url_response_info_s *)ptr)->url_loader;
            break;
        case PP_RESOURCE_URL_REQUEST_INFO:
            ppb_url_request_info_destroy(ptr);
            break;
        case PP_RESOURCE_IMAGE_DATA:
            ppb_image_data_destroy(ptr);
            break;
        case PP_RESOURCE_GRAPHICS2D:
            ppb_graphics2d_destroy(ptr);
            break;
        case PP_RESOURCE_GRAPHICS3D:
            ppb_graphics3d_destroy(ptr);
            break;
        case PP_RESOURCE_BROWSER_FONT:
            ppb_browser_font_trusted_destroy(ptr);
            break;
        case PP_RESOURCE_AUDIO_CONFIG:
            ppb_audio_config_destroy(ptr);
            break;
        case PP_RESOURCE_AUDIO:
            ppb_audio_destroy(ptr);
            break;
        case PP_RESOURCE_INPUT_EVENT:
            ppb_input_event_destroy(ptr);
            break;
        case PP_RESOURCE_FLASH_FONT_FILE:
            ppb_flash_font_file_destroy(ptr);
            break;
        case PP_RESOURCE_VIDEO_CAPTURE:
            ppb_video_capture_dev_destroy(ptr);
            break;
        case PP_RESOURCE_AUDIO_INPUT:
            ppb_audio_input_dev_destroy(ptr);
            break;
        case PP_RESOURCE_FLASH_MENU:
            ppb_flash_menu_destroy(ptr);
            break;
        case PP_RESOURCE_FLASH_MESSAGE_LOOP:
            ppb_flash_message_loop_destroy(ptr);
            break;
        case PP_RESOURCE_TCP_SOCKET:
            ppb_tcp_socket_private_destroy(ptr);
            break;
        default:
            break;
        }
    }

    if (ref_cnt <= 0) {
        pp_resource_expunge(resource);
        if (parent)
            pp_resource_unref(parent);
    }

    if (config.quirks.dump_resource_histogram) {
        time_t current_time = time(NULL);
        static uintptr_t throttling = 0;

        if (current_time % 5 == 0) {
            if (!throttling) {
                int counts[PP_RESOURCE_TYPES_COUNT + 1] = {};

                throttling = 1;
                pthread_mutex_lock(&res_tbl_lock);
                g_hash_table_foreach(res_tbl, _count_resources, counts);
                pthread_mutex_unlock(&res_tbl_lock);

                trace_error("-- %10lu ------------\n", (unsigned long)current_time);
                for (int k = 0; k < PP_RESOURCE_TYPES_COUNT; k ++)
                    trace_error("counts[%2d] = %d\n", k, counts[k]);
                if (counts[PP_RESOURCE_TYPES_COUNT] > 0)
                    trace_error("%d unknown resources (should never happen)\n",
                                counts[PP_RESOURCE_TYPES_COUNT]);
                trace_error("==========================\n");
            }
        } else {
            throttling = 0;
        }
    }
}
