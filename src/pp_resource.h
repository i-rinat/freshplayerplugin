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
#include <X11/Xlib.h>
#include <npapi.h>
#include <EGL/egl.h>
#include <pango/pango.h>
#include <pango/pangoft2.h>
#include <pango/pangocairo.h>
#include <cairo.h>

#define FREE_HELPER(st, field)  if (st->field) { free(st->field); st->field = NULL; }

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
};

enum pp_request_method_e {
    PP_METHOD_UNKNOWN,
    PP_METHOD_GET,
    PP_METHOD_POST,
};

struct pp_var_object_s {
    const struct PPP_Class_Deprecated *klass;
    void *data;
    NPP   npp;
};

struct pp_instance_s {
    const struct PPP_Instance_1_1  *ppp_instance_1_1;
    Window          wnd;
    PP_Instance     pp_instance_id;
    NPP             npp;
    uint32_t        x;
    uint32_t        y;
    uint32_t        width;
    uint32_t        height;
    NPRect          clip_rect;
    void           *ws_info;
    NPWindowType    window_type;
    int             argc;
    const char    **argn;
    const char    **argv;
    int             instance_loaded;
    const char     *instance_url;
    pthread_t       pp_thread;
    PP_Resource     graphics;
    int             draw_in_progress;
    struct PP_CompletionCallback draw_completion_callback;
    Display        *dpy;
    EGLDisplay      egl_dpy;
};

struct pp_resource_generic_s {
    int         type;
    int         ref_cnt;
    PP_Instance instance;
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
    enum pp_request_method_e    method;
    char       *url;
    char       *headers;
    PP_Bool     stream_to_file;
    PP_Bool     follow_redirects;
    PP_Bool     record_download_progress;
    PP_Bool     record_upload_progress;
    char       *custom_referrer_url;
    PP_Bool     allow_cross_origin_requests;
    PP_Bool     allow_credentials;
    char       *custom_content_transfer_encoding;
    int32_t     prefetch_buffer_upper_threshold;
    int32_t     prefetch_buffer_lower_threshold;
    char       *custom_user_agent;
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
    EGLDisplay      egl_dpy;
    EGLContext      egl_ctx;
    EGLConfig       egl_config;
    EGLSurface      egl_surf;
    int32_t         width;
    int32_t         height;
};

struct pp_image_data_s {
    struct pp_resource_generic_s _;
    int32_t             width;
    int32_t             height;
    int32_t             stride;
    char               *data;
    PP_ImageDataFormat  format;
    cairo_surface_t    *cairo_surf;
    cairo_t            *cairo_ctx;
};

struct pp_graphics2d_s {
    struct pp_resource_generic_s _;
    PP_Instance instance;
    int         is_always_opaque;
    int32_t     width;
    int32_t     height;
    int32_t     stride;
    char       *data;
};

struct pp_network_monitor_s {
    struct pp_resource_generic_s _;
};

struct pp_browser_font_s {
    struct pp_resource_generic_s _;
    PangoContext           *ctx;
    PangoFont              *font;
    PangoFontDescription   *font_desc;
    int32_t                 letter_spacing;
    int32_t                 word_spacing;
};

PP_Resource             pp_resource_allocate(enum pp_resource_type_e type, PP_Instance instance);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire_any(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);
void                    pp_resource_ref(PP_Resource resource);
void                    pp_resource_unref(PP_Resource resource);


#endif // FPP__PP_RESOURCE_H
