/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "async_network.h"
#include "pp_interface.h"
#include "ppb_host_resolver.h"
#include "ppb_message_loop.h"
#include "ppb_net_address.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "utils.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include <string.h>

STATIC_ASSERT(sizeof(struct pp_host_resolver_s) <= LARGEST_RESOURCE_SIZE);

PP_Resource
ppb_host_resolver_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource host_resolver = pp_resource_allocate(PP_RESOURCE_HOST_RESOLVER, pp_i);

    return host_resolver;
}

static
void
ppb_host_resolver_destroy(void *ptr)
{
    struct pp_host_resolver_s *hr = ptr;

    free_and_nullify(hr->addrs);
}

PP_Bool
ppb_host_resolver_is_host_resolver(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_HOST_RESOLVER;
}

int32_t
ppb_host_resolver_resolve(PP_Resource host_resolver, const char *host, uint16_t port,
                          const struct PP_HostResolver_Private_Hint *hint,
                          struct PP_CompletionCallback callback)
{
    struct pp_host_resolver_s *hr = pp_resource_acquire(host_resolver, PP_RESOURCE_HOST_RESOLVER);
    if (!hr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    hr->host = nullsafe_strdup(host);

    struct async_network_task_s *task = async_network_task_create();

    task->type =        ASYNC_NETWORK_HOST_RESOLVE;
    task->resource =    host_resolver;
    task->host =        nullsafe_strdup(host);
    task->port =        port;
    task->callback =    callback;
    task->callback_ml = ppb_message_loop_get_current();

    pp_resource_release(host_resolver);
    async_network_task_push(task);

    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_host_resolver_resolve_1_0(PP_Resource host_resolver, const char *host, uint16_t port,
                              const struct PP_HostResolver_Hint *hint,
                              struct PP_CompletionCallback callback)
{
    // TODO: use 'hint' somehow?
    return ppb_host_resolver_resolve(host_resolver, host, port, NULL, callback);
}

struct PP_Var
ppb_host_resolver_get_canonical_name(PP_Resource host_resolver)
{
    // TODO: implement
    struct pp_host_resolver_s *hr = pp_resource_acquire(host_resolver, PP_RESOURCE_HOST_RESOLVER);
    if (!hr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var var = ppb_var_var_from_utf8_z(hr->host);
    pp_resource_release(host_resolver);
    return var;
}

uint32_t
ppb_host_resolver_get_size(PP_Resource host_resolver)
{
    struct pp_host_resolver_s *hr = pp_resource_acquire(host_resolver, PP_RESOURCE_HOST_RESOLVER);
    if (!hr) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }

    uint32_t count = hr->addr_count;

    pp_resource_release(host_resolver);
    return count;
}

uint32_t
ppb_host_resolver_get_net_address_count(PP_Resource host_resolver)
{
    return ppb_host_resolver_get_size(host_resolver);
}

PP_Bool
ppb_host_resolver_get_net_address(PP_Resource host_resolver, uint32_t index,
                                  struct PP_NetAddress_Private *addr)
{
    struct pp_host_resolver_s *hr = pp_resource_acquire(host_resolver, PP_RESOURCE_HOST_RESOLVER);
    if (!hr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (index >= hr->addr_count) {
        pp_resource_release(host_resolver);
        return PP_FALSE;
    }

    memcpy(addr, &hr->addrs[index], sizeof(struct PP_NetAddress_Private));

    pp_resource_release(host_resolver);
    return PP_TRUE;
}

PP_Resource
ppb_host_resolver_get_net_address_1_0(PP_Resource host_resolver, uint32_t index)
{
    PP_Resource net_address = 0;
    struct pp_host_resolver_s *hr = pp_resource_acquire(host_resolver, PP_RESOURCE_HOST_RESOLVER);
    if (!hr) {
        trace_error("%s, bad resource\n", __func__);
        goto err_1;
    }

    if (index >= hr->addr_count)
        goto err_2;

    net_address = pp_resource_allocate(PP_RESOURCE_NET_ADDRESS, hr->instance);
    struct pp_net_address_s *na = pp_resource_acquire(net_address, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, resource allocation failed\n", __func__);
        goto err_2;
    }

    memcpy(&na->addr, &hr->addrs[index], sizeof(struct PP_NetAddress_Private));
    pp_resource_release(net_address);

err_2:
    pp_resource_release(host_resolver);
err_1:
    return net_address;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_host_resolver_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_host_resolver_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_host_resolver_is_host_resolver(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_host_resolver_is_host_resolver(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_host_resolver_resolve(PP_Resource host_resolver, const char *host, uint16_t port,
                                const struct PP_HostResolver_Private_Hint *hint,
                                struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s host_resolver=%d, host=%s, port=%u, hint=%p, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, host_resolver, host, (unsigned int)port,
               hint, callback.func, callback.user_data, callback.flags);
    return ppb_host_resolver_resolve(host_resolver, host, port, hint, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_host_resolver_resolve_1_0(PP_Resource host_resolver, const char *host, uint16_t port,
                                    const struct PP_HostResolver_Hint *hint,
                                    struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s host_resolver=%d, host=%s, port=%u, hint=%p, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, host_resolver, host, (unsigned int)port,
               hint, callback.func, callback.user_data, callback.flags);
    return ppb_host_resolver_resolve_1_0(host_resolver, host, port, hint, callback);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_host_resolver_get_canonical_name(PP_Resource host_resolver)
{
    trace_info("[PPB] {zilch} %s host_resolver=%d\n", __func__+6, host_resolver);
    return ppb_host_resolver_get_canonical_name(host_resolver);
}

TRACE_WRAPPER
uint32_t
trace_ppb_host_resolver_get_size(PP_Resource host_resolver)
{
    trace_info("[PPB] {full} %s host_resolver=%d\n", __func__+6, host_resolver);
    return ppb_host_resolver_get_size(host_resolver);
}

TRACE_WRAPPER
uint32_t
trace_ppb_host_resolver_get_net_address_count(PP_Resource host_resolver)
{
    trace_info("[PPB] {full} %s host_resolver=%d\n", __func__+6, host_resolver);
    return ppb_host_resolver_get_net_address_count(host_resolver);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_host_resolver_get_net_address(PP_Resource host_resolver, uint32_t index,
                                        struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s host_resolver=%d, index=%u\n", __func__+6, host_resolver, index);
    return ppb_host_resolver_get_net_address(host_resolver, index, addr);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_host_resolver_get_net_address_1_0(PP_Resource host_resolver, uint32_t index)
{
    trace_info("[PPB] {full} %s host_resolver=%d, index=%u\n", __func__+6, host_resolver, index);
    return ppb_host_resolver_get_net_address_1_0(host_resolver, index);
}


const struct PPB_HostResolver_Private_0_1 ppb_host_resolver_private_interface_0_1 = {
    .Create =           TWRAPF(ppb_host_resolver_create),
    .IsHostResolver =   TWRAPF(ppb_host_resolver_is_host_resolver),
    .Resolve =          TWRAPF(ppb_host_resolver_resolve),
    .GetCanonicalName = TWRAPZ(ppb_host_resolver_get_canonical_name),
    .GetSize =          TWRAPF(ppb_host_resolver_get_size),
    .GetNetAddress =    TWRAPF(ppb_host_resolver_get_net_address),
};

const struct PPB_HostResolver_1_0 ppb_host_resolver_interface_1_0 = {
    .Create =               TWRAPF(ppb_host_resolver_create),
    .IsHostResolver =       TWRAPF(ppb_host_resolver_is_host_resolver),
    .Resolve =              TWRAPF(ppb_host_resolver_resolve_1_0),
    .GetCanonicalName =     TWRAPZ(ppb_host_resolver_get_canonical_name),
    .GetNetAddressCount =   TWRAPF(ppb_host_resolver_get_net_address_count),
    .GetNetAddress =        TWRAPF(ppb_host_resolver_get_net_address_1_0),
};

static
void
__attribute__((constructor))
constructor_ppb_host_resolver(void)
{
    register_interface(PPB_HOSTRESOLVER_PRIVATE_INTERFACE_0_1,
                       &ppb_host_resolver_private_interface_0_1);
    register_interface(PPB_HOSTRESOLVER_INTERFACE_1_0, &ppb_host_resolver_interface_1_0);
    register_resource(PP_RESOURCE_HOST_RESOLVER, ppb_host_resolver_destroy);
}
