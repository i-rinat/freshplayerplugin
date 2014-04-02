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
#include <npapi.h>
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
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char *argn[],
        char *argv[], NPSavedData *saved)
{
    // TODO: mode parameter handling
    int k;
    struct pp_instance_s *pp_i;
    trace_info("[NPP] {part} %s pluginType=%s instance=%p, mode=%d, argc=%d, saved=%p\n", __func__,
               pluginType, instance, mode, argc, saved);

    tables_add_npp_instance(instance);

    for (k = 0; k < argc; k ++)
        trace_info("            argn[%d] = %s, argv[%d] = %s\n", k, argn[k], k, argv[k]);

    pp_i = calloc(sizeof(*pp_i), 1);
    instance->pdata = pp_i;
    if (!pp_i)
        return NPERR_OUT_OF_MEMORY_ERROR;

    pp_i->npp = instance;
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
    npn.setvalue(instance, NPPVpluginWindowBool, (void*)0);

    pp_i->ppp_instance_1_1->DidCreate(pp_i->pp_instance_id, pp_i->argc, pp_i->argn, pp_i->argv);
    pp_i->instance_loaded = 1;

    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData **save)
{
    trace_info("[NPP] {full} %s instance=%p, save=%p\n", __func__, instance, save);
    struct pp_instance_s *pp_i = instance->pdata;
    pp_i->ppp_instance_1_1->DidDestroy(pp_i->pp_instance_id);
    tables_remove_npp_instance(instance);
    if (save)
        *save = NULL;
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow *window)
{
    char *window_str = trace_np_window_as_string(window);
    trace_info("[NPP] {part} %s instance=%p, window=%s\n", __func__, instance, window_str);
    free(window_str);

    struct pp_instance_s *pp_i = instance->pdata;
    pp_i->wnd = (Window)window->window;
    pp_i->x = window->x;
    pp_i->y = window->y;
    pp_i->width = window->width;
    pp_i->height = window->height;
    pp_i->clip_rect = window->clipRect;
    pp_i->ws_info = window->ws_info;
    pp_i->window_type = window->type;

    PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i->pp_instance_id);
    struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
    v->rect.point.x = window->x;
    v->rect.point.y = window->y;
    v->rect.size.width = window->width;
    v->rect.size.height = window->height;
    // ignoring clipRect, will use full rect instead
    pp_resource_release(view);

    if (pp_i->instance_loaded)
        pp_i->ppp_instance_1_1->DidChangeView(pp_i->pp_instance_id, view);

    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable, uint16_t *stype)
{
    trace_info("[NPP] {part} %s instance=%p, type=%s, stream={.pdata=%p, .ndata=%p, .url=%s, "
               "end=%u, lastmodified=%u, .headers=%s}, seekable=%d\n", __func__, instance, type,
               stream->pdata, stream->ndata, stream->url, stream->end, stream->lastmodified,
               stream->headers, seekable);

    PP_Resource loader = tables_pop_url_pair(stream->url);
    if (!loader) {
        // ignoring unrequested streams
        stream->pdata = NULL;
        trace_info("      ignoring unrequested stream\n");
        return NPERR_NO_ERROR;
    }

    stream->pdata = (void*)(size_t)loader;
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);

    if (ul) {
        ul->headers = strdup(stream->headers ? stream->headers : "");
        pp_resource_release(loader);
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream *stream, NPReason reason)
{
    trace_info("[NPP] {part} %s instance=%p, stream=%p, reason=%d\n", __func__,
               instance, stream, reason);

    PP_Resource loader = (PP_Resource)(size_t)stream->pdata;
    if (!loader)
        return NPERR_NO_ERROR;

    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (ul) {
        ul->loaded = 1;
        if (ul->ccb.func) {
            ul->ccb.func(ul->ccb.user_data, PP_OK);
        }
        pp_resource_release(loader);
    }
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream *stream)
{
    trace_info("[NPP] {part} %s instance=%p, stream=%p\n", __func__, instance, stream);
    return 1024*1024;
}

int32_t
NPP_Write(NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{
    trace_info("[NPP] {part} %s instance=%p, stream=%p, offset=%d, len=%d, buffer=%p\n", __func__,
               instance, stream, offset, len, buffer);

    PP_Resource loader = (PP_Resource)(size_t)stream->pdata;
    if (!loader)
        return len;

    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
            trace_error("%s, ul is NULL\n", __func__);
            return len;
    }

    if (ul->fp) {
        fseek(ul->fp, offset, SEEK_SET);
        fwrite(buffer, len, 1, ul->fp);
    }

    pp_resource_release(loader);
    return len;
}

void
NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
    trace_info("[NPP] {zilch} %s instance=%p, stream=%p, fname=%s\n", __func__,
               instance, stream, fname);
    return;
}

void
NPP_Print(NPP instance, NPPrint *platformPrint)
{
    trace_info("[NPP] {zilch} %s instance=%p, platformPrint=%p\n", __func__,
               instance, platformPrint);
    return;
}

static
int16_t
handle_graphics_expose_event(NPP instance, void *event)
{
    XGraphicsExposeEvent *ev = event;
    struct pp_instance_s *pp_i = instance->pdata;
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
handle_enter_leave_event(NPP instance, void *event)
{
    XCrossingEvent *ev = event;
    struct pp_instance_s *pp_i = instance->pdata;

    // TODO: check if we want this event
    PP_Resource mouse_event;
    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);
    PP_InputEvent_Type event_type = ev->type == EnterNotify ? PP_INPUTEVENT_TYPE_MOUSEENTER
                                                            : PP_INPUTEVENT_TYPE_MOUSELEAVE;

    mouse_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                               ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                               &mouse_position, 0, &zero_point);
    if (pp_i->ppp_input_event)
        pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, mouse_event);

    return 1;
}

static
int16_t
handle_motion_event(NPP instance, void *event)
{
    XMotionEvent *ev = event;
    struct pp_instance_s *pp_i = instance->pdata;

    // TODO: check if we want this event
    PP_Resource mouse_event;
    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);

    mouse_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, PP_INPUTEVENT_TYPE_MOUSEMOVE,
                                               ev->time/1.0e6, mod, PP_INPUTEVENT_MOUSEBUTTON_NONE,
                                               &mouse_position, 0, &zero_point);

    if (pp_i->ppp_input_event)
        pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, mouse_event);

    return 1;
}

static
int16_t
handle_button_press_release_event(NPP instance, void *event)
{
    XButtonEvent *ev = event;
    struct pp_instance_s *pp_i = instance->pdata;

    // TODO: check if we want this event
    PP_InputEvent_MouseButton mouse_button = PP_INPUTEVENT_MOUSEBUTTON_NONE;
    PP_Resource mouse_event;
    struct PP_Point mouse_position = {.x = ev->x, .y = ev->y};
    struct PP_Point zero_point = {.x = 0, .y = 0};
    unsigned int mod = x_state_mask_to_pp_inputevent_modifier(ev->state);

    switch (ev->button) {
    case 1:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_LEFT;
        break;
    case 2:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_MIDDLE;
        break;
    case 3:
        mouse_button = PP_INPUTEVENT_MOUSEBUTTON_RIGHT;
        break;

    case 4: // up
    case 5: // down
    case 6: // left
    case 7: // right
        // TODO: wheel
        break;
    }

    PP_InputEvent_Type event_type = ev->type == ButtonPress ? PP_INPUTEVENT_TYPE_MOUSEDOWN
                                                            : PP_INPUTEVENT_TYPE_MOUSEUP;
    mouse_event = ppb_mouse_input_event_create(pp_i->pp_instance_id, event_type,
                                               ev->time/1.0e6, mod, mouse_button,
                                               &mouse_position, 1, &zero_point);
    if (pp_i->ppp_input_event)
        pp_i->ppp_input_event->HandleInputEvent(pp_i->pp_instance_id, mouse_event);

    return 1;
}

int16_t
NPP_HandleEvent(NPP instance, void *event)
{
    XAnyEvent *xaev = event;
    trace_info("[NPP] {part} %s instance=%p, event={.type=%s, .serial=%lu, .send_event=%d, "
               ".display=%p, .window=0x%x}\n", __func__, instance, reverse_xevent_type(xaev->type),
               xaev->serial, xaev->send_event, xaev->display, (int32_t)xaev->window);

    switch (xaev->type) {
    case GraphicsExpose:
        return handle_graphics_expose_event(instance, event);
        break;
    case EnterNotify:
        // fall through
    case LeaveNotify:
        return handle_enter_leave_event(instance, event);
    case MotionNotify:
        return handle_motion_event(instance, event);
    case ButtonPress:
        // fall through
    case ButtonRelease:
        return handle_button_press_release_event(instance, event);
    default:
        trace_warning("%s, event %s not handled\n", __func__, reverse_xevent_type(xaev->type));
        break;
    }

    return 0;
}

void
NPP_URLNotify(NPP instance, const char *url, NPReason reason, void *notifyData)
{
    trace_info("[NPP] {zilch} %s instance=%p, url=%s, reason=%d, notifyData=%p\n", __func__,
               instance, url, reason, notifyData);
    return;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    trace_info("[NPP] {part} %s instance=%p, variable=%s\n", __func__,
               instance, reverse_npp_variable(variable));
    switch (variable) {
    case NPPVpluginNameString:
        trace_info("[NPP] {zilch} NPPVpluginNameString not implemented\n");
        break;
    case NPPVpluginDescriptionString:
        trace_info("[NPP] {zilch} NPPVpluginDescriptionString not implemented\n");
        break;
    case NPPVpluginWindowBool:
        trace_info("[NPP] {zilch} NPPVpluginWindowBool not implemented\n");
        break;
    case NPPVpluginTransparentBool:
        trace_info("[NPP] {zilch} NPPVpluginTransparentBool not implemented\n");
        break;
    case NPPVjavaClass:
        trace_info("[NPP] {zilch} NPPVjavaClass not implemented\n");
        break;
    case NPPVpluginWindowSize:
        trace_info("[NPP] {zilch} NPPVpluginWindowSize not implemented\n");
        break;
    case NPPVpluginTimerInterval:
        trace_info("[NPP] {zilch} NPPVpluginTimerInterval not implemented\n");
        break;
    case NPPVpluginScriptableInstance:
        trace_info("[NPP] {zilch} NPPVpluginScriptableInstance not implemented\n");
        break;
    case NPPVpluginScriptableIID:
        trace_info("[NPP] {zilch} NPPVpluginScriptableIID not implemented\n");
        break;
    case NPPVjavascriptPushCallerBool:
        trace_info("[NPP] {zilch} NPPVjavascriptPushCallerBool not implemented\n");
        break;
    case NPPVpluginKeepLibraryInMemory:
        trace_info("[NPP] {zilch} NPPVpluginKeepLibraryInMemory not implemented\n");
        break;
    case NPPVpluginNeedsXEmbed:
        *(int *)value = 1;
        break;
    case NPPVpluginScriptableNPObject:
        trace_info("[NPP] {part} NPPVpluginScriptableNPObject not implemented\n");
        do {
            struct pp_instance_s *pp_i = instance->pdata;
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
        } while (0);
        break;
    case NPPVformValue:
        trace_info("[NPP] {zilch} NPPVformValue not implemented\n");
        break;
    case NPPVpluginUrlRequestsDisplayedBool:
        trace_info("[NPP] {zilch} NPPVpluginUrlRequestsDisplayedBool not implemented\n");
        break;
    case NPPVpluginWantsAllNetworkStreams:
        trace_info("[NPP] {zilch} NPPVpluginWantsAllNetworkStreams not implemented\n");
        break;
    case NPPVpluginNativeAccessibleAtkPlugId:
        trace_info("[NPP] {zilch} NPPVpluginNativeAccessibleAtkPlugId not implemented\n");
        break;
    case NPPVpluginCancelSrcStream:
        trace_info("[NPP] {zilch} NPPVpluginCancelSrcStream not implemented\n");
        break;
    case NPPVsupportsAdvancedKeyHandling:
        trace_info("[NPP] {zilch} NPPVsupportsAdvancedKeyHandling not implemented\n");
        break;
    case NPPVpluginUsesDOMForCursorBool:
        trace_info("[NPP] {zilch} NPPVpluginUsesDOMForCursorBool not implemented\n");
        break;
    case NPPVpluginDrawingModel:
        trace_info("[NPP] {zilch} NPPVpluginDrawingModel not implemented\n");
        break;
    default:
        trace_info("[NPP] {zilch} UNKNOWN not implemented\n");
        break;
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    trace_info("[NPP] {zilch} %s instance=%p, variable=%s, value=%p\n", __func__,
               instance, reverse_npn_variable(variable), value);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP instance, NPFocusDirection direction)
{
    trace_info("[NPP] {zilch} %s instance=%p, direction=%d\n", __func__, instance, direction);
    return 1;
}

void
NPP_LostFocus(NPP instance)
{
    trace_info("[NPP] {zilch} %s instance=%p\n", __func__, instance);
    return;
}

void
NPP_URLRedirectNotify(NPP instance, const char *url, int32_t status, void *notifyData)
{
    trace_info("[NPP] {zilch} %s instance=%p, url=%s, status=%d, notifyData=%p\n", __func__,
               instance, url, status, notifyData);
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
NPP_DidComposite(NPP instance)
{
    trace_info("[NPP] {zilch} %s instance=%p\n", __func__, instance);
    return;
}
