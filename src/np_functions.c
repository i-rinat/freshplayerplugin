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

#define GL_GLEXT_PROTOTYPES
#include <assert.h>
#include <inttypes.h>
#include <npapi/npapi.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <GL/gl.h>
#include <pthread.h>
#include "trace.h"
#include "reverse_constant.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "tables.h"
#include "p2n_proxy_class.h"
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp_input_event.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/private/ppp_instance_private.h>
#include "ppb_input_event.h"
#include "ppb_url_loader.h"
#include "ppb_url_request_info.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include "header_parser.h"
#include "keycodeconvert.h"


static
PP_Instance
generate_new_pp_instance_id(void)
{
    static int32_t instance_id = 10;
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&m);
    int32_t result = instance_id++;
    pthread_mutex_unlock(&m);

    return result;
}

static
void
empty_completion_callback(void *user_data, int32_t result)
{
    (void)user_data;
    (void)result;
}

NPError
NPP_New(NPMIMEType pluginType, NPP npp, uint16_t mode, int16_t argc, char *argn[],
        char *argv[], NPSavedData *saved)
{
    // TODO: mode parameter handling
    int k;
    struct pp_instance_s *pp_i;
    trace_info("[NPP] {part} %s pluginType=%s npp=%p, mode=%d, argc=%d, saved=%p\n", __func__,
               pluginType, npp, mode, argc, saved);

    tables_add_npp_instance(npp);
    np_main_thread = pthread_self();

    for (k = 0; k < argc; k ++)
        trace_info("            argn[%d] = %s, argv[%d] = %s\n", k, argn[k], k, argv[k]);

    pp_i = calloc(sizeof(*pp_i), 1);
    npp->pdata = pp_i;
    if (!pp_i)
        return NPERR_OUT_OF_MEMORY_ERROR;

    pp_i->npp = npp;
    pp_i->ppp_instance_1_1 = ppp_get_interface(PPP_INSTANCE_INTERFACE_1_1);
    if (!pp_i->ppp_instance_1_1)
        return NPERR_GENERIC_ERROR;
    pp_i->ppp_input_event = ppp_get_interface(PPP_INPUT_EVENT_INTERFACE_0_1);

    pp_i->argc = argc;
    pp_i->argn = malloc(argc * sizeof(char*));
    pp_i->argv = malloc(argc * sizeof(char*));
    for (k = 0; k < argc; k ++) {
        pp_i->argn[k] = strdup(argn[k] ? argn[k] : "");
        pp_i->argv[k] = strdup(argv[k] ? argv[k] : "");

        if (strcasecmp(argn[k], "src") == 0) {
            pp_i->instance_url = strdup(argv[k]);
        }
    }

    pp_i->is_fullframe = (mode == NP_FULL);
    pp_i->pp_instance_id = generate_new_pp_instance_id();
    tables_add_pp_instance(pp_i->pp_instance_id, pp_i);

    pthread_mutex_init(&pp_i->lock, NULL);
    pp_i->dpy = XOpenDisplay(NULL);
    if (!pp_i->dpy) {
        trace_error("%s, can't open X Display\n", __func__);
    }

    // get fullscreen resolution
    int screen_count;
    pthread_mutex_lock(&pp_i->lock);
    XineramaScreenInfo *xsi = XineramaQueryScreens(pp_i->dpy, &screen_count);
    XWindowAttributes xw_attrs;
    if (xsi) {
        int screen = config.xinerama_screen;
        if (screen < 0)
            screen = 0;
        if (screen > screen_count - 1)
            screen = screen_count - 1;
        pp_i->fs_width =  xsi[screen].width;
        pp_i->fs_height = xsi[screen].height;
        XFree(xsi);
    } else if (XGetWindowAttributes(pp_i->dpy, DefaultRootWindow(pp_i->dpy), &xw_attrs)) {
        pp_i->fs_width =  xw_attrs.width;
        pp_i->fs_height = xw_attrs.height;
    } else {
        pp_i->fs_width = 100;
        pp_i->fs_height = 100;
    }
    pthread_mutex_unlock(&pp_i->lock);

    // request windowless operation
    npn.setvalue(npp, NPPVpluginWindowBool, (void*)0);

    pp_i->ppp_instance_1_1->DidCreate(pp_i->pp_instance_id, pp_i->argc, pp_i->argn, pp_i->argv);
    pp_i->instance_loaded = 1;

    if (mode == NP_FULL) {
        PP_Resource request_info = ppb_url_request_info_create(pp_i->pp_instance_id);
        PP_Resource url_loader = ppb_url_loader_create(pp_i->pp_instance_id);

        struct PP_Var s_url = PP_MakeString(pp_i->instance_url);
        struct PP_Var s_method = PP_MakeString("GET");

        ppb_url_request_info_set_property(request_info, PP_URLREQUESTPROPERTY_URL, s_url);
        ppb_url_request_info_set_property(request_info, PP_URLREQUESTPROPERTY_METHOD, s_method);
        ppb_url_loader_open(url_loader, request_info,
                            PP_MakeCompletionCallback(empty_completion_callback, NULL));
        ppb_var_release(s_url);
        ppb_var_release(s_method);
        ppb_core_release_resource(request_info);

        pp_i->ppp_instance_1_1->HandleDocumentLoad(pp_i->pp_instance_id, url_loader);
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP npp, NPSavedData **save)
{
    trace_info("[NPP] {full} %s npp=%p, save=%p\n", __func__, npp, save);
    tables_remove_npp_instance(npp);
    struct pp_instance_s *pp_i = npp->pdata;
    pp_i->ppp_instance_1_1->DidDestroy(pp_i->pp_instance_id);
    if (save)
        *save = NULL;
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP npp, NPWindow *window)
{
    char *window_str = trace_np_window_as_string(window);
    trace_info("[NPP] {full} %s npp=%p, window=%s\n", __func__, npp, window_str);
    g_free(window_str);

    struct pp_instance_s *pp_i = npp->pdata;

    if (pp_i && !pp_i->is_fullscreen) {
        pp_i->wnd = (Window)window->window;
        pp_i->width = window->width;
        pp_i->height = window->height;

        if (pp_i->instance_loaded) {
            PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i->pp_instance_id);
            struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
            v->rect.point.x = window->x;
            v->rect.point.y = window->y;
            v->rect.size.width = window->width;
            v->rect.size.height = window->height;
            pp_resource_release(view);

            pp_i->ppp_instance_1_1->DidChangeView(pp_i->pp_instance_id, view);
            ppb_core_release_resource(view);
        }
    }

    return NPERR_NO_ERROR;
}

static
void
do_nothing(void *user_data, int32_t result)
{
    (void)user_data;
    (void)result;
}

NPError
NPP_NewStream(NPP npp, NPMIMEType type, NPStream *stream, NPBool seekable, uint16_t *stype)
{
    trace_info("[NPP] {full} %s npp=%p, type=%s, stream={.pdata=%p, .ndata=%p, .url=%s, "
               "end=%u, lastmodified=%u, .notifyData=%p, .headers=%s}, seekable=%d\n", __func__,
               npp, type, stream->pdata, stream->ndata, stream->url, stream->end,
               stream->lastmodified, stream->notifyData, stream->headers, seekable);

    PP_Resource loader = (size_t)stream->notifyData;
    if (!loader) {
        // ignoring unrequested streams
        stream->pdata = NULL;
        trace_info("      ignoring unrequested stream\n");
        return NPERR_NO_ERROR;
    }

    struct PP_CompletionCallback ccb;
    ccb.func = NULL;

    stream->pdata = (void*)(size_t)loader;
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);

    if (ul) {
        struct parsed_headers_s *ph = hp_parse_headers(stream->headers);
        unsigned int headers_len = 0;
        ccb = ul->ccb;
        ul->np_stream = stream;

        // handling redirection
        ul->redirect_url = nullsafe_strdup(hp_get_header_value(ph, "Location"));
        if (ph->http_code >= 300 && ph->http_code <= 307 && ul->redirect_url) {
            if (ul->follow_redirects) {
                trace_info("       %s, redirecting to %s\n", __func__, ul->redirect_url);
                pp_resource_release(loader);
                ppb_url_loader_follow_redirect(loader, PP_MakeCompletionCallback(do_nothing, NULL));
                // There is no need to fill response headers, status_line and other parameters
                // since they are freed in follow_redirect anyway.
                goto quit;
            }
        }

        for (unsigned int k = 0; k < ph->cnt; k ++)
            headers_len += strlen(ph->name[k]) + strlen(": ") + strlen(ph->value[k]) + strlen("\n");

        // recostruct headers
        ul->headers = malloc(headers_len + 1);
        char *ptr = ul->headers;
        for (unsigned int k = 0; k < ph->cnt; k ++) {
            memcpy(ptr, ph->name[k], strlen(ph->name[k]));  ptr += strlen(ph->name[k]);
            memcpy(ptr, ": ", strlen(": "));                ptr += strlen(": ");
            memcpy(ptr, ph->value[k], strlen(ph->value[k]));ptr += strlen(ph->value[k]);
            memcpy(ptr, "\n", strlen("\n"));                ptr += strlen("\n");
        }
        *ptr = 0;
        ul->http_code = ph->http_code;
        ul->response_size = stream->end > 0 ? stream->end : -1;
        ul->status_line = nullsafe_strdup(ph->status_line);

        hp_free_parsed_headers(ph);
        pp_resource_release(loader);
    }

quit:
    if (ccb.func)
        ccb.func(ccb.user_data, PP_OK);

    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP npp, NPStream *stream, NPReason reason)
{
    trace_info("[NPP] {full} %s npp=%p, stream=%p, reason=%d\n", __func__, npp, stream, reason);

    PP_Resource loader = (PP_Resource)(size_t)stream->pdata;
    if (!loader)
        return NPERR_NO_ERROR;

    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul)
        return NPERR_NO_ERROR;

    ul->loaded = 1;

    // execute all remaining tasks in task list
    while (ul && ul->read_tasks) {
        GList *llink = g_list_first(ul->read_tasks);
        struct url_loader_read_task_s *rt = llink->data;
        ul->read_tasks = g_list_delete_link(ul->read_tasks, llink);

        fseek(ul->fp, ul->read_pos, SEEK_SET);
        int32_t read_bytes = fread(rt->buffer, 1, rt->bytes_to_read, ul->fp);
        ul->read_pos += read_bytes;

        pp_resource_release(loader);
        if (rt->ccb.func)
            rt->ccb.func(rt->ccb.user_data, read_bytes);
        g_slice_free(struct url_loader_read_task_s, rt);
        ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    }

    pp_resource_release(loader);
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP npp, NPStream *stream)
{
    trace_info("[NPP] {full} %s npp=%p, stream=%p\n", __func__, npp, stream);
    return 1024*1024;
}

int32_t
NPP_Write(NPP npp, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{
    trace_info("[NPP] {full} %s npp=%p, stream=%p, offset=%d, len=%d, buffer=%p\n", __func__,
               npp, stream, offset, len, buffer);

    PP_Resource loader = (PP_Resource)(size_t)stream->pdata;
    if (!loader)
        return len;

    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
            trace_info("[NPP] %s, ignoring stream content\n", __func__);
            return len;
    }

    if (!ul->fp || len <= 0) {
        pp_resource_release(loader);
        return len;
    }

    fseek(ul->fp, offset, SEEK_SET);
    fwrite(buffer, len, 1, ul->fp);

    if (ul->read_tasks == NULL) {
        pp_resource_release(loader);
        return len;
    }

    GList *llink = g_list_first(ul->read_tasks);
    struct url_loader_read_task_s *rt = llink->data;
    ul->read_tasks = g_list_delete_link(ul->read_tasks, llink);

    fseek(ul->fp, ul->read_pos, SEEK_SET);
    int32_t read_bytes = fread(rt->buffer, 1, rt->bytes_to_read, ul->fp);
    ul->read_pos += read_bytes;

    if (read_bytes > 0) {
        pp_resource_release(loader);
        if (rt->ccb.func)
            rt->ccb.func(rt->ccb.user_data, read_bytes);
        g_slice_free(struct url_loader_read_task_s, rt);
        return len;
    } else {
        // reschedule task
        ul->read_tasks = g_list_prepend(ul->read_tasks, rt);
        pp_resource_release(loader);
        return len;
    }
}

void
NPP_StreamAsFile(NPP npp, NPStream *stream, const char *fname)
{
    trace_info("[NPP] {zilch} %s npp=%p, stream=%p, fname=%s\n", __func__, npp, stream, fname);
    return;
}

void
NPP_Print(NPP npp, NPPrint *platformPrint)
{
    trace_info("[NPP] {zilch} %s npp=%p, platformPrint=%p\n", __func__, npp, platformPrint);
    return;
}

static
int16_t
handle_graphics_expose_event(NPP npp, void *event)
{
    XGraphicsExposeEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    struct pp_graphics2d_s *g2d = pp_resource_acquire(pp_i->graphics, PP_RESOURCE_GRAPHICS2D);
    struct pp_graphics3d_s *g3d = pp_resource_acquire(pp_i->graphics, PP_RESOURCE_GRAPHICS3D);
    Display *dpy = ev->display;
    Drawable drawable = ev->drawable;
    int screen = 0;

    if (g2d) {
        pthread_mutex_lock(&pp_i->lock);
        XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, screen), 24, ZPixmap, 0,
                                  g2d->second_buffer, g2d->scaled_width, g2d->scaled_height, 32,
                                  g2d->scaled_stride);

        XPutImage(dpy, drawable, DefaultGC(dpy, screen), xi, 0, 0, 0, 0,
                  g2d->scaled_width, g2d->scaled_height);
        XFree(xi);
        pthread_mutex_unlock(&pp_i->lock);
        pp_resource_release(pp_i->graphics);
    } else if (g3d) {
        pthread_mutex_lock(&pp_i->lock);

        if (pp_i->is_fullscreen) {
            glXMakeCurrent(pp_i->dpy, drawable, g3d->presentation_glc);
        } else {
            glXMakeCurrent(pp_i->dpy, g3d->glx_pixmap, g3d->presentation_glc);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, g3d->width, g3d->height);
        glOrtho(0, g3d->width, 0, g3d->height, -1.0, 1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glMatrixMode(GL_TEXTURE);
        glLoadIdentity();

        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, g3d->tex_id);

        glColor4f(1, 1, 1, 1);
        glBegin(GL_QUADS);
            glTexCoord2i(0, 0); glVertex2i(0, 0);
            glTexCoord2i(1, 0); glVertex2i(g3d->width, 0);
            glTexCoord2i(1, 1); glVertex2i(g3d->width, g3d->height);
            glTexCoord2i(0, 1); glVertex2i(0, g3d->height);
        glEnd();

        glFinish();

        if (pp_i->is_fullscreen) {
            glXSwapBuffers(dpy, drawable);
        } else {
            XCopyArea(dpy, g3d->pixmap, drawable, DefaultGC(dpy, screen),
                      0, 0,
                      g3d->width, g3d->height,
                      0, 0);
            XFlush(dpy);
        }

        pthread_mutex_unlock(&pp_i->lock);
        pp_resource_release(pp_i->graphics);
    } else {
        return 0;
    }

    return 1;
}

static
unsigned int
x_state_mask_to_pp_inputevent_modifier(unsigned int state)
{
    unsigned int mod = 0;

    // TODO: refine this
    if (state & ShiftMask) {
        mod |= PP_INPUTEVENT_MODIFIER_SHIFTKEY;
        mod |= PP_INPUTEVENT_MODIFIER_ISLEFT;
    }

    if (state & LockMask) {
        mod |= PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY;
    }

    if (state & ControlMask) {
        mod |= PP_INPUTEVENT_MODIFIER_CONTROLKEY;
        mod |= PP_INPUTEVENT_MODIFIER_ISLEFT;
    }

    if (state & Mod1Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_ALTKEY;
        mod |= PP_INPUTEVENT_MODIFIER_ISLEFT;
    }

    if (state & Mod2Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_NUMLOCKKEY;
    }

    if (state & Mod4Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_METAKEY;
        mod |= PP_INPUTEVENT_MODIFIER_ISLEFT;
    }

    if (state & Button1Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN;
    }

    if (state & Button2Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN;
    }

    if (state & Button3Mask) {
        mod |= PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN;
    }

    return mod;
}

static
int16_t
handle_enter_leave_event(NPP npp, void *event)
{
    XCrossingEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;

    // quit if plugin doesn't handle input events
    if (!pp_i->ppp_input_event)
        return 0;

    const uint32_t combined_mask = pp_i->event_mask | pp_i->filtered_event_mask;
    if (!(PP_INPUTEVENT_CLASS_MOUSE & combined_mask))
        return 0;

    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
    PP_InputEvent_Type event_type = (ev->type == EnterNotify) ? PP_INPUTEVENT_TYPE_MOUSEENTER
                                                              : PP_INPUTEVENT_TYPE_MOUSELEAVE;
    PP_Resource pp_event;
    pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                            ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                            &mouse_position, 0, &zero_point);
    PP_Bool ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
    ppb_core_release_resource(pp_event);

    // return false only if handler returns PP_FALSE and event is filtered
    if (ret == PP_FALSE && (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
        return 0;

    // for non-filtered events return values are ignored
    return 1;
}

static
int16_t
handle_motion_event(NPP npp, void *event)
{
    XMotionEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;

    // quit if plugin doesn't handle input events
    if (!pp_i->ppp_input_event)
        return 0;

    const uint32_t combined_mask = pp_i->event_mask | pp_i->filtered_event_mask;
    if (!(PP_INPUTEVENT_CLASS_MOUSE & combined_mask))
        return 0;

    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
    PP_Resource pp_event;

    pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, PP_INPUTEVENT_TYPE_MOUSEMOVE,
                                            ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                            &mouse_position, 0, &zero_point);
    PP_Bool ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
    ppb_core_release_resource(pp_event);

    // return false only if handler returns PP_FALSE and event is filtered
    if (ret == PP_FALSE && (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
        return 0;

    // for non-filtered events return values are ignored
    return 1;
}

static
int16_t
handle_button_press_release_event(NPP npp, void *event)
{
    XButtonEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    uint32_t event_class = 0;
    PP_InputEvent_MouseButton mouse_button = PP_INPUTEVENT_MOUSEBUTTON_NONE;

    // quit if plugin doesn't handle input events
    if (!pp_i->ppp_input_event)
        return 0;

    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
    float wheel_x = 0.0, wheel_y = 0.0;
    int ev_button = ev->button;

    if (config.quirks.switch_buttons_2_3) {
        if (ev_button == 2)
            ev_button = 3;
        else if (ev_button == 3)
            ev_button = 2;
    }

    switch (ev_button) {
    case 1:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_LEFT;
        event_class = PP_INPUTEVENT_CLASS_MOUSE;
        break;
    case 2:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_MIDDLE;
        event_class = PP_INPUTEVENT_CLASS_MOUSE;
        break;
    case 3:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_RIGHT;
        event_class = PP_INPUTEVENT_CLASS_MOUSE;
        break;

    case 4: // wheel up
        wheel_y = -1;
        event_class = PP_INPUTEVENT_CLASS_WHEEL;
        break;
    case 5: // wheel down
        wheel_y = 1;
        event_class = PP_INPUTEVENT_CLASS_WHEEL;
        break;
    case 6: // wheel left
        wheel_x = -1;
        event_class = PP_INPUTEVENT_CLASS_WHEEL;
        break;
    case 7: // wheel right
        wheel_x = 1;
        event_class = PP_INPUTEVENT_CLASS_WHEEL;
        break;
    }

    // TODO: Firefox does not pass wheel event to windowless plugins.
    //       Only windowed plugins can receive such events.


    const uint32_t combined_mask = pp_i->event_mask | pp_i->filtered_event_mask;
    if (!(event_class & combined_mask))
        return 0;

    PP_Bool handled = PP_FALSE;

    if (event_class == PP_INPUTEVENT_CLASS_MOUSE) {
        PP_Resource         pp_event;
        PP_InputEvent_Type  event_type;
        PP_Bool             ret;

        event_type = (ev->type == ButtonPress) ? PP_INPUTEVENT_TYPE_MOUSEDOWN
                                               : PP_INPUTEVENT_TYPE_MOUSEUP;
        pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                                ev->time/1.0e6, mod, mouse_button,
                                                &mouse_position, 1, &zero_point);

        ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
        handled = handled || ret;
        ppb_core_release_resource(pp_event);

        // context menu event
        if (ev->type == ButtonRelease && ev_button == 3) {
            pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id,
                                                    PP_INPUTEVENT_TYPE_CONTEXTMENU,
                                                    ev->time/1.0e6, mod, mouse_button,
                                                    &mouse_position, 1, &zero_point);
            ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
            handled = handled || ret;
            ppb_core_release_resource(pp_event);
        }
    } else { // event_class == PP_INPUTEVENT_CLASS_WHEEL
        const float scroll_by_tick = 10.0;
        struct PP_FloatPoint wheel_delta = { .x = wheel_x * scroll_by_tick,
                                             .y = wheel_y * scroll_by_tick };
        struct PP_FloatPoint wheel_ticks = { .x = wheel_x, .y = wheel_y };

        // pp_event = ppb_wheel_input_event_create(
        //                 pp_i->pp_instance_id, ppb_core_get_time_ticks(), mod,
        //                 &wheel_delta, &wheel_ticks, PP_FALSE);
        (void)wheel_delta;
        (void)wheel_ticks;
        return 0;
    }

    // return false only if handler returns PP_FALSE and event is filtered
    if (!handled && (pp_i->filtered_event_mask & event_class))
        return 0;

    // for non-filtered events return values are ignored
    return 1;
}

int16_t
handle_key_press_release_event(NPP npp, void *event)
{
    XKeyEvent            *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    PP_InputEvent_Type    event_type;

    // quit if plugin doesn't handle input events
    if (!pp_i->ppp_input_event)
        return 0;

    event_type = (ev->type == KeyPress) ? PP_INPUTEVENT_TYPE_KEYDOWN
                                        : PP_INPUTEVENT_TYPE_KEYUP;
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);

    const uint32_t combined_mask = pp_i->event_mask | pp_i->filtered_event_mask;
    if (!(PP_INPUTEVENT_CLASS_KEYBOARD & combined_mask))
        return 0;

    PP_Bool        ret = PP_FALSE;
    char           buffer[20];
    KeySym         keysym;
    XComposeStatus compose_status;
    int            charcount;
    int            pp_keycode;
    PP_Resource    pp_event;

    charcount = XLookupString(ev, buffer, sizeof(buffer), &keysym, &compose_status);
    pp_keycode = xkeycode_to_pp_keycode(keysym);

    if (ev->type == KeyPress && charcount > 0) {
        struct PP_Var character_text = PP_MakeStringN(buffer, charcount);
        pp_event = ppb_keyboard_input_event_create(
                        pp_i->pp_instance_id, PP_INPUTEVENT_TYPE_CHAR, ev->time/1.0e6, mod,
                        pp_keycode, character_text);
        ppb_var_release(character_text);

        pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
        ppb_core_release_resource(pp_event);
    }

    pp_event = ppb_keyboard_input_event_create(pp_i->pp_instance_id, event_type, ev->time/1.0e6,
                                               mod, pp_keycode, PP_MakeUndefined());
    ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);
    ppb_core_release_resource(pp_event);

    // return false only if handler returns PP_FALSE and event is filtered
    if (ret == PP_FALSE && (PP_INPUTEVENT_CLASS_KEYBOARD & pp_i->filtered_event_mask))
        return 0;

    // for non-filtered events return values are ignored
    return 1;
}

int16_t
NPP_HandleEvent(NPP npp, void *event)
{
    XAnyEvent *xaev = event;

#define TRACE_HELPER(implstatus)                                                    \
    trace_info("[NPP] " implstatus " %s npp=%p, event={.type=%s, .serial=%lu, "   \
               ".send_event=%d, .display=%p, .window=0x%x}\n", __func__, npp,       \
               reverse_xevent_type(xaev->type), xaev->serial, xaev->send_event,     \
               xaev->display, (uint32_t)xaev->window)

    struct pp_instance_s *pp_i = npp->pdata;

    if (pp_i && pp_i->is_fullscreen && pp_i->fs_wnd != xaev->window) {
        return 0;
    }

    switch (xaev->type) {
    case Expose:
        TRACE_HELPER("{full}");
        // its event have similar layout to GraphicsExpose, so let ge handler to do the work
        return handle_graphics_expose_event(npp, event);
    case GraphicsExpose:
        TRACE_HELPER("{full}");
        return handle_graphics_expose_event(npp, event);
    case EnterNotify:
        TRACE_HELPER("{full}");
        return handle_enter_leave_event(npp, event);
    case LeaveNotify:
        TRACE_HELPER("{full}");
        return handle_enter_leave_event(npp, event);
    case MotionNotify:
        TRACE_HELPER("{full}");
        return handle_motion_event(npp, event);
    case ButtonPress:
        TRACE_HELPER("{full}");
        return handle_button_press_release_event(npp, event);
    case ButtonRelease:
        TRACE_HELPER("{full}");
        return handle_button_press_release_event(npp, event);
    case KeyPress:
        TRACE_HELPER("{full}");
        return handle_key_press_release_event(npp, event);
    case KeyRelease:
        TRACE_HELPER("{full}");
        return handle_key_press_release_event(npp, event);
    default:
        TRACE_HELPER("{zilch}");
        return 0;
    }
#undef TRACE_HELPER
}

void
NPP_URLNotify(NPP npp, const char *url, NPReason reason, void *notifyData)
{
    trace_info("[NPP] {full} %s npp=%p, url=%s, reason=%d, notifyData=%p\n", __func__,
               npp, url, reason, notifyData);
    // This is no-op. We are handling request in NPP_NewStream function.
    return;
}

NPError
NPP_GetValue(NPP npp, NPPVariable variable, void *value)
{
    const char *var_name = reverse_npp_variable(variable);

    switch (variable) {
    case NPPVpluginNameString:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginDescriptionString:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginWindowBool:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginTransparentBool:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVjavaClass:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginWindowSize:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginTimerInterval:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginScriptableInstance:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginScriptableIID:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVjavascriptPushCallerBool:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginKeepLibraryInMemory:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginNeedsXEmbed:
        trace_info("[NPP] {full} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        *(int *)value = 0;
        break;
    case NPPVpluginScriptableNPObject:
        trace_info("[NPP] {full} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        do {
            struct pp_instance_s *pp_i = npp->pdata;
            const struct PPP_Instance_Private_0_1 *ppp_instance_private =
                ppp_get_interface(PPP_INSTANCE_PRIVATE_INTERFACE_0_1);
            if (!ppp_instance_private || !ppp_instance_private->GetInstanceObject) {
                *(void **)value = NULL;
                break;
            }

            struct PP_Var ppobj = ppp_instance_private->GetInstanceObject(pp_i->pp_instance_id);
            NPVariant np_var = pp_var_to_np_variant(ppobj);
            ppb_var_release(ppobj);

            *(void **)value = np_var.value.objectValue;
            tables_add_npobj_npp_mapping(np_var.value.objectValue, npp);
        } while (0);
        break;
    case NPPVformValue:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginUrlRequestsDisplayedBool:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginWantsAllNetworkStreams:
        trace_info("[NPP] {full} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        *(int *)value = 1;
        break;
    case NPPVpluginNativeAccessibleAtkPlugId:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginCancelSrcStream:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVsupportsAdvancedKeyHandling:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginUsesDOMForCursorBool:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    case NPPVpluginDrawingModel:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    default:
        trace_info("[NPP] {zilch} %s npp=%p, variable=%s\n", __func__, npp, var_name);
        break;
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP npp, NPNVariable variable, void *value)
{
    trace_info("[NPP] {zilch} %s npp=%p, variable=%s, value=%p\n", __func__,
               npp, reverse_npn_variable(variable), value);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP npp, NPFocusDirection direction)
{
    trace_info("[NPP] {zilch} %s npp=%p, direction=%d\n", __func__, npp, direction);
    return 1;
}

void
NPP_LostFocus(NPP npp)
{
    trace_info("[NPP] {zilch} %s npp=%p\n", __func__, npp);
    return;
}

void
NPP_URLRedirectNotify(NPP npp, const char *url, int32_t status, void *notifyData)
{
    trace_info("[NPP] {full} %s npp=%p, url=%s, status=%d, notifyData=%p\n", __func__,
               npp, url, status, notifyData);
    // We are handling redirects ourselves. Tell browser to stop.
    npn.urlredirectresponse(npp, notifyData, false);
    return;
}

NPError
NPP_ClearSiteData(const char *site, uint64_t flags, uint64_t maxAge)
{
    trace_info("[NPP] {zilch} %s site=%s, flags=%"PRIu64", maxAge=%"PRIu64"\n", __func__,
               site, flags, maxAge);
    return NPERR_NO_ERROR;
}

char**
NPP_GetSitesWithData(void)
{
    trace_info("[NPP] {zilch} %s\n", __func__);
    return NULL;
}

void
NPP_DidComposite(NPP npp)
{
    trace_info("[NPP] {zilch} %s npp=%p\n", __func__, npp);
    return;
}
