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
#include <ppapi/c/private/pp_private_font_charset.h>
#include <ppapi/c/ppb_image_data.h>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/c/ppb_audio_config.h>
#include <ppapi/c/ppb_audio.h>

#include <stddef.h>
#include <X11/Xlib.h>
#include <npapi/npapi.h>
#include <npapi/npruntime.h>
#include <glib.h>
#include <GL/glx.h>
#include <pango/pango.h>
#include <pango/pangoft2.h>
#include <pango/pangocairo.h>
#include <cairo.h>
#include <asoundlib.h>


#define free_and_nullify(obj, field)    \
    do {                                \
        free(obj->field);               \
        obj->field = NULL;              \
    } while (0)

#define nullsafe_strdup(s)       ((s) ? strdup(s) : NULL)


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
};

enum pp_request_method_e {
    PP_METHOD_UNKNOWN,
    PP_METHOD_GET,
    PP_METHOD_POST,
};

struct pp_var_object_s {
    const struct PPP_Class_Deprecated *klass;
    void *data;
};

struct np_proxy_object_s {
    NPObject npobj;
    struct PP_Var ppobj;
};

struct pp_instance_s {
    const struct PPP_Instance_1_1  *ppp_instance_1_1;
    const struct PPP_InputEvent_0_1 *ppp_input_event;
    uint32_t        event_mask;
    uint32_t        filtered_event_mask;
    Window          wnd;
    PP_Instance     pp_instance_id;
    NPP             npp;
    uint32_t        is_fullframe;
    uint32_t        is_fullscreen;
    pthread_t       fs_thread;
    Window          fs_wnd;
    uint32_t        width;
    uint32_t        height;
    int             argc;
    const char    **argn;
    const char    **argv;
    int             instance_loaded;
    const char     *instance_url;
    pthread_t       pp_thread;
    PP_Resource     graphics;
    Display        *dpy;
};

struct pp_resource_generic_s {
    int             type;
    int             ref_cnt;
    PP_Instance     instance;
    pthread_mutex_t lock;
};

struct pp_url_loader_s {
    struct pp_resource_generic_s _;
    char                   *status_line;    ///< HTTP/1.1 200 OK
    char                   *headers;        ///< response headers
    int                     http_code;      ///< HTTP response code
    FILE                   *fp;             ///< file used to store response
    size_t                  read_pos;       ///< reading position
    enum pp_request_method_e method;        ///< GET/POST
    char                   *url;            ///< request URL
    char                   *redirect_url;   ///< value of the Location header if this is
                                            ///< a redirection response
    int                     loaded;         ///< if whole stream loaded already
    int32_t                 response_size;  ///< Content-Length value of -1 if absent

    char                   *request_headers;
    PP_Bool                 follow_redirects;   ///< handle redirections internally
    PP_Bool                 record_download_progress;
    PP_Bool                 record_upload_progress;
    char                   *custom_referrer_url;
    PP_Bool                 allow_cross_origin_requests;
    PP_Bool                 allow_credentials;
    char                   *custom_content_transfer_encoding;
    char                   *custom_user_agent;
    char                   *post_data;
    size_t                  post_len;
    GList                  *read_tasks;     ///< list of url_loader_read_task_s
    NPStream               *np_stream;      ///< associated NPStream
    struct PP_CompletionCallback    ccb;    ///< callback to call on headers arrival
};

struct url_loader_read_task_s {
    void                           *buffer;
    int32_t                         bytes_to_read;
    struct PP_CompletionCallback    ccb;
};

struct pp_url_request_info_s {
    struct pp_resource_generic_s _;
    enum pp_request_method_e    method;
    char                       *url;
    char                       *headers;
    PP_Bool                     stream_to_file;
    PP_Bool                     follow_redirects;
    PP_Bool                     record_download_progress;
    PP_Bool                     record_upload_progress;
    char                       *custom_referrer_url;
    PP_Bool                     allow_cross_origin_requests;
    PP_Bool                     allow_credentials;
    char                       *custom_content_transfer_encoding;
    int32_t                     prefetch_buffer_upper_threshold;
    int32_t                     prefetch_buffer_lower_threshold;
    char                       *custom_user_agent;
    size_t                      post_len;
    void                       *post_data;
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
    GLXContext      glc;
    GLXFBConfig     fb_config;
    Pixmap          pixmap;
    GLXPixmap       glx_pixmap;
    Display        *dpy;
    int32_t         width;
    int32_t         height;
    GHashTable     *sub_maps;
};

struct pp_image_data_s {
    struct pp_resource_generic_s _;
    int32_t             width;
    int32_t             height;
    int32_t             stride;
    char               *data;
    PP_ImageDataFormat  format;
    cairo_surface_t    *cairo_surf;
};

struct pp_graphics2d_s {
    struct pp_resource_generic_s _;
    PP_Instance         instance;
    int                 is_always_opaque;
    int32_t             width;
    int32_t             height;
    int32_t             stride;
    char               *data;
    char               *second_buffer;
    pthread_mutex_t     lock;
    cairo_surface_t    *cairo_surf;
    GList              *task_list;
};

struct pp_network_monitor_s {
    struct pp_resource_generic_s _;
};

struct pp_browser_font_s {
    struct pp_resource_generic_s _;
    PangoFont              *font;
    PangoFontDescription   *font_desc;
    int32_t                 letter_spacing;
    int32_t                 word_spacing;
    int32_t                 family;
};

struct pp_audio_config_s {
    struct pp_resource_generic_s _;
    PP_AudioSampleRate      sample_rate;
    uint32_t                sample_frame_count;
};

struct pp_audio_s {
    struct pp_resource_generic_s _;
    uint32_t                sample_rate;
    uint32_t                sample_frame_count;
    snd_pcm_t              *ph;
    void                   *audio_buffer;
    pthread_t               thread;
    uint32_t                playing;
    uint32_t                shutdown;
    PPB_Audio_Callback_1_0  callback;
    void                   *user_data;
};

struct pp_input_event_s {
    struct pp_resource_generic_s _;
    uint32_t                    event_class;
    PP_InputEvent_Type          type;
    PP_TimeTicks                time_stamp;
    uint32_t                    modifiers;
    PP_InputEvent_MouseButton   mouse_button;
    struct PP_Point             mouse_position;
    int32_t                     click_count;
    struct PP_Point             mouse_movement;
    struct PP_FloatPoint        wheel_delta;
    struct PP_FloatPoint        wheel_ticks;
    PP_Bool                     scroll_by_page;
    uint32_t                    key_code;
    struct PP_Var               character_text;
};

struct pp_flash_font_file_s {
    struct pp_resource_generic_s _;
    PangoFont              *font;
    FT_Face                 ft_face;
    PP_PrivateFontCharset   charset;
};

struct pp_printing_s {
    struct pp_resource_generic_s _;
};

struct pp_video_capture_s {
    struct pp_resource_generic_s _;
};

struct pp_audio_input_s {
    struct pp_resource_generic_s _;
};

PP_Resource             pp_resource_allocate(enum pp_resource_type_e type, PP_Instance instance);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);
void                    pp_resource_ref(PP_Resource resource);
void                    pp_resource_unref(PP_Resource resource);


#endif // FPP__PP_RESOURCE_H
