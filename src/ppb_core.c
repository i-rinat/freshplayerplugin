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

#include <assert.h>
#include "ppb_core.h"
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "pp_interface.h"
#include "ppb_message_loop.h"
#include <ppapi/c/pp_errors.h>


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
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

void
ppb_core_trampoline_to_main_thread(struct PP_CompletionCallback callback, int32_t result,
                                   const char *origin)
{
    PP_Resource main_message_loop = ppb_message_loop_get_for_main_thread();
    if (main_message_loop == 0)
        trace_error("%s, no main loop\n", __func__);
    const int depth = ppb_message_loop_get_depth(main_message_loop);
    const int32_t delay_in_milliseconds = 0;
    ppb_message_loop_post_work_with_result(main_message_loop, callback, delay_in_milliseconds,
                                           result, depth, origin);
}

void
ppb_core_call_on_main_thread2(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                              int32_t result, const char *origin)
{
    PP_Resource main_message_loop = ppb_message_loop_get_for_main_thread();
    if (main_message_loop == 0)
        trace_error("%s, no main loop\n", __func__);
    const int depth = 1;
    ppb_message_loop_post_work_with_result(main_message_loop, callback, delay_in_milliseconds,
                                           result, depth, origin);
}

void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result)
{
    return ppb_core_call_on_main_thread2(delay_in_milliseconds, callback, result, __func__);
}

struct call_on_browser_thread_task_s {
    void            (*func)(void *);
    void            *user_data;
};

static
void
call_on_browser_thread_comt(void *user_data, int32_t result)
{
    struct call_on_browser_thread_task_s *task = user_data;
    task->func(task->user_data);
    g_slice_free(struct call_on_browser_thread_task_s, task);
}

static
void
activate_browser_thread_ml_ptac(void *param)
{
    // If task was already executed and queue is empty, ppb_message_loop_run_int() will return
    // without waiting.
    PP_Resource m_loop = ppb_message_loop_get_for_browser_thread();
    ppb_message_loop_run_int(m_loop, ML_INCREASE_DEPTH | ML_EXIT_ON_EMPTY | ML_NESTED);
}

// Schedules task for execution on browser thread.
//
// Since there is no access to browser event loop, we start a nested event loop which is terminated
// as long as there is no tasks left. That way we can implement waiting as entering a nested loop
// and thus avoid deadlocks.
void
ppb_core_call_on_browser_thread(PP_Instance instance, void (*func)(void *), void *user_data)
{
    struct call_on_browser_thread_task_s *task = g_slice_alloc(sizeof(*task));
    task->func = func;
    task->user_data = user_data;

    // Push task into queue. The only purpose is to put task into queue even if message loop
    // is currenly terminating (in teardown state), so we are ignoring that. There are three
    // possible loop states. Message loop is either running, stopped, or terminating. If it's
    // still running, task will be executed in the context of that loop. If it's stopped or
    // stopping right now, task will be pushed to a queue. After that code below will schedule
    // nested loop on browser thread.
    PP_Resource m_loop = ppb_message_loop_get_for_browser_thread();
    ppb_message_loop_post_work_with_result(m_loop, PP_MakeCCB(call_on_browser_thread_comt, task), 0,
                                           PP_OK, 0, __func__);

    struct pp_instance_s *pp_i = instance ? tables_get_pp_instance(instance)
                                          : tables_get_some_pp_instance();
    if (!pp_i) {
        trace_error("%s, no alive instance available\n", __func__);
        return;
    }

    // Schedule activation routine.
    pthread_mutex_lock(&display.lock);
    if (pp_i->npp)
        npn.pluginthreadasynccall(pp_i->npp, activate_browser_thread_ml_ptac, user_data);
    pthread_mutex_unlock(&display.lock);
}

PP_Bool
ppb_core_is_main_thread(void)
{
    return ppb_message_loop_get_current() == ppb_message_loop_get_for_main_thread();
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_core_add_ref_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    ppb_core_add_ref_resource(resource);
}

TRACE_WRAPPER
void
trace_ppb_core_release_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    ppb_core_release_resource(resource);
}

TRACE_WRAPPER
PP_Time
trace_ppb_core_get_time(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_get_time();
}

TRACE_WRAPPER
PP_TimeTicks
trace_ppb_core_get_time_ticks(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_get_time_ticks();
}

TRACE_WRAPPER
void
trace_ppb_core_call_on_main_thread(int32_t delay_in_milliseconds,
                                   struct PP_CompletionCallback callback, int32_t result)
{
    trace_info("[PPB] {full} %s delay_in_milliseconds=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}, result=%d\n", __func__+6, delay_in_milliseconds, callback.func,
               callback.user_data, callback.flags, result);
    ppb_core_call_on_main_thread(delay_in_milliseconds, callback, result);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_core_is_main_thread(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_core_is_main_thread();
}


const struct PPB_Core_1_0 ppb_core_interface_1_0 = {
    .AddRefResource =   TWRAPF(ppb_core_add_ref_resource),
    .ReleaseResource =  TWRAPF(ppb_core_release_resource),
    .GetTime =          TWRAPF(ppb_core_get_time),
    .GetTimeTicks =     TWRAPF(ppb_core_get_time_ticks),
    .CallOnMainThread = TWRAPF(ppb_core_call_on_main_thread),
    .IsMainThread =     TWRAPF(ppb_core_is_main_thread),
};

static
void
__attribute__((constructor))
constructor_ppb_core(void)
{
    register_interface(PPB_CORE_INTERFACE_1_0, &ppb_core_interface_1_0);
}
