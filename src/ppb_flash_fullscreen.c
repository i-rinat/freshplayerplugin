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
#include "reverse_constant.h"
#include <X11/Xatom.h>


PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    return pp_i->is_fullscreen;
}


static
void
_update_instance_view_comt(void *p)
{
    PP_Instance instance = (size_t)p;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);

    if (pp_i->instance_loaded) {
        PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i->pp_instance_id);
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

        pp_i->ppp_instance_1_1->DidChangeView(pp_i->pp_instance_id, view);
        ppb_core_release_resource(view);
    }
}

struct thread_param_s {
    PP_Instance         instance;
    pthread_barrier_t   startup_barrier;
};

static
void *
fullscreen_window_thread(void *p)
{
    struct thread_param_s *tp = p;
    Display *dpy = XOpenDisplay(NULL);
    struct pp_instance_s *pp_i = tables_get_pp_instance(tp->instance);

    pp_i->fs_wnd = XCreateSimpleWindow(dpy, XDefaultRootWindow(dpy),
                                       0, 0, pp_i->fs_width, pp_i->fs_height, 0, 0, 0x809080);
    XSelectInput(dpy, pp_i->fs_wnd, KeyPressMask | KeyReleaseMask | ButtonPressMask |
                                    ButtonReleaseMask | PointerMotionMask | ExposureMask);

    // go fullscreen
    Atom netwm_fullscreen_atom = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    Atom netwm_state_atom = XInternAtom(dpy, "_NET_WM_STATE", True);
    XChangeProperty(dpy, pp_i->fs_wnd, netwm_state_atom, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)&netwm_fullscreen_atom, 1);

    // show window
    XMapWindow(dpy, pp_i->fs_wnd);
    XSync(dpy, false);
    pp_i->is_fullscreen = 1;

    pthread_barrier_wait(&tp->startup_barrier);
    pthread_barrier_destroy(&tp->startup_barrier);

    npn.pluginthreadasynccall(pp_i->npp, _update_instance_view_comt, (void*)(size_t)tp->instance);

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
            case GraphicsExpose: {
                // replace display handle
                ev.xgraphicsexpose.display = dpy;
            } break;
        }
        NPP_HandleEvent(pp_i->npp, &ev);
    }

quit_and_destroy_fs_wnd:
    pp_i->is_fullscreen = 0;
    XDestroyWindow(dpy, pp_i->fs_wnd);
    XCloseDisplay(dpy);

    npn.pluginthreadasynccall(pp_i->npp, _update_instance_view_comt, (void*)(size_t)tp->instance);
    free(tp);
    return NULL;
}

PP_Bool
ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    Window browser_wnd;

    if (!!fullscreen == !!pp_i->is_fullscreen)
        return PP_FALSE;

    npn.getvalue(pp_i->npp, NPNVnetscapeWindow, &browser_wnd);

    if (fullscreen) {
        struct thread_param_s *tparams = malloc(sizeof(*tparams));
        tparams->instance = instance;

        pthread_barrier_init(&tparams->startup_barrier, NULL, 2);

        pthread_create(&pp_i->fs_thread, NULL, fullscreen_window_thread, tparams);

        pthread_barrier_wait(&tparams->startup_barrier);
    } else {
        pp_i->is_fullscreen = 0;
        // TODO: remember previous size
        pp_i->width = 400;
        pp_i->height = 300;

        XKeyEvent ev = {
            .type = KeyPress,
            .display = pp_i->dpy,
            .window = pp_i->fs_wnd,
            .keycode = XKeysymToKeycode(pp_i->dpy, XK_Escape)
        };

        XSendEvent(pp_i->dpy, pp_i->fs_wnd, False, 0, (void *)&ev);
        XFlush(pp_i->dpy);
    }

    return PP_TRUE;
}

struct get_fs_param_s {
    PP_Instance         instance;
    struct PP_Size     *size;
    pthread_barrier_t   barrier;
    int                 should_wait;
    int                 retval;
};

PP_Bool
ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size *size)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);

    size->width = pp_i->fs_width;
    size->height = pp_i->fs_height;

    return PP_TRUE;
}

// trace wrappers
static
PP_Bool
trace_ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_is_fullscreen(instance);
}

static
PP_Bool
trace_ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    trace_info("[PPB] {full} %s instance=%d fullscreen=%d\n", __func__+6, instance, fullscreen);
    return ppb_flash_fullscreen_set_fullscreen(instance, fullscreen);
}

static
PP_Bool
trace_ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size *size)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_get_screen_size(instance, size);
}


const struct PPB_FlashFullscreen_1_0 ppb_flash_fullscreen_interface_1_0 = {
    .IsFullscreen =     trace_ppb_flash_fullscreen_is_fullscreen,
    .SetFullscreen =    trace_ppb_flash_fullscreen_set_fullscreen,
    .GetScreenSize =    trace_ppb_flash_fullscreen_get_screen_size,
};
