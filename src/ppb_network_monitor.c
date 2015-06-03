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

#include "ppb_network_monitor.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "pp_resource.h"
#include "trace.h"
#include "tables.h"
#include "pp_interface.h"

PP_Resource
ppb_network_monitor_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource network_monitor = pp_resource_allocate(PP_RESOURCE_NETWORK_MONITOR, pp_i);
    return network_monitor;
}

static
void
ppb_network_monitor_destroy(void *ptr)
{
}

int32_t
ppb_network_monitor_update_network_list(PP_Resource network_monitor, PP_Resource *network_list,
                                        struct PP_CompletionCallback callback)
{
    return PP_ERROR_NOACCESS;
}

PP_Bool
ppb_network_monitor_is_network_monitor(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_NETWORK_MONITOR;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_network_monitor_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_network_monitor_create(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_network_monitor_update_network_list(PP_Resource network_monitor,
                                              PP_Resource *network_list,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_network_monitor_update_network_list(network_monitor, network_list, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_network_monitor_is_network_monitor(PP_Resource resource)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_network_monitor_is_network_monitor(resource);
}


const struct PPB_NetworkMonitor_1_0 ppb_network_monitor_interface_1_0 = {
    .Create =               TWRAPF(ppb_network_monitor_create),
    .UpdateNetworkList =    TWRAPZ(ppb_network_monitor_update_network_list),
    .IsNetworkMonitor =     TWRAPF(ppb_network_monitor_is_network_monitor),
};

static
void
__attribute__((constructor))
constructor_ppb_network_monitor(void)
{
    register_interface(PPB_NETWORKMONITOR_INTERFACE_1_0, &ppb_network_monitor_interface_1_0);
    register_resource(PP_RESOURCE_NETWORK_MONITOR, ppb_network_monitor_destroy);
}
