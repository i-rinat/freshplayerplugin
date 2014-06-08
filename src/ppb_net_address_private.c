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
#include <string.h>
#include <netinet/in.h>
#include "trace.h"


PP_Bool
ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                  const struct PP_NetAddress_Private *addr2)
{
    int ret = (addr1->size == addr2->size) && (memcmp(addr1->data, addr2->data, addr1->size) == 0);
    return ret ? PP_TRUE : PP_FALSE;
}

PP_Bool
ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2)
{
    if (addr1->size != addr2->size)
        return PP_FALSE;

    if (addr1->size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in *a1 = (struct sockaddr_in *)addr1->data;
        struct sockaddr_in *a2 = (struct sockaddr_in *)addr2->data;

        int res = memcmp(&a1->sin_addr, &a2->sin_addr, sizeof(a1->sin_addr));
        return res == 0 ? PP_TRUE : PP_FALSE;
    } else if (addr1->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *a1 = (struct sockaddr_in6 *)addr1->data;
        struct sockaddr_in6 *a2 = (struct sockaddr_in6 *)addr2->data;

        int res = memcmp(&a1->sin6_addr, &a2->sin6_addr, sizeof(a1->sin6_addr));
        return res == 0 ? PP_TRUE : PP_FALSE;
    } else {
        return PP_FALSE;
    }
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
    *addr_out = *src_addr;

    if (addr_out->size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in *a = (struct sockaddr_in *)addr_out->data;
        a->sin_port = htons(port);
        return PP_TRUE;
    } else if (addr_out->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *a = (struct sockaddr_in6 *)addr_out->data;
        a->sin6_port = htons(port);
        return PP_TRUE;
    } else {
        return PP_FALSE;
    }
}

void
ppb_net_address_private_get_any_address(PP_Bool is_ipv6, struct PP_NetAddress_Private *addr)
{
}

PP_NetAddressFamily_Private
ppb_net_address_private_get_family(const struct PP_NetAddress_Private *addr)
{
    if (addr->size == sizeof(struct sockaddr_in))
        return PP_NETADDRESSFAMILY_PRIVATE_IPV4;
    else if (addr->size == sizeof(struct sockaddr_in6))
        return PP_NETADDRESSFAMILY_PRIVATE_IPV6;
    else
        return PP_NETADDRESSFAMILY_PRIVATE_UNSPECIFIED;
}

uint16_t
ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr)
{
    if (addr->size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in *a = (struct sockaddr_in *)addr->data;
        return ntohs(a->sin_port);
    } else if (addr->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *a = (struct sockaddr_in6 *)addr->data;
        return ntohs(a->sin6_port);
    } else {
        return 0;
    }
}

PP_Bool
ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                    uint16_t address_size)
{
    if (addr->size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in *a = (struct sockaddr_in *)addr->data;
        if (address_size >= sizeof(a->sin_addr)) {
            memcpy(address, &a->sin_addr, sizeof(a->sin_addr));
            return PP_TRUE;
        }
    } else if (addr->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *a = (struct sockaddr_in6 *)addr->data;
        if (address_size >= sizeof(a->sin6_addr)) {
            memcpy(address, &a->sin6_addr, sizeof(a->sin6_addr));
            return PP_TRUE;
        }
    }

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


#ifndef NDEBUG
// trace wrappers
static
PP_Bool
trace_ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {full} %s addr1={%p}, addr2={%p}\n", __func__+6, addr1, addr2);
    return ppb_net_address_private_are_equal(addr1, addr2);
}

static
PP_Bool
trace_ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                              const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {full} %s addr1={%p}, addr2={%p}\n", __func__+6, addr1, addr2);
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
    trace_info("[PPB] {full} %s src_addr={%p}, port=%u\n", __func__+6, src_addr, port);
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
    trace_info("[PPB] {full} %s addr={%p}\n", __func__+6, addr);
    return ppb_net_address_private_get_family(addr);
}

static
uint16_t
trace_ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s addr={%p}\n", __func__+6, addr);
    return ppb_net_address_private_get_port(addr);
}

static
PP_Bool
trace_ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                          uint16_t address_size)
{
    trace_info("[PPB] {full} %s addr={%p}, address=%p, address_size=%u\n", __func__+6, addr,
               address, address_size);
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
#endif // NDEBUG


const struct PPB_NetAddress_Private_1_1 ppb_net_address_private_interface_1_1 = {
    .AreEqual =         TWRAP(ppb_net_address_private_are_equal),
    .AreHostsEqual =    TWRAP(ppb_net_address_private_are_hosts_equal),
    .Describe =         TWRAP(ppb_net_address_private_describe),
    .ReplacePort =      TWRAP(ppb_net_address_private_replace_port),
    .GetAnyAddress =    TWRAP(ppb_net_address_private_get_any_address),
    .GetFamily =        TWRAP(ppb_net_address_private_get_family),
    .GetPort =          TWRAP(ppb_net_address_private_get_port),
    .GetAddress =       TWRAP(ppb_net_address_private_get_address),
    .GetScopeID =       TWRAP(ppb_net_address_private_get_scope_id),
    .CreateFromIPv4Address = TWRAP(ppb_net_address_private_create_from_ipv4_address),
    .CreateFromIPv6Address = TWRAP(ppb_net_address_private_create_from_ipv6_address),
};
