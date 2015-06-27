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
#include "xembed.h"
#include "reverse_constant.h"
#include "trace.h"
#include "utils.h"
#include "eintr_retry.h"
#include <poll.h>


static GHashTable        *ht = NULL;  // Window -> struct ht_entry_s
static GHashTable        *socket_ht = NULL; // plug_wnd -> socket wnd
static Display           *dpy = NULL;
static pthread_mutex_t    lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t          thread;
static uint32_t           thread_started = 0;
static pthread_barrier_t  task_pass_barrier;
static Atom               xembed_atom;
static Atom               xembed_info_atom;
static int                task_pipe[2];

struct ht_entry_s {
    PP_Instance             instance;
    NPP_HandleEventProcPtr  proc;
    uint32_t                is_xembed;
    Window                  plug_wnd;
};

enum cmd_e {
    X11ET_CMD_UNDEFINED         = 0,
    X11ET_CMD_REGISTER_WINDOW   = 1,
    X11ET_CMD_UNREGISTER_WINDOW = 2,
};

struct task_s {
    Window          socket_wnd;
    enum cmd_e      cmd;
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
send_xembed_message(Display *dpy, Window receiver, long int message, long int detail)
{
    XEvent ev = {
        .xclient = {
            .type =         ClientMessage,
            .window =       receiver,
            .message_type = xembed_atom,
            .format =       32,
            .data.l[0] =    CurrentTime,
            .data.l[1] =    message,
            .data.l[2] =    detail,
        }
    };

    XSendEvent(dpy, receiver, False, NoEventMask, &ev);
    XFlush(dpy);
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
void
x11et_handle_task(struct task_s *task)
{
    struct ht_entry_s  *entry;

    pthread_mutex_lock(&lock);
    entry = g_hash_table_lookup(ht, GSIZE_TO_POINTER(task->socket_wnd));
    pthread_mutex_unlock(&lock);

    if (!entry)
        return;

    switch (task->cmd) {
    case X11ET_CMD_REGISTER_WINDOW:
        if (entry->is_xembed) {
            const int screen = DefaultScreen(dpy);
            XSetWindowAttributes attrs = {
                .background_pixel = 0x000000,
                .backing_store =    Always,
            };

            entry->plug_wnd = XCreateWindow(dpy, task->socket_wnd, 0, 0, 200, 200, 0,
                                            DefaultDepth(dpy, screen),
                                            InputOutput, CopyFromParent,
                                            CWBackPixel | CWBackingStore, &attrs);

            unsigned long buffer[2] = { 1, XEMBED_MAPPED };

            XChangeProperty(dpy, entry->plug_wnd, xembed_info_atom, xembed_info_atom, 32,
                            PropModeReplace, (unsigned char *)buffer, 2);
        }

        pthread_mutex_lock(&lock);
        g_hash_table_insert(socket_ht, GSIZE_TO_POINTER(entry->plug_wnd),
                            GSIZE_TO_POINTER(task->socket_wnd));
        pthread_mutex_unlock(&lock);

        const long event_mask = KeyPressMask | KeyReleaseMask | ButtonPressMask
                                | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask
                                | PointerMotionMask | ExposureMask | FocusChangeMask;
        XSelectInput(dpy, entry->plug_wnd, event_mask);
        break;

    case X11ET_CMD_UNREGISTER_WINDOW:
        XSelectInput(dpy, entry->plug_wnd, 0);
        XFlush(dpy);
        if (entry->is_xembed)
            XDestroyWindow(dpy, entry->plug_wnd);

        pthread_mutex_lock(&lock);
        g_hash_table_remove(socket_ht, GSIZE_TO_POINTER(entry->plug_wnd));
        pthread_mutex_unlock(&lock);
        break;

    default:
        // should not happen
        break;
    }

    XFlush(dpy);
}

static
void
x11et_handle_xevent(void)
{
    XEvent *ev = g_slice_alloc0(sizeof(*ev));
    XNextEvent(dpy, ev);

    Window plug_wnd = ev->xany.window;
    Window socket_wnd = GPOINTER_TO_SIZE(g_hash_table_lookup(socket_ht,
                                                             GSIZE_TO_POINTER(plug_wnd)));

    struct ht_entry_s *d = g_hash_table_lookup(ht, GSIZE_TO_POINTER(socket_wnd));
    int skip_event = 0;
    if (d && d->is_xembed) {
        switch (ev->type) {
        case ClientMessage:
            switch (ev->xclient.data.l[1]) {
            case XEMBED_EMBEDDED_NOTIFY:
                skip_event = 1;
                break;

            case XEMBED_WINDOW_ACTIVATE:
            case XEMBED_WINDOW_DEACTIVATE:
            case XEMBED_MODALITY_ON:
            case XEMBED_MODALITY_OFF:
                // not handled
                skip_event = 1;
                break;

            case XEMBED_FOCUS_IN:
                memset(ev, 0, sizeof(*ev));
                ev->xfocus.type =   FocusIn;
                ev->xfocus.window = plug_wnd;
                ev->xfocus.mode =   NotifyNormal;
                ev->xfocus.detail = NotifyDetailNone;
                break;

            case XEMBED_FOCUS_OUT:
                memset(ev, 0, sizeof(*ev));
                ev->xfocus.type =   FocusOut;
                ev->xfocus.window = plug_wnd;
                ev->xfocus.mode =   NotifyNormal;
                ev->xfocus.detail = NotifyDetailNone;
                break;

            default:
                trace_error("%s, unknown XEmbed message %d\n", __func__,
                            (int)ev->xclient.data.l[1]);
                skip_event = 1;
                break;
            }
            break;

        case ButtonPress:
            send_xembed_message(dpy, socket_wnd, XEMBED_REQUEST_FOCUS, 0);
            break;

        case FocusIn:
        case FocusOut:
            // native focus events should be skipped if XEmbed is in use
            skip_event = 1;
            break;
        }
    }

    if (d && !skip_event) {
        struct pp_instance_s *pp_i = tables_get_pp_instance(d->instance);
        if (pp_i && pp_i->npp)
            npn.pluginthreadasynccall(pp_i->npp, call_handle_event_ptac, ev);
        else
            g_slice_free1(sizeof(*ev), ev);
    } else {
        g_slice_free1(sizeof(*ev), ev);
    }
}

static
void *
x11_event_thread_func(void *param)
{
    int x11_fd = ConnectionNumber(dpy);

    struct pollfd fds[2] = {
        { .fd = task_pipe[0], .events = POLLIN, },
        { .fd = x11_fd,       .events = POLLIN, },
    };

    while (1) {
        int ret = poll(fds, sizeof(fds)/sizeof(fds[0]), -1);

        if (ret == -1) {
            trace_error("%s, poll() failed, errno=%d\n", __func__, errno);
            sleep(1);   // relax tight loop
            continue;
        }

        if (fds[0].revents & POLLIN) {
            // task has come
            struct task_s       task;

            ret = RETRY_ON_EINTR(read(task_pipe[0], &task, sizeof(task)));
            if (ret != sizeof(task))
                trace_error("%s, read wrong number of bytes from task_fd\n", __func__);

            x11et_handle_task(&task);
            pthread_barrier_wait(&task_pass_barrier);
            continue;
        }

        if (fds[1].revents & POLLIN) {
            x11et_handle_xevent();
            continue;
        }
    }

    // TODO: do we need to shutdown thread?
    close(task_pipe[0]); task_pipe[0] = -1;
    close(task_pipe[1]); task_pipe[1] = -1;

    return NULL;
}

static
void
x11et_start_thread(void)
{
    dpy = XOpenDisplay(NULL);

    xembed_atom =                 XInternAtom(dpy, "_XEMBED", False);
    xembed_info_atom =            XInternAtom(dpy, "_XEMBED_INFO", False);

    if (pipe(task_pipe) == 0) {
        make_nonblock(task_pipe[0]);
        make_nonblock(task_pipe[1]);
    } else {
        trace_error("%s, can't create pipe\n", __func__);
        task_pipe[0] = -1;
        task_pipe[1] = -1;
    }

    pthread_create(&thread, NULL, x11_event_thread_func, NULL);
    pthread_detach(thread);
}

Window
x11et_register_window(PP_Instance instance, Window wnd, NPP_HandleEventProcPtr handle_event_cb,
                      uint32_t is_xembed)
{
    pthread_mutex_lock(&lock);
    if (!thread_started) {
        x11et_start_thread();
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

    pthread_mutex_lock(&lock);
    g_hash_table_insert(ht, GSIZE_TO_POINTER(wnd), entry);
    pthread_mutex_unlock(&lock);

    struct task_s task = {
        .socket_wnd =   wnd,
        .cmd =          X11ET_CMD_REGISTER_WINDOW,
    };

    if (task_pipe[1] >= 0) {
        int ret = RETRY_ON_EINTR(write(task_pipe[1], &task, sizeof(task)));
        if (ret != sizeof(task))
            trace_error("%s, can't write to task_pipe\n", __func__);

        pthread_barrier_wait(&task_pass_barrier);
    } else {
        trace_warning("%s, no pipe to send message\n", __func__);
    }

    // entry->plug_wnd here will contain plug window if XEmbed is used
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

    struct task_s task = {
        .socket_wnd =   wnd,
        .cmd =          X11ET_CMD_UNREGISTER_WINDOW,
    };

    if (task_pipe[1] >= 0) {
        int ret = RETRY_ON_EINTR(write(task_pipe[1], &task, sizeof(task)));
        if (ret != sizeof(task))
            trace_error("%s, can't write to task_pipe\n", __func__);

        pthread_barrier_wait(&task_pass_barrier);
    } else {
        trace_warning("%s, no pipe to send message\n", __func__);
    }

    pthread_mutex_lock(&lock);
    g_hash_table_remove(ht, GSIZE_TO_POINTER(wnd));
    pthread_mutex_unlock(&lock);
}
