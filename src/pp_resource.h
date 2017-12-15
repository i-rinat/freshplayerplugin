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

#include <ppapi/c/pp_resource.h>
#include <pthread.h>
#include <stdint.h>

enum pp_resource_type_e {
    PP_RESOURCE_UNKNOWN = 0,
    PP_RESOURCE_URL_LOADER,
    PP_RESOURCE_URL_REQUEST_INFO,
    PP_RESOURCE_URL_RESPONSE_INFO,
    PP_RESOURCE_VIEW,
    PP_RESOURCE_GRAPHICS3D,
    PP_RESOURCE_IMAGE_DATA,
    PP_RESOURCE_GRAPHICS2D,
    PP_RESOURCE_NETWORK_MONITOR,
    PP_RESOURCE_BROWSER_FONT,
    PP_RESOURCE_AUDIO_CONFIG,
    PP_RESOURCE_AUDIO,
    PP_RESOURCE_INPUT_EVENT,
    PP_RESOURCE_FLASH_FONT_FILE,
    PP_RESOURCE_PRINTING,
    PP_RESOURCE_VIDEO_CAPTURE,
    PP_RESOURCE_AUDIO_INPUT,
    PP_RESOURCE_FLASH_MENU,
    PP_RESOURCE_FLASH_MESSAGE_LOOP,
    PP_RESOURCE_TCP_SOCKET,
    PP_RESOURCE_FILE_REF,
    PP_RESOURCE_FILE_IO,
    PP_RESOURCE_MESSAGE_LOOP,
    PP_RESOURCE_FLASH_DRM,
    PP_RESOURCE_VIDEO_DECODER,
    PP_RESOURCE_BUFFER,
    PP_RESOURCE_FILE_CHOOSER,
    PP_RESOURCE_UDP_SOCKET,
    PP_RESOURCE_X509_CERTIFICATE,
    PP_RESOURCE_FONT,
    PP_RESOURCE_DEVICE_REF,
    PP_RESOURCE_HOST_RESOLVER,
    PP_RESOURCE_NET_ADDRESS,

    PP_RESOURCE_TYPES_COUNT,        // must be the last item in the list
};

enum pp_request_method_e {
    PP_METHOD_UNKNOWN,
    PP_METHOD_GET,
    PP_METHOD_POST,
};

struct pp_instance_s;

#define LARGEST_RESOURCE_SIZE   1536

#define COMMON_STRUCTURE_FIELDS                 \
    uint32_t                resource_type;      \
    int                     ref_cnt;            \
    struct pp_instance_s   *instance;           \
    PP_Resource             self_id;            \
    pthread_mutex_t         lock;

PP_Resource             pp_resource_allocate(enum pp_resource_type_e type,
                                             struct pp_instance_s *instance);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);
PP_Resource             pp_resource_ref(PP_Resource resource);
void                    pp_resource_unref(PP_Resource resource);

void                    register_resource(enum pp_resource_type_e type,
                                          void (*destructor)(void *ptr));
