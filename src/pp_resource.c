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
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include "ppb_graphics2d.h"
#include "ppb_image_data.h"
#include "ppb_url_loader.h"
#include "ppb_url_request_info.h"
#include "ppb_browser_font_trusted.h"
#include "ppb_audio_config.h"
#include "ppb_audio.h"
#include "ppb_input_event.h"
#include "ppb_flash_font_file.h"
#include "ppb_video_capture_dev.h"
#include "ppb_audio_input_dev.h"


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
    void *ptr;

#define ALLOC_HELPER(typename)                  \
    ptr = calloc(sizeof(typename), 1);          \
    ((typename *)ptr)->_.type = type;           \
    ((typename *)ptr)->_.ref_cnt = 1;           \
    ((typename *)ptr)->_.instance = instance;

    pthread_mutex_lock(&res_tbl_lock);
    switch (type) {
    case PP_RESOURCE_URL_LOADER:
        ALLOC_HELPER(struct pp_url_loader_s);
        break;
    case PP_RESOURCE_URL_REQUEST_INFO:
        ALLOC_HELPER(struct pp_url_request_info_s);
        break;
    case PP_RESOURCE_URL_RESPONSE_INFO:
        ALLOC_HELPER(struct pp_url_response_info_s);
        break;
    case PP_RESOURCE_VIEW:
        ALLOC_HELPER(struct pp_view_s);
        break;
    case PP_RESOURCE_GRAPHICS3D:
        ALLOC_HELPER(struct pp_graphics3d_s);
        break;
    case PP_RESOURCE_IMAGE_DATA:
        ALLOC_HELPER(struct pp_image_data_s);
        break;
    case PP_RESOURCE_GRAPHICS2D:
        ALLOC_HELPER(struct pp_graphics2d_s);
        break;
    case PP_RESOURCE_NETWORK_MONITOR:
        ALLOC_HELPER(struct pp_network_monitor_s);
        break;
    case PP_RESOURCE_BROWSER_FONT:
        ALLOC_HELPER(struct pp_browser_font_s);
        break;
    case PP_RESOURCE_AUDIO_CONFIG:
        ALLOC_HELPER(struct pp_audio_config_s);
        break;
    case PP_RESOURCE_AUDIO:
        ALLOC_HELPER(struct pp_audio_s);
        break;
    case PP_RESOURCE_INPUT_EVENT:
        ALLOC_HELPER(struct pp_input_event_s);
        break;
    case PP_RESOURCE_FLASH_FONT_FILE:
        ALLOC_HELPER(struct pp_flash_font_file_s);
        break;
    case PP_RESOURCE_PRINTING:
        ALLOC_HELPER(struct pp_printing_s);
        break;
    case PP_RESOURCE_VIDEO_CAPTURE:
        ALLOC_HELPER(struct pp_video_capture_s);
        break;
    case PP_RESOURCE_AUDIO_INPUT:
        ALLOC_HELPER(struct pp_audio_input_s);
        break;
    default:
        // fall through
    case PP_RESOURCE_UNKNOWN:
        ptr = calloc(sizeof(struct pp_resource_generic_s), 1);
        ((struct pp_resource_generic_s *)ptr)->type = type;
        break;
    }

    int handle = res_tbl_next ++;
    g_hash_table_insert(res_tbl, GINT_TO_POINTER(handle), ptr);

    pthread_mutex_unlock(&res_tbl_lock);
    return handle;
}

void
pp_resource_expunge(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    void *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        free(ptr);
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
    }
    pthread_mutex_unlock(&res_tbl_lock);
}

void
pp_resource_unref(PP_Resource resource)
{
    PP_Resource parent = 0;
    int ref_cnt;

    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    pthread_mutex_unlock(&res_tbl_lock);

    if (!ptr)
        return;

    ref_cnt = --ptr->ref_cnt;
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
        default:
            break;
        }
    }

    if (ref_cnt <= 0) {
        pp_resource_expunge(resource);
        if (parent)
            pp_resource_unref(parent);
    }
}
