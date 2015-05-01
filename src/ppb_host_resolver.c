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

#include "ppb_host_resolver.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include <ppapi/c/pp_errors.h>



PP_Resource
ppb_host_resolver_create(PP_Instance instance)
{
    return 0;
}

void
ppb_host_resolver_destroy(void *ptr)
{
}

PP_Bool
ppb_host_resolver_is_host_resolver(PP_Resource resource)
{
    return PP_FALSE;
}

int32_t
ppb_host_resolver_resolve(PP_Resource host_resolver, const char *host, uint16_t port,
                          const struct PP_HostResolver_Private_Hint *hint,
                          struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

struct PP_Var
ppb_host_resolver_get_canonical_name(PP_Resource host_resolver)
{
    return PP_MakeUndefined();
}

uint32_t
ppb_host_resolver_get_size(PP_Resource host_resolver)
{
    return 0;
}

PP_Bool
ppb_host_resolver_get_net_address(PP_Resource host_resolver, uint32_t index,
                                  struct PP_NetAddress_Private *addr)
{
    return PP_FALSE;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_host_resolver_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_host_resolver_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_host_resolver_is_host_resolver(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_host_resolver_is_host_resolver(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_host_resolver_resolve(PP_Resource host_resolver, const char *host, uint16_t port,
                                const struct PP_HostResolver_Private_Hint *hint,
                                struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s host_resolver=%d, host=%s, port=%u, hint=%p, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, host_resolver, host, (unsigned int)port,
               hint, callback.func, callback.user_data, callback.flags);
    return ppb_host_resolver_resolve(host_resolver, host, port, hint, callback);
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
    trace_info("[PPB] {zilch} %s host_resolver=%d\n", __func__+6, host_resolver);
    return ppb_host_resolver_get_size(host_resolver);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_host_resolver_get_net_address(PP_Resource host_resolver, uint32_t index,
                                        struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s host_resolver=%d, index=%u\n", __func__+6, host_resolver, index);
    return ppb_host_resolver_get_net_address(host_resolver, index, addr);
}


const struct PPB_HostResolver_Private_0_1 ppb_host_resolver_private_interface_0_1 = {
    .Create =           TWRAPZ(ppb_host_resolver_create),
    .IsHostResolver =   TWRAPZ(ppb_host_resolver_is_host_resolver),
    .Resolve =          TWRAPZ(ppb_host_resolver_resolve),
    .GetCanonicalName = TWRAPZ(ppb_host_resolver_get_canonical_name),
    .GetSize =          TWRAPZ(ppb_host_resolver_get_size),
    .GetNetAddress =    TWRAPZ(ppb_host_resolver_get_net_address),
};
