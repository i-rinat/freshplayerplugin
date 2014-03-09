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

#include "ppb_net_address_private.h"
#include <stdlib.h>
#include "trace.h"


PP_Bool
ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                  const struct PP_NetAddress_Private *addr2)
{
    return PP_FALSE;
}

PP_Bool
ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2)
{
    return PP_FALSE;
}

struct PP_Var
ppb_net_address_private_describe(PP_Module module, const struct PP_NetAddress_Private *addr,
                                 PP_Bool include_port)
{
    return PP_MakeUndefined();
}

PP_Bool
ppb_net_address_private_replace_port(const struct PP_NetAddress_Private *src_addr, uint16_t port,
                                     struct PP_NetAddress_Private *addr_out)
{
    return PP_FALSE;
}

void
ppb_net_address_private_get_any_address(PP_Bool is_ipv6, struct PP_NetAddress_Private *addr)
{
}

PP_NetAddressFamily_Private
ppb_net_address_private_get_family(const struct PP_NetAddress_Private *addr)
{
    return 0;
}

uint16_t
ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr)
{
    return 0;
}

PP_Bool
ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                    uint16_t address_size)
{
    return PP_FALSE;
}

uint32_t
ppb_net_address_private_get_scope_id(const struct PP_NetAddress_Private *addr)
{
    return 0;
}

void
ppb_net_address_private_create_from_ipv4_address(const uint8_t ip[4], uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out)
{
}

void
ppb_net_address_private_create_from_ipv6_address(const uint8_t ip[16], uint32_t scope_id,
                                                 uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out)
{
}


// trace wrappers
static
PP_Bool
trace_ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_are_equal(addr1, addr2);
}

static
PP_Bool
trace_ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                              const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_are_hosts_equal(addr1, addr2);
}

static
struct PP_Var
trace_ppb_net_address_private_describe(PP_Module module, const struct PP_NetAddress_Private *addr,
                                       PP_Bool include_port)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_describe(module, addr, include_port);
}

static
PP_Bool
trace_ppb_net_address_private_replace_port(const struct PP_NetAddress_Private *src_addr,
                                           uint16_t port, struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_replace_port(src_addr, port, addr_out);
}

static
void
trace_ppb_net_address_private_get_any_address(PP_Bool is_ipv6, struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_get_any_address(is_ipv6, addr);
}

static
PP_NetAddressFamily_Private
trace_ppb_net_address_private_get_family(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_get_family(addr);
}

static
uint16_t
trace_ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_get_port(addr);
}

static
PP_Bool
trace_ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                          uint16_t address_size)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_get_address(addr, address, address_size);
}

static
uint32_t
trace_ppb_net_address_private_get_scope_id(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_get_scope_id(addr);
}

static
void
trace_ppb_net_address_private_create_from_ipv4_address(const uint8_t ip[4], uint16_t port,
                                                       struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_create_from_ipv4_address(ip, port, addr_out);
}

static
void
trace_ppb_net_address_private_create_from_ipv6_address(const uint8_t ip[16], uint32_t scope_id,
                                                       uint16_t port,
                                                       struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_net_address_private_create_from_ipv6_address(ip, scope_id, port, addr_out);
}


const struct PPB_NetAddress_Private_1_1 ppb_net_address_private_interface_1_1 = {
    .AreEqual =         trace_ppb_net_address_private_are_equal,
    .AreHostsEqual =    trace_ppb_net_address_private_are_hosts_equal,
    .Describe =         trace_ppb_net_address_private_describe,
    .ReplacePort =      trace_ppb_net_address_private_replace_port,
    .GetAnyAddress =    trace_ppb_net_address_private_get_any_address,
    .GetFamily =        trace_ppb_net_address_private_get_family,
    .GetPort =          trace_ppb_net_address_private_get_port,
    .GetAddress =       trace_ppb_net_address_private_get_address,
    .GetScopeID =       trace_ppb_net_address_private_get_scope_id,
    .CreateFromIPv4Address = trace_ppb_net_address_private_create_from_ipv4_address,
    .CreateFromIPv6Address = trace_ppb_net_address_private_create_from_ipv6_address,
};
