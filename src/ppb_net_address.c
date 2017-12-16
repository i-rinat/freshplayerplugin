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

#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_net_address.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include <glib.h>
#include <netinet/in.h>
#include <string.h>

STATIC_ASSERT(sizeof(struct pp_net_address_s) <= LARGEST_RESOURCE_SIZE);

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
    (void)module;

    if (addr->size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in sai;
        char *s;
        uint8_t ip[4];

        STATIC_ASSERT(sizeof(ip) == sizeof(sai.sin_addr));

        memcpy(&sai, addr->data, sizeof(struct sockaddr_in));
        memcpy(ip, &sai.sin_addr, sizeof(ip));

        if (include_port)
            s = g_strdup_printf("%u.%u.%u.%u:%u", ip[0], ip[1], ip[2], ip[3], ntohs(sai.sin_port));
        else
            s = g_strdup_printf("%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);

        struct PP_Var var = ppb_var_var_from_utf8_z(s);
        g_free(s);
        return var;

    } else if (addr->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 sai6;
        char *s;
        uint16_t ip[8];

        STATIC_ASSERT(sizeof(ip) == sizeof(sai6.sin6_addr));

        memcpy(&sai6, addr->data, sizeof(struct sockaddr_in6));
        memcpy(ip, &sai6.sin6_addr, sizeof(ip));

        if (include_port) {
            s = g_strdup_printf("[%x:%x:%x:%x:%x:%x:%x:%x]:%u", ntohs(ip[0]), ntohs(ip[1]),
                                ntohs(ip[2]), ntohs(ip[3]), ntohs(ip[4]), ntohs(ip[5]),
                                ntohs(ip[6]), ntohs(ip[7]), ntohs(sai6.sin6_port));
        } else {
            s = g_strdup_printf("%x:%x:%x:%x:%x:%x:%x:%x", ntohs(ip[0]), ntohs(ip[1]), ntohs(ip[2]),
                                ntohs(ip[3]), ntohs(ip[4]), ntohs(ip[5]), ntohs(ip[6]),
                                ntohs(ip[7]));
        }

        struct PP_Var var = ppb_var_var_from_utf8_z(s);
        g_free(s);
        return var;

    } else {
        return PP_MakeUndefined();
    }
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
    if (is_ipv6) {
        struct sockaddr_in6 a;
        memset(&a, 0, sizeof(a));
        a.sin6_addr = in6addr_any;
        memcpy(addr->data, &a, sizeof(a));
        addr->size = sizeof(a);
    } else {
        // ipv4
        struct sockaddr_in a;
        memset(&a, 0, sizeof(a));
        a.sin_addr.s_addr = INADDR_ANY;
        memcpy(addr->data, &a, sizeof(a));
        addr->size = sizeof(a);
    }
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
    if (addr->size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 sai6;

        memcpy(&sai6, addr->data, sizeof(sai6));
        return ntohl(sai6.sin6_scope_id);
    }

    // it's IPv4, function should return 0
    return 0;
}

void
ppb_net_address_private_create_from_ipv4_address(const uint8_t ip[4], uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out)
{
    struct sockaddr_in sai = { .sin_port = htons(port) };

    memcpy(&sai.sin_addr, ip, sizeof(sai.sin_addr));
    memset(addr_out, 0, sizeof(struct PP_NetAddress_Private));

    addr_out->size = sizeof(sai);
    memcpy(addr_out->data, &sai, sizeof(sai));
}

void
ppb_net_address_private_create_from_ipv6_address(const uint8_t ip[16], uint32_t scope_id,
                                                 uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out)
{
    struct sockaddr_in6 sai6 = { .sin6_port = htons(port), .sin6_scope_id = htonl(scope_id) };

    memcpy(&sai6.sin6_addr, ip, sizeof(sai6.sin6_addr));
    memset(addr_out, 0, sizeof(struct PP_NetAddress_Private));

    addr_out->size = sizeof(sai6);
    memcpy(addr_out->data, &sai6, sizeof(sai6));
}

PP_Resource
ppb_net_address_create_from_ipv4_address(PP_Instance instance,
                                         const struct PP_NetAddress_IPv4 *ipv4_addr)
{
    if (!ipv4_addr) {
        trace_error("%s, ipv4_addr == NULL\n", __func__);
        return 0;
    }

    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource net_address = pp_resource_allocate(PP_RESOURCE_NET_ADDRESS, pp_i);
    struct pp_net_address_s *na = pp_resource_acquire(net_address, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, resource allocation failed\n", __func__);
        return 0;
    }

    ppb_net_address_private_create_from_ipv4_address(ipv4_addr->addr, ntohs(ipv4_addr->port),
                                                     &na->addr);

    pp_resource_release(net_address);
    return net_address;
}

PP_Resource
ppb_net_address_create_from_ipv6_address(PP_Instance instance,
                                         const struct PP_NetAddress_IPv6 *ipv6_addr)
{
    if (!ipv6_addr) {
        trace_error("%s, ipv6_addr == NULL\n", __func__);
        return 0;
    }

    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource net_address = pp_resource_allocate(PP_RESOURCE_NET_ADDRESS, pp_i);
    struct pp_net_address_s *na = pp_resource_acquire(net_address, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, resource allocation failed\n", __func__);
        return 0;
    }

    ppb_net_address_private_create_from_ipv6_address(ipv6_addr->addr, 0, ntohs(ipv6_addr->port),
                                                     &na->addr);

    pp_resource_release(net_address);
    return net_address;
}

static
void
ppb_net_address_destroy(void *ptr)
{
    // nothing to do
}

PP_Bool
ppb_net_address_is_net_address(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_NET_ADDRESS;
}

PP_NetAddress_Family
ppb_net_address_get_family(PP_Resource addr)
{
    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_NETADDRESS_FAMILY_UNSPECIFIED;
    }

    PP_NetAddress_Family family;

    if (na->addr.size == sizeof(struct sockaddr_in))
        family = PP_NETADDRESS_FAMILY_IPV4;
    else if (na->addr.size == sizeof(struct sockaddr_in6))
        family = PP_NETADDRESS_FAMILY_IPV6;
    else
        family = PP_NETADDRESS_FAMILY_UNSPECIFIED;

    pp_resource_release(addr);
    return family;
}

struct PP_Var
ppb_net_address_describe_as_string(PP_Resource addr, PP_Bool include_port)
{
    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var var = ppb_net_address_private_describe(0, &na->addr, include_port);

    pp_resource_release(addr);
    return var;
}

PP_Bool
ppb_net_address_describe_as_ipv4_address(PP_Resource addr, struct PP_NetAddress_IPv4 *ipv4_addr)
{
    if (!ipv4_addr) {
        trace_error("%s, ipv4_addr == NULL\n", __func__);
        return PP_FALSE;
    }

    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (na->addr.size != sizeof(struct sockaddr_in)) {
        // not ipv4
        pp_resource_release(addr);
        return PP_FALSE;
    }

    struct sockaddr_in *sai = (void *)na->addr.data;
    memcpy(ipv4_addr->addr, &sai->sin_addr, sizeof(sai->sin_addr));
    ipv4_addr->port = sai->sin_port;

    pp_resource_release(addr);
    return PP_TRUE;
}

PP_Bool
ppb_net_address_describe_as_ipv6_address(PP_Resource addr, struct PP_NetAddress_IPv6 *ipv6_addr)
{
    if (!ipv6_addr) {
        trace_error("%s, ipv6_addr == NULL\n", __func__);
        return PP_FALSE;
    }

    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (na->addr.size != sizeof(struct sockaddr_in6)) {
        // not ipv6
        pp_resource_release(addr);
        return PP_FALSE;
    }

    struct sockaddr_in6 *sai6 = (void *)na->addr.data;
    memcpy(ipv6_addr->addr, &sai6->sin6_addr, sizeof(sai6->sin6_addr));
    ipv6_addr->port = sai6->sin6_port;

    pp_resource_release(addr);
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {full} %s addr1={%p}, addr2={%p}\n", __func__+6, addr1, addr2);
    return ppb_net_address_private_are_equal(addr1, addr2);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                              const struct PP_NetAddress_Private *addr2)
{
    trace_info("[PPB] {full} %s addr1={%p}, addr2={%p}\n", __func__+6, addr1, addr2);
    return ppb_net_address_private_are_hosts_equal(addr1, addr2);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_net_address_private_describe(PP_Module module, const struct PP_NetAddress_Private *addr,
                                       PP_Bool include_port)
{
    trace_info("[PPB] {full} %s module=%d, addr={%p}, include_port=%u\n", __func__+6, module,
               addr, include_port);
    return ppb_net_address_private_describe(module, addr, include_port);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_private_replace_port(const struct PP_NetAddress_Private *src_addr,
                                           uint16_t port, struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {full} %s src_addr={%p}, port=%u\n", __func__+6, src_addr, port);
    return ppb_net_address_private_replace_port(src_addr, port, addr_out);
}

TRACE_WRAPPER
void
trace_ppb_net_address_private_get_any_address(PP_Bool is_ipv6, struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_net_address_private_get_any_address(is_ipv6, addr);
}

TRACE_WRAPPER
PP_NetAddressFamily_Private
trace_ppb_net_address_private_get_family(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s addr={%p}\n", __func__+6, addr);
    return ppb_net_address_private_get_family(addr);
}

TRACE_WRAPPER
uint16_t
trace_ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s addr={%p}\n", __func__+6, addr);
    return ppb_net_address_private_get_port(addr);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                          uint16_t address_size)
{
    trace_info("[PPB] {full} %s addr={%p}, address=%p, address_size=%u\n", __func__+6, addr,
               address, address_size);
    return ppb_net_address_private_get_address(addr, address, address_size);
}

TRACE_WRAPPER
uint32_t
trace_ppb_net_address_private_get_scope_id(const struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s addr=%p\n", __func__+6, addr);
    return ppb_net_address_private_get_scope_id(addr);
}

TRACE_WRAPPER
void
trace_ppb_net_address_private_create_from_ipv4_address(const uint8_t ip[4], uint16_t port,
                                                       struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {full} %s ip=%u.%u.%u.%u, port=%u\n", __func__+6, ip[0], ip[1], ip[2], ip[3],
               port);
    return ppb_net_address_private_create_from_ipv4_address(ip, port, addr_out);
}

TRACE_WRAPPER
void
trace_ppb_net_address_private_create_from_ipv6_address(const uint8_t ip[16], uint32_t scope_id,
                                                       uint16_t port,
                                                       struct PP_NetAddress_Private *addr_out)
{
    trace_info("[PPB] {full} %s ip=[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:"
               "%02x%02x], scope_id=%u, port=%u\n", __func__+6, ip[0], ip[1], ip[2], ip[3], ip[4],
               ip[5], ip[6], ip[7], ip[8], ip[9], ip[10], ip[11], ip[12], ip[13], ip[14], ip[15],
               scope_id, port);
    return ppb_net_address_private_create_from_ipv6_address(ip, scope_id, port, addr_out);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_net_address_create_from_ipv4_address(PP_Instance instance,
                                               const struct PP_NetAddress_IPv4 *ipv4_addr)
{
    gchar *s_ipv4_addr = trace_netaddress_ipv4_as_string(ipv4_addr);
    trace_info("[PPB] {full} %s instance=%d, ipv4_addr=%s\n", __func__+6, instance, s_ipv4_addr);
    g_free(s_ipv4_addr);
    return ppb_net_address_create_from_ipv4_address(instance, ipv4_addr);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_net_address_create_from_ipv6_address(PP_Instance instance,
                                               const struct PP_NetAddress_IPv6 *ipv6_addr)
{
    gchar *s_ipv6_addr = trace_netaddress_ipv6_as_string(ipv6_addr);
    trace_info("[PPB] {full} %s instance=%d, ipv6_addr=%s\n", __func__+6, instance, s_ipv6_addr);
    g_free(s_ipv6_addr);
    return ppb_net_address_create_from_ipv6_address(instance, ipv6_addr);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_is_net_address(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_net_address_is_net_address(resource);
}

TRACE_WRAPPER
PP_NetAddress_Family
trace_ppb_net_address_get_family(PP_Resource addr)
{
    trace_info("[PPB] {full} %s addr=%d\n", __func__+6, addr);
    return ppb_net_address_get_family(addr);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_net_address_describe_as_string(PP_Resource addr, PP_Bool include_port)
{
    trace_info("[PPB] {full} %s addr=%d, include_port=%u\n", __func__+6, addr, include_port);
    return ppb_net_address_describe_as_string(addr, include_port);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_describe_as_ipv4_address(PP_Resource addr,
                                               struct PP_NetAddress_IPv4 *ipv4_addr)
{
    trace_info("[PPB] {full} %s addr=%d\n", __func__+6, addr);
    return ppb_net_address_describe_as_ipv4_address(addr, ipv4_addr);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_net_address_describe_as_ipv6_address(PP_Resource addr,
                                               struct PP_NetAddress_IPv6 *ipv6_addr)
{
    trace_info("[PPB] {full} %s addr=%d\n", __func__+6, addr);
    return ppb_net_address_describe_as_ipv6_address(addr, ipv6_addr);
}


const struct PPB_NetAddress_Private_1_1 ppb_net_address_private_interface_1_1 = {
    .AreEqual =              TWRAPF(ppb_net_address_private_are_equal),
    .AreHostsEqual =         TWRAPF(ppb_net_address_private_are_hosts_equal),
    .Describe =              TWRAPF(ppb_net_address_private_describe),
    .ReplacePort =           TWRAPF(ppb_net_address_private_replace_port),
    .GetAnyAddress =         TWRAPF(ppb_net_address_private_get_any_address),
    .GetFamily =             TWRAPF(ppb_net_address_private_get_family),
    .GetPort =               TWRAPF(ppb_net_address_private_get_port),
    .GetAddress =            TWRAPF(ppb_net_address_private_get_address),
    .GetScopeID =            TWRAPF(ppb_net_address_private_get_scope_id),
    .CreateFromIPv4Address = TWRAPF(ppb_net_address_private_create_from_ipv4_address),
    .CreateFromIPv6Address = TWRAPF(ppb_net_address_private_create_from_ipv6_address),
};

const struct PPB_NetAddress_1_0 ppb_net_address_interface_1_0 = {
    .CreateFromIPv4Address = TWRAPF(ppb_net_address_create_from_ipv4_address),
    .CreateFromIPv6Address = TWRAPF(ppb_net_address_create_from_ipv6_address),
    .IsNetAddress =          TWRAPF(ppb_net_address_is_net_address),
    .GetFamily =             TWRAPF(ppb_net_address_get_family),
    .DescribeAsString =      TWRAPF(ppb_net_address_describe_as_string),
    .DescribeAsIPv4Address = TWRAPF(ppb_net_address_describe_as_ipv4_address),
    .DescribeAsIPv6Address = TWRAPF(ppb_net_address_describe_as_ipv6_address),
};

static
void
__attribute__((constructor))
constructor_ppb_net_address(void)
{
    register_interface(PPB_NETADDRESS_PRIVATE_INTERFACE_1_1,
                       &ppb_net_address_private_interface_1_1);
    register_interface(PPB_NETADDRESS_INTERFACE_1_0, &ppb_net_address_interface_1_0);
    register_resource(PP_RESOURCE_NET_ADDRESS, ppb_net_address_destroy);
}
