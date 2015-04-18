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

#include "x11_event_thread.h"
#include <glib.h>
#include <pthread.h>
#include "pp_resource.h"
#include "tables.h"


static GHashTable        *ht = NULL;  // Window -> struct ht_entry_s
static GHashTable        *socket_ht = NULL; // plug_wnd -> socket wnd
static Display           *dpy = NULL;
static pthread_mutex_t    lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t          thread;
static uint32_t           thread_started = 0;
static pthread_barrier_t  task_pass_barrier;
static Atom               freshwrapper_x11et_cmd_atom;
static Window             task_pass_wnd;

struct ht_entry_s {
    PP_Instance             instance;
    NPP_HandleEventProcPtr  proc;
    uint32_t                is_xembed;
    Window                  plug_wnd;
};


static
void
ht_entry_s_free(gpointer data)
{
    g_slice_free1(sizeof(struct ht_entry_s), data);
}

static
void
__attribute__((constructor))
constructor_x11_event_thread(void)
{
    ht = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, ht_entry_s_free);
    socket_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pthread_barrier_init(&task_pass_barrier, NULL, 2);
}

static
void
__attribute__((destructor))
destructor_x11_event_thread(void)
{
    g_hash_table_unref(ht);
    g_hash_table_unref(socket_ht);
    pthread_barrier_destroy(&task_pass_barrier);
}

static
void
call_handle_event_ptac(void *param)
{
    XEvent            *ev = param;
    struct ht_entry_s *d;

    pthread_mutex_lock(&lock);
    Window socket_wnd = GPOINTER_TO_SIZE(g_hash_table_lookup(socket_ht,
                                                             GSIZE_TO_POINTER(ev->xany.window)));
    d = g_hash_table_lookup(ht, GSIZE_TO_POINTER(socket_wnd));
    pthread_mutex_unlock(&lock);
    if (d && d->proc) {
        struct pp_instance_s *pp_i = tables_get_pp_instance(d->instance);

        if (pp_i && pp_i->npp) {
            ev->xany.display = display.x;
            d->proc(pp_i->npp, ev);
        }
    }

    g_slice_free1(sizeof(*ev), ev);
}

static
void *
x11_event_thread_func(void *param)
{
    while (1) {
        XEvent *ev = g_slice_alloc0(sizeof(*ev));

        XNextEvent(dpy, ev);
        if (ev->type == ClientMessage && ev->xclient.message_type == freshwrapper_x11et_cmd_atom) {
            XSelectInput(dpy, (Window)ev->xclient.data.l[0], ev->xclient.data.l[1]);
            XFlush(dpy);

            pthread_barrier_wait(&task_pass_barrier);
            continue;
        }

        Window socket_wnd;
        socket_wnd = GPOINTER_TO_SIZE(g_hash_table_lookup(socket_ht,
                                                          GSIZE_TO_POINTER(ev->xany.window)));

        struct ht_entry_s *d = g_hash_table_lookup(ht, GSIZE_TO_POINTER(socket_wnd));
        if (socket_wnd != ev->xany.window) {
            if (ev->type == ClientMessage) {
                printf("xembed: ClientMessage\n");
            }
        }

        if (d) {
            struct pp_instance_s *pp_i = tables_get_pp_instance(d->instance);
            if (pp_i && pp_i->npp)
                npn.pluginthreadasynccall(pp_i->npp, call_handle_event_ptac, ev);
        } else
            g_slice_free1(sizeof(*ev), ev);
    }

    return NULL;
}

Window
x11et_register_window(PP_Instance instance, Window wnd, NPP_HandleEventProcPtr handle_event_cb,
                      uint32_t is_xembed)
{
    pthread_mutex_lock(&lock);
    if (!thread_started) {
        dpy = XOpenDisplay(NULL);
        freshwrapper_x11et_cmd_atom = XInternAtom(dpy, "FRESHWRAPPER_X11ET_CMD", False);

        // create helper window to pass tasks to the event thread
        task_pass_wnd = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0, 10, 10, 0, 0,
                                            0x000000);
        XSelectInput(dpy, task_pass_wnd, StructureNotifyMask); // which mask?
        XFlush(dpy);

        pthread_create(&thread, NULL, x11_event_thread_func, NULL);
        pthread_detach(thread);
        thread_started = 1;
    }

    struct ht_entry_s *entry = g_hash_table_lookup(ht, GSIZE_TO_POINTER(wnd));
    pthread_mutex_unlock(&lock);

    if (entry) {
        // already registered
        return entry->plug_wnd;
    }

    entry = g_slice_alloc0(sizeof(*entry));
    entry->instance =   instance;
    entry->proc =       handle_event_cb;
    entry->is_xembed =  is_xembed;
    entry->plug_wnd =   wnd;

    pthread_mutex_lock(&display.lock);
    if (is_xembed) {
        entry->plug_wnd = XCreateSimpleWindow(display.x, DefaultRootWindow(display.x), 0, 0,
                                              200, 200, 0, 0, 0x000000);
        XReparentWindow(display.x, entry->plug_wnd, wnd, 0, 0);

        Atom xembed_info_atom = XInternAtom(display.x, "_XEMBED_INFO", False);
        unsigned long buffer[2] = { 1, 1 }; // version 1, flag (1 << 0)

        XChangeProperty(display.x, entry->plug_wnd, xembed_info_atom, xembed_info_atom, 32,
                        PropModeReplace, (unsigned char *)buffer, 2);
    }

    XEvent ev = {
        .xclient = {
            .type =         ClientMessage,
            .message_type = freshwrapper_x11et_cmd_atom,
            .format =       32,
            .data.l[0] =    entry->plug_wnd,
            .data.l[1] =    KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask
                            | EnterWindowMask | LeaveWindowMask | PointerMotionMask | ExposureMask
                            | FocusChangeMask,
        }
    };

    XSendEvent(display.x, task_pass_wnd, True, 0, &ev);
    XFlush(display.x);
    pthread_mutex_unlock(&display.lock);

    pthread_mutex_lock(&lock);
    g_hash_table_insert(ht, GSIZE_TO_POINTER(wnd), entry);
    g_hash_table_insert(socket_ht, GSIZE_TO_POINTER(entry->plug_wnd), GSIZE_TO_POINTER(wnd));
    pthread_mutex_unlock(&lock);

    pthread_barrier_wait(&task_pass_barrier);

    return entry->plug_wnd;
}

void
x11et_unregister_window(Window wnd)
{
    pthread_mutex_lock(&lock);
    struct ht_entry_s *entry = g_hash_table_lookup(ht, GSIZE_TO_POINTER(wnd));
    pthread_mutex_unlock(&lock);

    if (!entry)
        return;

    XEvent ev = {
        .xclient = {
            .type =         ClientMessage,
            .message_type = freshwrapper_x11et_cmd_atom,
            .format =       32,
            .data.l[0] =    entry->plug_wnd,
            .data.l[1] =    0,
        }
    };

    pthread_mutex_lock(&display.lock);
    XSendEvent(display.x, task_pass_wnd, True, 0, &ev);
    XFlush(display.x);
    if (entry->is_xembed)
        XDestroyWindow(display.x, entry->plug_wnd);
    pthread_mutex_unlock(&display.lock);

    pthread_barrier_wait(&task_pass_barrier);

    pthread_mutex_lock(&lock);
    g_hash_table_remove(socket_ht, GSIZE_TO_POINTER(entry->plug_wnd));
    g_hash_table_remove(ht, GSIZE_TO_POINTER(wnd));
    pthread_mutex_unlock(&lock);
}
