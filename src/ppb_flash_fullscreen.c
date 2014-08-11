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
_update_instance_view_comt(void *user_data, int32_t result)
{
    struct pp_instance_s *pp_i = user_data;

    if (g_atomic_int_get(&pp_i->instance_loaded)) {
        PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i);
        struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
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
_handle_event_ptac(void *p)
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
void *
fullscreen_window_thread(void *p)
{
    struct thread_param_s *tp = p;
    Display *dpy = XOpenDisplay(NULL);
    struct pp_instance_s *pp_i = tp->pp_i;

    pp_i->fs_wnd = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy),
                                       0, 0, pp_i->fs_width, pp_i->fs_height, 0, 0, 0x809080);
    XSelectInput(dpy, pp_i->fs_wnd, KeyPressMask | KeyReleaseMask | ButtonPressMask |
                                    ButtonReleaseMask | PointerMotionMask | ExposureMask);

    // go fullscreen
    Atom netwm_fullscreen_atom = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    XChangeProperty(dpy, pp_i->fs_wnd,
                    XInternAtom(dpy, "_NET_WM_STATE", False),
                    XA_ATOM,
                    32, PropModeReplace,
                    (unsigned char *)&netwm_fullscreen_atom, 1);

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
    pp_i->is_fullscreen = 1;

    pthread_barrier_wait(&tp->startup_barrier);
    pthread_barrier_destroy(&tp->startup_barrier);

    ppb_core_call_on_main_thread(0, PP_MakeCompletionCallback(_update_instance_view_comt, pp_i),
                                 PP_OK);
    while (1) {
        XEvent ev;
        XNextEvent(dpy, &ev);
        switch (ev.type) {
            case KeyPress: {
                KeySym keysym = XLookupKeysym(&ev.xkey, 0);
                if (XK_Escape == keysym) {
                    goto quit_and_destroy_fs_wnd;
                }
            } break;
        }
        ev.xany.display = pp_i->dpy;

        struct handle_event_comt_param_s *params = g_slice_alloc(sizeof(*params));
        params->instance_id =   pp_i->id;
        params->ev =            ev;
        g_atomic_int_add(&events_inflight, 1);
        ppb_core_call_on_browser_thread(_handle_event_ptac, params);
    }

quit_and_destroy_fs_wnd:

    // wait for all events to be processed
    while (g_atomic_int_get(&events_inflight)) {
        usleep(10);
    }

    pp_i->is_fullscreen = 0;
    XDestroyWindow(dpy, pp_i->fs_wnd);
    XCloseDisplay(dpy);

    ppb_core_call_on_main_thread(0, PP_MakeCompletionCallback(_update_instance_view_comt, pp_i),
                                 PP_OK);
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

    if (!!fullscreen == !!pp_i->is_fullscreen)
        return PP_FALSE;

    if (fullscreen) {
        struct thread_param_s *tparams = g_slice_alloc(sizeof(*tparams));
        tparams->pp_i = pp_i;

        pthread_barrier_init(&tparams->startup_barrier, NULL, 2);
        pthread_create(&pp_i->fs_thread, NULL, fullscreen_window_thread, tparams);
        pthread_barrier_wait(&tparams->startup_barrier);
    } else {
        pthread_mutex_lock(&pp_i->lock);
        pp_i->is_fullscreen = 0;
        XKeyEvent ev = {
            .type = KeyPress,
            .display = pp_i->dpy,
            .window = pp_i->fs_wnd,
            .keycode = XKeysymToKeycode(pp_i->dpy, XK_Escape)
        };

        XSendEvent(pp_i->dpy, pp_i->fs_wnd, False, 0, (void *)&ev);
        XFlush(pp_i->dpy);
        pthread_mutex_unlock(&pp_i->lock);
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

    size->width = pp_i->fs_width;
    size->height = pp_i->fs_height;

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
