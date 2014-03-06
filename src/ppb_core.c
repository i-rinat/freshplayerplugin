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
#include "pp_resource.h"
#include "globals.h"


static pthread_t main_thread;

 __attribute__((constructor))
static
void
ppb_core_constructor(void)
{
    main_thread = pthread_self();
}

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

struct comt_proxy_param_s {
    struct PP_CompletionCallback    callback;
    int32_t                         result_to_pass;
};

static
void
comt_proxy(void *param)
{
    struct comt_proxy_param_s *p = param;

    if (p->callback.func) {
        p->callback.func(p->callback.user_data, p->result_to_pass);
    }
    free(p);
}

void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result)
{
    struct comt_proxy_param_s *p = malloc(sizeof(*p));

    p->callback.func = callback.func;
    p->callback.user_data = callback.user_data;
    p->callback.flags = callback.flags;
    p->result_to_pass = result;
    npn.pluginthreadasynccall(obligatory_npp_instance, comt_proxy, p);
    return;
}

PP_Bool
ppb_core_is_main_thread(void)
{
    return pthread_equal(main_thread, pthread_self());
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
