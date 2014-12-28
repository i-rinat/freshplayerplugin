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

#include "ppb_flash_fullscreen.h"
#include "ppb_core.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/pp_errors.h>
#include "reverse_constant.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>


static volatile gint events_inflight = 0;

struct handle_event_comt_param_s {
    PP_Instance instance_id;
    XEvent      ev;
};

struct thread_param_s {
    struct pp_instance_s   *pp_i;
    pthread_barrier_t       startup_barrier;
    Window                  browser_window;
    pthread_barrier_t       browser_window_barrier;
};


PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }
    return pp_i->is_fullscreen;
}

static
void
update_instance_view_comt(void *user_data, int32_t result)
{
    struct pp_instance_s *pp_i = user_data;

    if (g_atomic_int_get(&pp_i->instance_loaded)) {
        PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i);
        struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
        if (!v) {
            trace_error("%s, resource allocation failure\n", __func__);
            return;
        }

        v->rect.point.x = 0;
        v->rect.point.y = 0;
        if (pp_i->is_fullscreen) {
            v->rect.size.width = pp_i->fs_width;
            v->rect.size.height = pp_i->fs_height;
        } else {
            v->rect.size.width = pp_i->width;
            v->rect.size.height = pp_i->height;
        }
        pp_resource_release(view);

        pp_i->ppp_instance_1_1->DidChangeView(pp_i->id, view);
        ppb_core_release_resource(view);
    }
}

static
void
handle_event_ptac(void *p)
{
    struct handle_event_comt_param_s *params = p;
    struct pp_instance_s *pp_i = tables_get_pp_instance(params->instance_id);
    if (pp_i) {
        NPP_HandleEvent(pp_i->npp, &params->ev);
    }
    g_slice_free(struct handle_event_comt_param_s, params);
    g_atomic_int_add(&events_inflight, -1);
}

static
void
get_browser_window(void *p)
{
    struct thread_param_s *tp = p;

    if (npn.getvalue(tp->pp_i->npp, NPNVnetscapeWindow, &tp->browser_window) != NPERR_NO_ERROR)
        tp->browser_window = None;

    pthread_barrier_wait(&tp->browser_window_barrier);
}

static
void *
fullscreen_window_thread(void *p)
{
    struct thread_param_s *tp = p;
    Display *dpy = XOpenDisplay(NULL);
    struct pp_instance_s *pp_i = tp->pp_i;
    int             px, py;
    Window          root, child;
    int             rel_x, rel_y;
    unsigned int    mask;

    // get current mouse pointer position
    XQueryPointer(dpy, DefaultRootWindow(dpy), &root, &child, &px, &py, &rel_x, &rel_y, &mask);

    // create tiny window where mouse pointer is
    pp_i->fs_wnd = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy),
                                       px, py, 3, 3, 0, 0, 0x000000);
    XSelectInput(dpy, pp_i->fs_wnd, KeyPressMask | KeyReleaseMask | ButtonPressMask |
                                    ButtonReleaseMask | PointerMotionMask | ExposureMask |
                                    StructureNotifyMask);

    // round trip to browser thread to get browser window
    tp->browser_window = None;
    pthread_barrier_init(&tp->browser_window_barrier, NULL, 2);
    ppb_core_call_on_browser_thread(get_browser_window, tp);
    pthread_barrier_wait(&tp->browser_window_barrier);
    pthread_barrier_destroy(&tp->browser_window_barrier);

    // set window transient for browser window
    if (tp->browser_window != None)
        XSetTransientForHint(dpy, pp_i->fs_wnd, tp->browser_window);
    else
        trace_error("%s, can't get tp->browser_window\n", __func__);

    // go fullscreen
    Atom netwm_state_atom = XInternAtom(dpy, "_NET_WM_STATE", False);
    Atom netwm_fullscreen_atom = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XChangeProperty(dpy, pp_i->fs_wnd,
                    netwm_state_atom,
                    XA_ATOM,
                    32, PropModeReplace,
                    (unsigned char *)&netwm_fullscreen_atom, 1);

    // do not appear in pager
    Atom netwm_skip_pager_atom = XInternAtom(dpy, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(dpy, pp_i->fs_wnd,
                    netwm_state_atom,
                    XA_ATOM,
                    32, PropModeAppend,
                    (unsigned char *)&netwm_skip_pager_atom, 1);

    // give window a name
    const char *fs_window_name = "freshwrapper fullscreen window";
    XChangeProperty(dpy, pp_i->fs_wnd,
                    XInternAtom(dpy, "WM_NAME", False),
                    XInternAtom(dpy, "STRING", False),
                    8, PropModeReplace,
                    (unsigned char *)fs_window_name, strlen(fs_window_name));

    XChangeProperty(dpy, pp_i->fs_wnd,
                    XInternAtom(dpy, "_NET_WM_NAME", False),
                    XInternAtom(dpy, "UTF8_STRING", False),
                    8, PropModeReplace,
                    (unsigned char *)fs_window_name, strlen(fs_window_name));

    // show window
    XMapWindow(dpy, pp_i->fs_wnd);
    XSync(dpy, False);
    pthread_mutex_lock(&display.lock);
    pp_i->is_fullscreen = 1;
    pthread_mutex_unlock(&display.lock);

    pthread_barrier_wait(&tp->startup_barrier);

    while (1) {
        XEvent  ev;
        int     handled = 0;
        KeySym  keysym;

        XNextEvent(dpy, &ev);
        switch (ev.type) {
            case KeyPress:
                keysym = XLookupKeysym(&ev.xkey, 0);
                if (XK_Escape == keysym)
                    goto quit_and_destroy_fs_wnd;
                break;

            case ConfigureNotify:
                pthread_mutex_lock(&display.lock);
                pp_i->fs_width = ev.xconfigure.width;
                pp_i->fs_height = ev.xconfigure.height;
                pthread_mutex_unlock(&display.lock);
                ppb_core_call_on_main_thread(0, PP_MakeCCB(update_instance_view_comt, pp_i),
                                             PP_OK);
                handled = 1;
                break;

            case ReparentNotify:
            case MapNotify:
                handled = 1;
                break;
        }
        ev.xany.display = display.x;

        if (!handled) {
            struct handle_event_comt_param_s *params = g_slice_alloc(sizeof(*params));
            params->instance_id =   pp_i->id;
            params->ev =            ev;
            g_atomic_int_add(&events_inflight, 1);
            ppb_core_call_on_browser_thread(handle_event_ptac, params);
        }
    }

quit_and_destroy_fs_wnd:

    // wait for all events to be processed
    while (g_atomic_int_get(&events_inflight)) {
        usleep(10);
    }

    pp_i->is_fullscreen = 0;
    XDestroyWindow(dpy, pp_i->fs_wnd);
    XCloseDisplay(dpy);

    ppb_core_call_on_main_thread(0, PP_MakeCCB(update_instance_view_comt, pp_i), PP_OK);
    g_slice_free(struct thread_param_s, tp);
    return NULL;
}

PP_Bool
ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    pthread_mutex_lock(&display.lock);
    int in_same_state = (!!fullscreen == !!pp_i->is_fullscreen);
    pthread_mutex_unlock(&display.lock);
    if (in_same_state)
        return PP_FALSE;

    if (fullscreen) {
        struct thread_param_s *tparams = g_slice_alloc(sizeof(*tparams));
        tparams->pp_i = pp_i;

        pthread_barrier_init(&tparams->startup_barrier, NULL, 2);
        pthread_create(&pp_i->fs_thread, NULL, fullscreen_window_thread, tparams);
        pthread_detach(pp_i->fs_thread);
        pthread_barrier_wait(&tparams->startup_barrier);
        pthread_barrier_destroy(&tparams->startup_barrier);
    } else {
        pthread_mutex_lock(&display.lock);
        pp_i->is_fullscreen = 0;
        XKeyEvent ev = {
            .type = KeyPress,
            .display = display.x,
            .window = pp_i->fs_wnd,
            .keycode = XKeysymToKeycode(display.x, XK_Escape)
        };

        XSendEvent(display.x, pp_i->fs_wnd, False, 0, (void *)&ev);
        XFlush(display.x);
        pthread_mutex_unlock(&display.lock);
    }

    return PP_TRUE;
}

PP_Bool
ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size *size)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    size->width = display.fs_width;
    size->height = display.fs_height;

    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_is_fullscreen(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    trace_info("[PPB] {full} %s instance=%d fullscreen=%d\n", __func__+6, instance, fullscreen);
    return ppb_flash_fullscreen_set_fullscreen(instance, fullscreen);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size *size)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_get_screen_size(instance, size);
}


const struct PPB_FlashFullscreen_1_0 ppb_flash_fullscreen_interface_1_0 = {
    .IsFullscreen =     TWRAPF(ppb_flash_fullscreen_is_fullscreen),
    .SetFullscreen =    TWRAPF(ppb_flash_fullscreen_set_fullscreen),
    .GetScreenSize =    TWRAPF(ppb_flash_fullscreen_get_screen_size),
};
