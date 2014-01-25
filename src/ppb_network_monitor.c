#include <ppapi/c/ppb_network_monitor.h>
#include <stddef.h>
#include "trace.h"

PP_Resource
ppb_network_monitor_create(PP_Instance instance)
{
    return 0;
}

int32_t
ppb_network_monitor_update_network_list(PP_Resource network_monitor, PP_Resource* network_list,
                                     struct PP_CompletionCallback callback)
{
    return 0;
}

PP_Bool
ppb_network_monitor_is_network_monitor(PP_Resource resource)
{
    return PP_TRUE;
}

// trace wrappers
static
PP_Resource
trace_ppb_network_monitor_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_network_monitor_create(instance);
}

static
int32_t
trace_ppb_network_monitor_update_network_list(PP_Resource network_monitor,
                                              PP_Resource* network_list,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_network_monitor_update_network_list(network_monitor, network_list, callback);
}

static
PP_Bool
trace_ppb_network_monitor_is_network_monitor(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_network_monitor_is_network_monitor(resource);
}


const struct PPB_NetworkMonitor_1_0 ppb_network_monitor_interface_1_0 = {
    .Create =               trace_ppb_network_monitor_create,
    .UpdateNetworkList =    trace_ppb_network_monitor_update_network_list,
    .IsNetworkMonitor =     trace_ppb_network_monitor_is_network_monitor,
};
