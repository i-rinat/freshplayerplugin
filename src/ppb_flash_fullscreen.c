/*
 * Copyright © 2013-2015  Rinat Ibragimov
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
#include <pthread.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "pp_resource.h"
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/pp_errors.h>
#include "reverse_constant.h"
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "pp_interface.h"
#include <drm.h>
#include <sys/ioctl.h>


static volatile gint        currently_fullscreen = 0;
static GAsyncQueue         *fullscreen_transition_queue = NULL;
static volatile gint        run_fullscreen_thread = 1;
static volatile gint        run_delay_thread = 1;
static pthread_barrier_t    cross_thread_call_barrier;
static Atom                 freshwrappercommand_atom;

struct handle_event_comt_param_s {
    PP_Instance instance_id;
    XEvent      ev;
};

struct thread_param_s {
    struct pp_instance_s   *pp_i;
    Window                  browser_window;
};

enum {
    FSCMD_UNDEFINED = 0,
    FSCMD_FULLSCREEN_WAIT_IS_OVER,
    FSCMD_VSYNC,
};


static
void
__attribute__((destructor))
destructor_ppb_flash_fullscreen(void)
{
    if (fullscreen_transition_queue)
        g_async_queue_unref(fullscreen_transition_queue);
    g_atomic_int_set(&run_fullscreen_thread, 0);
}

PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    pthread_mutex_lock(&display.lock);
    uint32_t is_fullscreen = pp_i->is_fullscreen_apparent;
    pthread_mutex_unlock(&display.lock);

    return is_fullscreen;
}

static
void
call_did_change_view_comt(void *user_data, int32_t result)
{
    PP_Instance instance = GPOINTER_TO_INT(user_data);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    const int is_fullscreen = result;

    // check if instance is still alive
    if (!pp_i)
        goto done;

    if (!g_atomic_int_get(&pp_i->instance_loaded)) {
        goto done;
    }

    PP_Resource view = pp_resource_allocate(PP_RESOURCE_VIEW, pp_i);
    struct pp_view_s *v = pp_resource_acquire(view, PP_RESOURCE_VIEW);
    if (!v) {
        trace_error("%s, resource allocation failure\n", __func__);
        goto done;
    }

    // looks like transition is over, so current window geometry can be safely used as
    // full screen size
    pp_i->fs_width = pp_i->fs_width_current;
    pp_i->fs_height = pp_i->fs_height_current;

    v->rect.point.x = 0;
    v->rect.point.y = 0;
    if (is_fullscreen) {
        v->rect.size.width = pp_i->fs_width / config.device_scale;
        v->rect.size.height = pp_i->fs_height / config.device_scale;
    } else {
        v->rect.size.width = pp_i->width / config.device_scale;
        v->rect.size.height = pp_i->height / config.device_scale;
    }
    pp_resource_release(view);

    pthread_mutex_lock(&display.lock);
    pp_i->is_fullscreen_apparent = is_fullscreen;
    pthread_mutex_unlock(&display.lock);

    pp_i->ppp_instance_1_1->DidChangeView(pp_i->id, view);
    ppb_core_release_resource(view);

done:
    pthread_barrier_wait(&cross_thread_call_barrier);
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
    pthread_barrier_wait(&cross_thread_call_barrier);
}

static
void
get_browser_window(void *p)
{
    struct thread_param_s *tp = p;

    if (npn.getvalue(tp->pp_i->npp, NPNVnetscapeWindow, &tp->browser_window) != NPERR_NO_ERROR)
        tp->browser_window = None;

    pthread_barrier_wait(&cross_thread_call_barrier);
}

static
void
append_wm_protocol(Display *dpy, Window wnd, Atom a)
{
    Atom   *protocols = NULL;
    int     n = 0;
    XGetWMProtocols(dpy, wnd, &protocols, &n);

    // check if the atom was already set
    for (int k = 0; k < n; k ++)
        if (protocols[k] == a)
            goto done;

    // append to the end
    Atom *p2 = malloc(sizeof(Atom) * (n + 1));
    if (!p2)
        goto done;

    memcpy(p2, protocols, sizeof(Atom) * n);
    p2[n] = a;
    XSetWMProtocols(dpy, wnd, p2, n + 1);
    free(p2);

done:
    XFree(protocols);
}

static
void
call_did_change_view(PP_Instance instance_id, int is_fullscreen)
{
    ppb_core_call_on_main_thread2(0, PP_MakeCCB(call_did_change_view_comt,
                                                GINT_TO_POINTER(instance_id)), is_fullscreen,
                                                __func__);
    pthread_barrier_wait(&cross_thread_call_barrier);
}

static
void
craft_graphicsexpose_event(XEvent *ev, Display *dpy, struct pp_instance_s *pp_i)
{
    memset(ev, 0, sizeof(*ev));
    ev->xgraphicsexpose.type =     GraphicsExpose;
    ev->xgraphicsexpose.display =  dpy;
    ev->xgraphicsexpose.drawable = pp_i->fs_wnd;
    ev->xgraphicsexpose.width =    pp_i->fs_width;
    ev->xgraphicsexpose.height =   pp_i->fs_height;
}

static
void
fullscreen_window_thread_int(Display *dpy, struct thread_param_s *tp)
{
    struct pp_instance_s *pp_i = tp->pp_i;
    int             px, py;
    Window          root, child;
    int             rel_x, rel_y;
    unsigned int    mask;
    const int       wnd_size = 10;
    const int       screen = DefaultScreen(dpy);

    trace_info_f("%s started\n", __func__);

    // get current mouse pointer position
    XQueryPointer(dpy, DefaultRootWindow(dpy), &root, &child, &px, &py, &rel_x, &rel_y, &mask);

    // create tiny window where mouse pointer is
    XSetWindowAttributes attrs = {
        .background_pixel = 0x000000,
        .backing_store =    Always,
    };
    pp_i->fs_wnd = XCreateWindow(dpy, DefaultRootWindow(dpy),
                                 px - wnd_size / 2, py - wnd_size / 2, wnd_size, wnd_size,
                                 0, DefaultDepth(dpy, screen), InputOutput,
                                 DefaultVisual(dpy, screen), CWBackPixel | CWBackingStore, &attrs);

    XSelectInput(dpy, pp_i->fs_wnd, KeyPressMask | KeyReleaseMask | ButtonPressMask |
                                    ButtonReleaseMask | PointerMotionMask | ExposureMask |
                                    StructureNotifyMask);

    // tell window manager we want exact position
    XSizeHints size_hints = {
        .flags = USPosition,
        .x = px - wnd_size / 2,
        .y = py - wnd_size / 2,
    };
    XSetWMNormalHints(dpy, pp_i->fs_wnd, &size_hints);

    // update windows state properties
    Atom netwm_state_atom = XInternAtom(dpy, "_NET_WM_STATE", False);


    if (config.tie_fullscreen_window_to_browser) {
        Atom state_atoms[] = {
            XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False),// fill horizontal space
            XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False),// fill vertical space
            XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False),    // go fullscreen
            XInternAtom(dpy, "_NET_WM_STATE_SKIP_PAGER", False),    // do not appear in pager
            XInternAtom(dpy, "_NET_WM_STATE_SKIP_TASKBAR", False),  // do not appear in taskbar
        };
        XChangeProperty(dpy, pp_i->fs_wnd, netwm_state_atom, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&state_atoms, sizeof(state_atoms)/sizeof(state_atoms[0]));
    } else {

        Atom state_atoms[] = {
            XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_HORZ", False),// fill horizontal space
            XInternAtom(dpy, "_NET_WM_STATE_MAXIMIZED_VERT", False),// fill vertical space
            XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False),    // go fullscreen
        };
        XChangeProperty(dpy, pp_i->fs_wnd, netwm_state_atom, XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&state_atoms, sizeof(state_atoms)/sizeof(state_atoms[0]));
    }

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

    // set hint for a compositor
    long int net_wm_bypass_compositor_hint_on = 1;
    XChangeProperty(dpy, pp_i->fs_wnd, XInternAtom(dpy, "_NET_WM_BYPASS_COMPOSITOR", False),
                    XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *)&net_wm_bypass_compositor_hint_on, 1);

    // tell window manager we want to handle WM_DELETE_WINDOW
    Atom wm_delete_window_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    append_wm_protocol(dpy, pp_i->fs_wnd, wm_delete_window_atom);

    // show window
    XMapRaised(dpy, pp_i->fs_wnd);
    XSync(dpy, False);

    // round trip to browser thread to get browser window
    tp->browser_window = None;
    ppb_core_call_on_browser_thread(pp_i->id, get_browser_window, tp);
    pthread_barrier_wait(&cross_thread_call_barrier);

    // set window transient for browser window
    if (config.tie_fullscreen_window_to_browser) {
        if (tp->browser_window != None)
            XSetTransientForHint(dpy, pp_i->fs_wnd, tp->browser_window);
        else
            trace_error("%s, can't get tp->browser_window\n", __func__);
    }

    pthread_mutex_lock(&display.lock);
    pp_i->is_fullscreen = 1;
    pp_i->fs_width_current = wnd_size;
    pp_i->fs_height_current = wnd_size;
    pthread_mutex_unlock(&display.lock);

    int called_did_change_view = 0;
    int graphics_expose_events_in_queue = 0;
    int quit_message_received = 0;
    int graphics_expose_was_at_least_once = 0;

    while (1) {
        XEvent  ev;
        int     handled = 0;
        KeySym  keysym;

        if (quit_message_received && graphics_expose_was_at_least_once)
            goto quit_and_destroy_fs_wnd;

        XNextEvent(dpy, &ev);
        switch (ev.type) {
            case KeyPress:
                keysym = XLookupKeysym(&ev.xkey, 0);
                if (XK_Escape == keysym)
                    quit_message_received = 1;
                break;

            case ClientMessage:
                handled = 1;
                if (ev.xclient.data.l[0] == wm_delete_window_atom)
                    quit_message_received = 1;

                if (ev.xclient.message_type == freshwrappercommand_atom) {
                    switch (ev.xclient.data.l[0]) {
                    case FSCMD_FULLSCREEN_WAIT_IS_OVER:
                        if (!called_did_change_view) {
                            call_did_change_view(pp_i->id, 1);
                            called_did_change_view = 1;

                            // add unrequested GraphicsExpose in case some was lost during
                            // fullscreen transition
                            craft_graphicsexpose_event(&ev, dpy, pp_i);
                            handled = 0; // feed HandleEvent with crafted event
                        }
                        break;

                    case FSCMD_VSYNC:
                        if (graphics_expose_events_in_queue > 0) {
                            craft_graphicsexpose_event(&ev, dpy, pp_i);

                            graphics_expose_events_in_queue -= 1;
                            graphics_expose_was_at_least_once = 1;

                            handled = 0; // feed HandleEvent with crafted event
                        }
                        break;

                    case FSCMD_UNDEFINED:
                    default:
                        // do nothing
                        break;
                    }
                }

                break;

            case ConfigureNotify:
                pthread_mutex_lock(&display.lock);
                pp_i->fs_width_current = ev.xconfigure.width;
                pp_i->fs_height_current = ev.xconfigure.height;
                pthread_mutex_unlock(&display.lock);

                handled = 1;
                break;

            case ReparentNotify:
            case MapNotify:
            case UnmapNotify:
            case DestroyNotify:
            case Expose:
                handled = 1;
                break;

            case GraphicsExpose:
                if (config.enable_vsync) {
                    graphics_expose_events_in_queue += 1;
                    handled = 1;
                } else {
                    graphics_expose_was_at_least_once = 1;
                    handled = 0;
                }
                break;
        }

        ev.xany.display = display.x;

        if (!handled) {
            struct handle_event_comt_param_s *params = g_slice_alloc(sizeof(*params));
            params->instance_id =   pp_i->id;
            params->ev =            ev;
            ppb_core_call_on_browser_thread(pp_i->id, handle_event_ptac, params);
            pthread_barrier_wait(&cross_thread_call_barrier);
        }
    }

quit_and_destroy_fs_wnd:

    pthread_mutex_lock(&display.lock);
    pp_i->is_fullscreen = 0;
    pp_i->is_fullscreen_apparent = 0;
    pthread_mutex_unlock(&display.lock);

    XDestroyWindow(dpy, pp_i->fs_wnd);
    XFlush(dpy);

    call_did_change_view(pp_i->id, 0);

    // GraphicsExpose could be jammed in delay loop. We need to call it here explicitly,
    // to prevent callback chain breakage.
    while (graphics_expose_events_in_queue > 0) {
        XEvent ev = {
            .xgraphicsexpose = {
                .type =     GraphicsExpose,
                .display =  dpy,
                .drawable = pp_i->wnd,
                .width =    pp_i->width,
                .height =   pp_i->height,
            }
        };

        struct handle_event_comt_param_s *params = g_slice_alloc(sizeof(*params));
        params->instance_id =   pp_i->id;
        params->ev =            ev;
        ppb_core_call_on_browser_thread(pp_i->id, handle_event_ptac, params);
        pthread_barrier_wait(&cross_thread_call_barrier);

        graphics_expose_events_in_queue -= 1;
    }

    g_slice_free(struct thread_param_s, tp);
    trace_info_f("%s terminated\n", __func__);
}

static int enable_drm_vsync = 1;

static void
wait_vsync_drm(void)
{
    if (display.dri_fd < 0)
        return;

    int ret;
    do {
        drm_wait_vblank_t dwv = {
            .request.type =     _DRM_VBLANK_RELATIVE,
            .request.sequence = 1,
        };

        ret = ioctl(display.dri_fd, DRM_IOCTL_WAIT_VBLANK, &dwv);
    } while (ret != 0 && errno == EINTR);

    if (ret != 0) {
        trace_warning("Got an error (errno=%d) while waiting for VSYNC event. Disabling VSYNC.\n",
                      errno);
        enable_drm_vsync = 0;
    }

    if (config.vsync_afterwait_us > 0)
        usleep(config.vsync_afterwait_us);
}

/// thread used for active waiting
static
void *
delay_thread(void *param)
{
    struct thread_param_s *tp = param;
    struct pp_instance_s *pp_i = tp->pp_i;

    // first, wait some time before window manager performs fullscreen transition
    usleep(config.fs_delay_ms * 1000);

    pthread_mutex_lock(&display.lock);
    if (pp_i->is_fullscreen) {
        XEvent ev = {
            .xclient = {
                .type =         ClientMessage,
                .window =       pp_i->fs_wnd,
                .message_type = freshwrappercommand_atom,
                .format =       32,
                .data.l[0] =    FSCMD_FULLSCREEN_WAIT_IS_OVER,
            }
        };

        XSendEvent(display.x, ev.xclient.window, False, NoEventMask, &ev);
        XFlush(display.x);
    }
    pthread_mutex_unlock(&display.lock);

    if (!config.enable_vsync) {
        // if no vsync required, just wait until thread could be terminated
        while (g_atomic_int_get(&run_delay_thread)) {
            usleep(300 * 1000);
        }

        return NULL;
    }

    // wait for vsync events, pass them back to fullscreen thread
    while (g_atomic_int_get(&run_delay_thread)) {
        pthread_mutex_lock(&display.lock);
        if (pp_i->is_fullscreen) {
            XEvent ev = {
                .xclient = {
                    .type =         ClientMessage,
                    .window =       pp_i->fs_wnd,
                    .message_type = freshwrappercommand_atom,
                    .format =       32,
                    .data.l[0] =    FSCMD_VSYNC,
                }
            };

            XSendEvent(display.x, ev.xclient.window, False, NoEventMask, &ev);
            XFlush(display.x);
        }
        pthread_mutex_unlock(&display.lock);

        if (display.dri_fd >= 0) {
            if (enable_drm_vsync)
                wait_vsync_drm();
            else
                usleep(16 * 1000);
        } else {
            usleep(16 * 1000);
        }
    }

    return NULL;
}

static
void *
fullscreen_window_thread(void *p)
{
    GAsyncQueue *async_q = fullscreen_transition_queue;
    Display *dpy = XOpenDisplay(NULL);

    freshwrappercommand_atom = XInternAtom(display.x, "FRESHWRAPPER_COMMAND", False);

    g_async_queue_ref(async_q);
    while (g_atomic_int_get(&run_fullscreen_thread)) {
        struct thread_param_s *tp = g_async_queue_pop(async_q);
        pthread_t t;

        g_atomic_int_set(&currently_fullscreen, 1);

        g_atomic_int_set(&run_delay_thread, 1);
        pthread_create(&t, NULL, delay_thread, tp);

        fullscreen_window_thread_int(dpy, tp);

        g_atomic_int_set(&run_delay_thread, 0);
        pthread_join(t, NULL);

        g_atomic_int_set(&currently_fullscreen, 0);
    }

    pthread_barrier_destroy(&cross_thread_call_barrier);
    g_async_queue_unref(async_q);
    XCloseDisplay(dpy);
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

    if (!fullscreen_transition_queue) {
        pthread_t t;
        fullscreen_transition_queue = g_async_queue_new();
        pthread_create(&t, NULL, fullscreen_window_thread, NULL);
        pthread_detach(t);
    }

    if (fullscreen) {
        if (g_atomic_int_get(&currently_fullscreen)) {
            // can't go fullscreen -- other instance is still fullscreen
            return PP_FALSE;
        }

        struct thread_param_s *tparams = g_slice_alloc(sizeof(*tparams));
        tparams->pp_i = pp_i;
        g_async_queue_push(fullscreen_transition_queue, tparams);
    } else if (g_atomic_int_get(&currently_fullscreen)) {
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

    size->width = pp_i->fs_width > 0 ? pp_i->fs_width : display.min_width;
    size->height = pp_i->fs_height > 0 ? pp_i->fs_height : display.min_height;

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

static
void
__attribute__((constructor))
constructor_ppb_flash_fullscreen(void)
{
    pthread_barrier_init(&cross_thread_call_barrier, NULL, 2);
    register_interface(PPB_FLASHFULLSCREEN_INTERFACE_1_0, &ppb_flash_fullscreen_interface_1_0);
}
