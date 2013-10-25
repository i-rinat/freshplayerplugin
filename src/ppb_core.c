#include <ppapi/c/ppb_core.h>
#include <stddef.h>
#include <pthread.h>
#include <time.h>
#include "trace.h"
#include "pp_resource.h"


static pthread_t main_thread;

 __attribute__((constructor))
static
void
ppb_core_constructor(void)
{
    main_thread = pthread_self();
}

static
void
ppb_core_add_ref_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__, resource);
    pp_resource_ref(resource);
}

static
void
ppb_core_release_resource(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__, resource);
    pp_resource_unref(resource);
}

static
PP_Time
ppb_core_get_time(void)
{
    trace_info("[PPB] {full} %s\n", __func__);
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static
PP_TimeTicks
ppb_core_get_time_ticks(void)
{
    trace_info("[PPB] {full} %s\n", __func__);
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static
void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result)
{
    trace_info("[PPB] {zilch} %s delay_in_milliseconds=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}, result=%d\n", __func__, delay_in_milliseconds, callback.func,
               callback.user_data, callback.flags, result);
}

static
PP_Bool
ppb_core_is_main_thread(void)
{
    trace_info("[PPB] {full} %s\n", __func__);
    return pthread_equal(main_thread, pthread_self());
}

const struct PPB_Core_1_0 ppb_core_interface_1_0 = {
    .AddRefResource = ppb_core_add_ref_resource,
    .ReleaseResource = ppb_core_release_resource,
    .GetTime = ppb_core_get_time,
    .GetTimeTicks = ppb_core_get_time_ticks,
    .CallOnMainThread = ppb_core_call_on_main_thread,
    .IsMainThread = ppb_core_is_main_thread
};
