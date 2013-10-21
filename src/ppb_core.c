#include <ppapi/c/ppb_core.h>
#include <stddef.h>
#include "trace.h"

static
void
ppb_core_add_ref_resource(PP_Resource resource)
{
    trace_info("{zilch} %s resource=%d\n", __func__, resource);
}

static
void
ppb_core_release_resource(PP_Resource resource)
{
    trace_info("{zilch} %s resource=%d\n", __func__, resource);
}

static
PP_Time
ppb_core_get_time(void)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
PP_TimeTicks
ppb_core_get_time_ticks(void)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result)
{
    trace_info("{zilch} %s delay_in_milliseconds=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}, result=%d\n", __func__, delay_in_milliseconds, callback.func,
               callback.user_data, callback.flags, result);
}

static
PP_Bool
ppb_core_is_main_thread(void)
{
    trace_info("{fake} %s\n", __func__);
    return PP_FALSE;
}

const struct PPB_Core_1_0 ppb_core_interface_1_0 = {
    .AddRefResource = ppb_core_add_ref_resource,
    .ReleaseResource = ppb_core_release_resource,
    .GetTime = ppb_core_get_time,
    .GetTimeTicks = ppb_core_get_time_ticks,
    .CallOnMainThread = ppb_core_call_on_main_thread,
    .IsMainThread = ppb_core_is_main_thread
};
