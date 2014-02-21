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

#ifndef FPP__PP_RESOURCE_H
#define FPP__PP_RESOURCE_H

#include <ppapi/c/pp_completion_callback.h>
#include <ppapi/c/pp_rect.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/ppb_image_data.h>
#include <stddef.h>

enum pp_resource_type_e {
    PP_RESOURCE_UNKNOWN = 0,
    PP_RESOURCE_URL_LOADER,
    PP_RESOURCE_URL_REQUEST_INFO,
    PP_RESOURCE_URL_RESPONSE_INFO,
    PP_RESOURCE_VIEW,
    PP_RESOURCE_GRAPHICS3D,
    PP_RESOURCE_IMAGE_DATA,
    PP_RESOURCE_GRAPHICS2D,
};


struct pp_resource_generic_s {
    int         type;
    int         ref_cnt;
};

struct pp_url_loader_s {
    struct pp_resource_generic_s _;
    char       *headers;        ///< response headers
    char       *body;           ///< storage for response
    size_t      body_size;      ///< got so far
    size_t      body_allocated; ///< allocated size for .body
    size_t      body_pos;       ///< read stream position
    char       *url;            ///< request URL
    int         loaded;         ///< if whole stream loaded already
    struct PP_CompletionCallback    ccb;
};

struct pp_url_request_info_s {
    struct pp_resource_generic_s _;
};

struct pp_url_response_info_s {
    struct pp_resource_generic_s _;
    PP_Resource     url_loader;
};

struct pp_view_s {
    struct pp_resource_generic_s _;
    struct PP_Rect      rect;
};

struct pp_graphics3d_s {
    struct pp_resource_generic_s _;
};

struct pp_image_data_s {
    struct pp_resource_generic_s _;
    int32_t             width;
    int32_t             height;
    int32_t             stride;
    char               *data;
    PP_ImageDataFormat  format;
};

struct pp_graphics2d_s {
    struct pp_resource_generic_s _;
};


PP_Resource             pp_resource_allocate(enum pp_resource_type_e type);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire_any(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);
void                    pp_resource_ref(PP_Resource resource);
void                    pp_resource_unref(PP_Resource resource);


#endif // FPP__PP_RESOURCE_H
