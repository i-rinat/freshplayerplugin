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

#include <assert.h>
#include <inttypes.h>
#include <npapi/npapi.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
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
#include "ppb_var.h"
#include "ppb_core.h"
#include "header_parser.h"


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
    pp_i->dpy = XOpenDisplay(NULL);
    if (!pp_i->dpy) {
        trace_error("%s, can't open X Display\n", __func__);
    }
    pp_i->egl_dpy = eglGetDisplay(pp_i->dpy);
    if (!pp_i->egl_dpy) {
        trace_error("%s, can't open EGL display\n", __func__);
    }

    {
        EGLint v_major, v_minor;
        if (!eglInitialize(pp_i->egl_dpy, &v_major, &v_minor)) {
            trace_error("%s, can't initialize EGL\n", __func__);
        }
        trace_info("EGL version %d.%d\n", v_major, v_minor);
    }

    // request windowless operation
    npn.setvalue(npp, NPPVpluginWindowBool, (void*)0);

    pp_i->ppp_instance_1_1->DidCreate(pp_i->pp_instance_id, pp_i->argc, pp_i->argn, pp_i->argv);
    pp_i->instance_loaded = 1;

    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP npp, NPSavedData **save)
{
    trace_info("[NPP] {full} %s npp=%p, save=%p\n", __func__, npp, save);
    struct pp_instance_s *pp_i = npp->pdata;
    pp_i->ppp_instance_1_1->DidDestroy(pp_i->pp_instance_id);
    tables_remove_npp_instance(npp);
    if (save)
        *save = NULL;
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP npp, NPWindow *window)
{
    char *window_str = trace_np_window_as_string(window);
    trace_info("[NPP] {full} %s npp=%p, window=%s\n", __func__, npp, window_str);
    free(window_str);

    struct pp_instance_s *pp_i = npp->pdata;

    if (pp_i && !pp_i->is_fullscreen) {
        pp_i->wnd = (Window)window->window;
        pp_i->width = window->width;
        pp_i->height = window->height;

        PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i->pp_instance_id);
        struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
        v->rect.point.x = window->x;
        v->rect.point.y = window->y;
        v->rect.size.width = window->width;
        v->rect.size.height = window->height;
        pp_resource_release(view);

        if (pp_i->instance_loaded)
            pp_i->ppp_instance_1_1->DidChangeView(pp_i->pp_instance_id, view);
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
        free(rt);
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
        free(rt);
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
    if (!g2d)
        return 0;
    Display *dpy = ev->display;
    Drawable drawable = ev->drawable;
    int screen = 0;
    pthread_mutex_lock(&g2d->lock);
    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, screen), 24, ZPixmap, 0, g2d->second_buffer,
                              g2d->width, g2d->height, 32, g2d->stride);

    XPutImage(dpy, drawable, DefaultGC(dpy, screen), xi, 0, 0, 0, 0, g2d->width, g2d->height);
    free(xi);
    pthread_mutex_unlock(&g2d->lock);
    pp_resource_release(pp_i->graphics);

    return 1;
}

static
unsigned int
x_state_mask_to_pp_inputevent_modifier(unsigned int state)
{
    unsigned int mod = 0;

    // TODO: refine this
    if (state & ShiftMask)      mod |= PP_INPUTEVENT_MODIFIER_SHIFTKEY;
    if (state & LockMask)       mod |= PP_INPUTEVENT_MODIFIER_CAPSLOCKKEY;
    if (state & ControlMask)    mod |= PP_INPUTEVENT_MODIFIER_CONTROLKEY;
    if (state & Mod1Mask)       mod |= PP_INPUTEVENT_MODIFIER_ALTKEY;
    if (state & Mod2Mask)       mod |= PP_INPUTEVENT_MODIFIER_NUMLOCKKEY;
    if (state & Mod4Mask)       mod |= PP_INPUTEVENT_MODIFIER_METAKEY;

    if (state & Button1Mask)    mod |= PP_INPUTEVENT_MODIFIER_LEFTBUTTONDOWN;
    if (state & Button2Mask)    mod |= PP_INPUTEVENT_MODIFIER_MIDDLEBUTTONDOWN;
    if (state & Button3Mask)    mod |= PP_INPUTEVENT_MODIFIER_RIGHTBUTTONDOWN;

    mod |= PP_INPUTEVENT_MODIFIER_ISLEFT;

    return mod;
}

static
int16_t
handle_enter_leave_event(NPP npp, void *event)
{
    XCrossingEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    PP_Resource pp_event;
    PP_Bool ret;

    if ((PP_INPUTEVENT_CLASS_MOUSE & pp_i->event_mask) ||
        (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
    {
        struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
        struct PP_Point zero_point = {.x = 0, .y = 0};
        unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
        PP_InputEvent_Type event_type = (ev->type == EnterNotify) ? PP_INPUTEVENT_TYPE_MOUSEENTER
                                                                  : PP_INPUTEVENT_TYPE_MOUSELEAVE;

        pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                                ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                                &mouse_position, 0, &zero_point);
        if (pp_i->ppp_input_event)
            ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);

        if (ret == PP_FALSE && (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
            return 0;

        return 1;
    }

    return 0;
}

static
int16_t
handle_motion_event(NPP npp, void *event)
{
    XMotionEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    PP_Resource pp_event;
    PP_Bool ret;

    if ((PP_INPUTEVENT_CLASS_MOUSE & pp_i->event_mask) ||
        (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
    {
        struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
        struct PP_Point zero_point = {.x = 0, .y = 0};
        unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);

        pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, PP_INPUTEVENT_TYPE_MOUSEMOVE,
                                                ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                                &mouse_position, 0, &zero_point);
        if (pp_i->ppp_input_event)
            ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);

        if (ret == PP_FALSE && (PP_INPUTEVENT_CLASS_MOUSE & pp_i->filtered_event_mask))
            return 0;

        return 1;
    }

    return 0;
}

static
int16_t
handle_button_press_release_event(NPP npp, void *event)
{
    XButtonEvent *ev = event;
    struct pp_instance_s *pp_i = npp->pdata;
    uint32_t event_class = 0;
    PP_Resource pp_event = 0;
    PP_Bool ret;
    PP_InputEvent_MouseButton mouse_button = PP_INPUTEVENT_MOUSEBUTTON_NONE;
    const float scroll_by_tick = 10.0;

    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
    float wheel_x = 0.0, wheel_y = 0.0;

    switch (ev->button) {
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

    if ((event_class & pp_i->event_mask) ||
        (event_class & pp_i->filtered_event_mask))
    {
        if (event_class == PP_INPUTEVENT_CLASS_MOUSE) {
            PP_InputEvent_Type event_type;
            event_type = (ev->type == ButtonPress) ? PP_INPUTEVENT_TYPE_MOUSEDOWN
                                                   : PP_INPUTEVENT_TYPE_MOUSEUP;
            pp_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                                    ev->time/1.0e6, mod, mouse_button,
                                                    &mouse_position, 1, &zero_point);
        } else { // event_class == PP_INPUTEVENT_CLASS_WHEEL
            struct PP_FloatPoint wheel_delta = { .x = wheel_x * scroll_by_tick,
                                                 .y = wheel_y * scroll_by_tick };
            struct PP_FloatPoint wheel_ticks = { .x = wheel_x, .y = wheel_y };

            // pp_event = ppb_wheel_input_event_create(
            //                 pp_i->pp_instance_id, ppb_core_get_time_ticks(), mod,
            //                 &wheel_delta, &wheel_ticks, PP_FALSE);
            // TODO: figure out why this doesn't work
            (void)wheel_delta;
            (void)wheel_ticks;
            return 0;
        }

        if (pp_i->ppp_input_event && pp_event)
            ret = pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, pp_event);

        // return false only of handler returned PP_FALSE and event is filtered
        if (ret == PP_FALSE && (pp_i->filtered_event_mask & event_class))
            return 0;

        return 1;
    }

    return 0;
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
