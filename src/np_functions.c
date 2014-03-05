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
#include "interface_list.h"
#include "tables.h"
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/pp_errors.h>
#include "globals.h"

#define NO_IMPL assert(0 && "no implementation yet")

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

    if (!obligatory_npp_instance)
        obligatory_npp_instance = instance;

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

    ul->headers = strdup(stream->headers ? stream->headers : "");

    pp_resource_release(loader);
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
    ul->loaded = 1;
    pp_resource_release(loader);
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

    if (offset + len > ul->body_allocated) {
        size_t nsize = ul->body_allocated + (ul->body_allocated >> 1);
        if (offset + len > nsize)
            nsize = offset + len;
        ul->body = realloc(ul->body, nsize);
        ul->body_allocated = nsize;
    }

    memcpy(ul->body + offset, buffer, len);
    ul->body_size += len;

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
handle_GraphicsExpose_event(NPP instance, void *event)
{
    XGraphicsExposeEvent *ev = event;
    struct pp_instance_s *pp_i = instance->pdata;
    if (!pp_i->draw_in_progress)
        return 0;
    struct pp_graphics2d_s *g2d = pp_resource_acquire(pp_i->graphics, PP_RESOURCE_GRAPHICS2D);
    if (!g2d)
        return 0;
    Display *dpy = ev->display;
    Drawable drawable = ev->drawable;
    int screen = 0;
    memset(g2d->data, 0x76, g2d->width * g2d->height);
    XImage *xi = XCreateImage(dpy, DefaultVisual(dpy, screen), 24, ZPixmap, 0, g2d->data,
                              g2d->width, g2d->height, 32, g2d->stride);

    XPutImage(dpy, drawable, DefaultGC(dpy, screen), xi, 0, 0, 0, 0, g2d->width, g2d->height);

    free(xi);
    pp_resource_release(pp_i->graphics);

    pp_i->draw_in_progress = 0;
    pp_i->draw_completion_callback.func(pp_i->draw_completion_callback.user_data, PP_OK);

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
        return handle_GraphicsExpose_event(instance, event);
        break;
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
        NO_IMPL; break;
    case NPPVpluginDescriptionString:
        NO_IMPL; break;
    case NPPVpluginWindowBool:
        NO_IMPL; break;
    case NPPVpluginTransparentBool:
        NO_IMPL; break;
    case NPPVjavaClass:
        NO_IMPL; break;
    case NPPVpluginWindowSize:
        NO_IMPL; break;
    case NPPVpluginTimerInterval:
        NO_IMPL; break;
    case NPPVpluginScriptableInstance:
        NO_IMPL; break;
    case NPPVpluginScriptableIID:
        NO_IMPL; break;
    case NPPVjavascriptPushCallerBool:
        NO_IMPL; break;
    case NPPVpluginKeepLibraryInMemory:
        NO_IMPL; break;
    case NPPVpluginNeedsXEmbed:
        *(int *)value = 1;
        break;
    case NPPVpluginScriptableNPObject:
        *(void **)value = NULL;
        break;
    case NPPVformValue:
        NO_IMPL; break;
    case NPPVpluginUrlRequestsDisplayedBool:
        NO_IMPL; break;
    case NPPVpluginWantsAllNetworkStreams:
        NO_IMPL; break;
    case NPPVpluginNativeAccessibleAtkPlugId:
        NO_IMPL; break;
    case NPPVpluginCancelSrcStream:
        NO_IMPL; break;
    case NPPVsupportsAdvancedKeyHandling:
        NO_IMPL; break;
    case NPPVpluginUsesDOMForCursorBool:
        NO_IMPL; break;
    case NPPVpluginDrawingModel:
        NO_IMPL; break;
    default:
        NO_IMPL; break;
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
