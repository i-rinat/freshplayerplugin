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

#include "ppb_message_loop.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include <inttypes.h>
#include <glib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"


PP_Resource
ppb_message_loop_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return 0;

    PP_Resource message_loop = pp_resource_allocate(PP_RESOURCE_MESSAGE_LOOP, pp_i);
    struct pp_message_loop_s *ml = pp_resource_acquire(message_loop, PP_RESOURCE_MESSAGE_LOOP);

    ml->async_q = g_async_queue_new();

    pp_resource_release(message_loop);
    return message_loop;
}

void
ppb_message_loop_destroy(void *p)
{
    struct pp_message_loop_s *ml = p;

    if (ml->async_q) {
        g_async_queue_unref(ml->async_q);
        ml->async_q = NULL;
    }
}

PP_Resource
ppb_message_loop_get_for_main_thread(void)
{
    return 0;
}

PP_Resource
ppb_message_loop_get_current(void)
{
    return 0;
}

int32_t
ppb_message_loop_attach_to_current_thread(PP_Resource message_loop)
{
    return PP_ERROR_FAILED;
}

struct message_loop_task_s {
    struct timespec                 when;
    int                             terminate;
    struct PP_CompletionCallback    ccb;
    int32_t                         result_to_pass;
};

static gint
time_compare_func(gconstpointer a, gconstpointer b, gpointer user_data)
{
    const struct message_loop_task_s *task_a = a;
    const struct message_loop_task_s *task_b = b;

    if (task_a->when.tv_sec < task_b->when.tv_sec)
        return -1;
    else if (task_a->when.tv_sec > task_b->when.tv_sec)
        return 1;
    else if (task_a->when.tv_nsec < task_b->when.tv_nsec)
        return -1;
    else if (task_a->when.tv_nsec > task_b->when.tv_nsec)
        return 1;
    else
        return 0;
}

int32_t
ppb_message_loop_run(PP_Resource message_loop)
{
    struct pp_message_loop_s *ml = pp_resource_acquire(message_loop, PP_RESOURCE_MESSAGE_LOOP);
    if (!ml)
        return PP_ERROR_BADRESOURCE;

    GQueue *int_q = g_queue_new();
    struct timespec now;

    while (1) {
        struct message_loop_task_s *task = g_queue_peek_head(int_q);
        gint64 timeout = 1000 * 1000;
        if (task) {
            clock_gettime(CLOCK_REALTIME, &now);
            timeout = (task->when.tv_sec - now.tv_sec) * 1000 * 1000 +
                      (task->when.tv_nsec - now.tv_nsec) / 1000;
            if (timeout <= 0) {
                int terminate = task->terminate;
                // remove task from the queue
                g_queue_pop_head(int_q);

                if (terminate) {
                    // TODO: decide what to do with remaining tasks
                    g_slice_free(struct message_loop_task_s, task);
                    break;
                }

                // run task
                const struct PP_CompletionCallback ccb = task->ccb;
                if (ccb.func) {
                    ccb.func(ccb.user_data, task->result_to_pass);
                }

                // free task
                g_slice_free(struct message_loop_task_s, task);
                continue;   // run cycle again
            }
        }

        task = g_async_queue_timeout_pop(ml->async_q, timeout);
        if (task)
            g_queue_insert_sorted(int_q, task, time_compare_func, NULL);
    }

    g_queue_free(int_q);
    return PP_OK;
}

int32_t
ppb_message_loop_post_work(PP_Resource message_loop, struct PP_CompletionCallback callback,
                           int64_t delay_ms)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_message_loop_post_quit(PP_Resource message_loop, PP_Bool should_destroy)
{
    return PP_ERROR_FAILED;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_message_loop_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_message_loop_create(instance);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_message_loop_get_for_main_thread(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_message_loop_get_for_main_thread();
}

TRACE_WRAPPER
PP_Resource
trace_ppb_message_loop_get_current(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_message_loop_get_current();
}

TRACE_WRAPPER
int32_t
trace_ppb_message_loop_attach_to_current_thread(PP_Resource message_loop)
{
    trace_info("[PPB] {zilch} %s message_loop=%d\n", __func__+6, message_loop);
    return ppb_message_loop_attach_to_current_thread(message_loop);
}

TRACE_WRAPPER
int32_t
trace_ppb_message_loop_run(PP_Resource message_loop)
{
    trace_info("[PPB] {full} %s message_loop=%d\n", __func__+6, message_loop);
    return ppb_message_loop_run(message_loop);
}

TRACE_WRAPPER
int32_t
trace_ppb_message_loop_post_work(PP_Resource message_loop, struct PP_CompletionCallback callback,
                                 int64_t delay_ms)
{
    trace_info("[PPB] {zilch} %s message_loop=%d, callback={.func=%p, .user_data=%p, .flags=%u}, "
               "delay_ms=%"PRId64"\n", __func__+6, message_loop, callback.func, callback.user_data,
               callback.flags, delay_ms);
    return ppb_message_loop_post_work(message_loop, callback, delay_ms);
}

TRACE_WRAPPER
int32_t
trace_ppb_message_loop_post_quit(PP_Resource message_loop, PP_Bool should_destroy)
{
    trace_info("[PPB] {zilch} %s message_loop=%d, should_destroy=%d\n", __func__+6,
               message_loop, should_destroy);
    return ppb_message_loop_post_quit(message_loop, should_destroy);
}

const struct PPB_MessageLoop_1_0 ppb_message_loop_interface_1_0 = {
    .Create =                TWRAPF(ppb_message_loop_create),
    .GetForMainThread =      TWRAPZ(ppb_message_loop_get_for_main_thread),
    .GetCurrent =            TWRAPZ(ppb_message_loop_get_current),
    .AttachToCurrentThread = TWRAPZ(ppb_message_loop_attach_to_current_thread),
    .Run =                   TWRAPF(ppb_message_loop_run),
    .PostWork =              TWRAPZ(ppb_message_loop_post_work),
    .PostQuit =              TWRAPZ(ppb_message_loop_post_quit),
};
