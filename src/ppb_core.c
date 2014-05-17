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
#include "ppb_core.h"
#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"


static GAsyncQueue *async_q = NULL;


void
ppb_core_add_ref_resource(PP_Resource resource)
{
    pp_resource_ref(resource);
}

void
ppb_core_release_resource(PP_Resource resource)
{
    pp_resource_unref(resource);
}

PP_Time
ppb_core_get_time(void)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

PP_TimeTicks
ppb_core_get_time_ticks(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

struct comt_task_s {
    struct PP_CompletionCallback    callback;
    int32_t                     result_to_pass;
    struct timespec             when;
    int                         terminate;
};

static
void
comt_proxy(void *param)
{
    struct comt_task_s *p = param;

    if (p->callback.func) {
        p->callback.func(p->callback.user_data, p->result_to_pass);
    }
    free(p);
}

gint
time_compare_func(gconstpointer a, gconstpointer b, gpointer user_data)
{
    const struct comt_task_s *task_a = a;
    const struct comt_task_s *task_b = b;

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

static
void *
comt_delay_thread(void *param)
{
    struct timespec now;
    GAsyncQueue *async_q = param;
    GQueue *int_q = g_queue_new();

    while (1) {
        struct comt_task_s *task = g_queue_peek_head(int_q);
        gint64 timeout = 1000 * 1000;
        if (task) {
            clock_gettime(CLOCK_REALTIME, &now);
            timeout = (task->when.tv_sec - now.tv_sec) * 1000 * 1000 +
                      (task->when.tv_nsec - now.tv_nsec) / 1000;
            if (timeout <= 0) {
                int terminate = task->terminate;
                // remove task from queue
                g_queue_pop_head(int_q);

                // run task
                NPP npp = tables_get_some_npp_instance();
                if (npp)
                    npn.pluginthreadasynccall(npp, comt_proxy, task);

                if (terminate)  // should terminate?
                    break;

                continue;   // run cycle again
            }
        }

        task = g_async_queue_timeout_pop(async_q, timeout);
        if (task)
            g_queue_insert_sorted(int_q, task, time_compare_func, NULL);
    }

    g_queue_free(int_q);
    return NULL;
}

void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result)
{
    struct comt_task_s *task = malloc(sizeof(*task));

    task->callback = callback;
    task->result_to_pass = result;
    task->terminate = 0;

    if (delay_in_milliseconds <= 0) {
        NPP npp = tables_get_some_npp_instance();
        if (npp)
            npn.pluginthreadasynccall(npp, comt_proxy, task);
    } else {
        // start thread and create async queue (once)
        if (!async_q) {
            async_q = g_async_queue_new();
            g_thread_new("delay-thread", comt_delay_thread, async_q);
        }

        // calculate absolute time callback should be run at
        clock_gettime(CLOCK_REALTIME, &task->when);
        task->when.tv_sec += delay_in_milliseconds / 1000;
        task->when.tv_nsec += (delay_in_milliseconds % 1000) * 1000 * 1000;
        while (task->when.tv_nsec >= 1000 * 1000 * 1000) {
            task->when.tv_sec += 1;
            task->when.tv_nsec -= 1000 * 1000 * 1000;
        }

        g_async_queue_push(async_q, task);
    }
    return;
}

PP_Bool
ppb_core_is_main_thread(void)
{
    return pthread_equal(np_main_thread, pthread_self());
}

// trace wrappers
static
void
trace_ppb_core_add_ref_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    ppb_core_add_ref_resource(resource);
}

static
void
trace_ppb_core_release_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    ppb_core_release_resource(resource);
}

static
PP_Time
trace_ppb_core_get_time(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_get_time();
}

static
PP_TimeTicks
trace_ppb_core_get_time_ticks(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_get_time_ticks();
}

static
void
trace_ppb_core_call_on_main_thread(int32_t delay_in_milliseconds,
                                   struct PP_CompletionCallback callback, int32_t result)
{
    trace_info("[PPB] {full} %s delay_in_milliseconds=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}, result=%d\n", __func__+6, delay_in_milliseconds, callback.func,
               callback.user_data, callback.flags, result);
    ppb_core_call_on_main_thread(delay_in_milliseconds, callback, result);
}

static
PP_Bool
trace_ppb_core_is_main_thread(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_is_main_thread();
}


const struct PPB_Core_1_0 ppb_core_interface_1_0 = {
    .AddRefResource =   trace_ppb_core_add_ref_resource,
    .ReleaseResource =  trace_ppb_core_release_resource,
    .GetTime =          trace_ppb_core_get_time,
    .GetTimeTicks =     trace_ppb_core_get_time_ticks,
    .CallOnMainThread = trace_ppb_core_call_on_main_thread,
    .IsMainThread =     trace_ppb_core_is_main_thread
};
