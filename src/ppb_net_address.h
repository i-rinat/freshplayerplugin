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

#ifndef FPP_PPB_NET_ADDRESS_H
#define FPP_PPB_NET_ADDRESS_H

#include <ppapi/c/private/ppb_net_address_private.h>
#include <ppapi/c/ppb_net_address.h>


PP_Bool
ppb_net_address_private_are_equal(const struct PP_NetAddress_Private *addr1,
                                  const struct PP_NetAddress_Private *addr2);

PP_Bool
ppb_net_address_private_are_hosts_equal(const struct PP_NetAddress_Private *addr1,
                                        const struct PP_NetAddress_Private *addr2);

struct PP_Var
ppb_net_address_private_describe(PP_Module module, const struct PP_NetAddress_Private *addr,
                                 PP_Bool include_port);

PP_Bool
ppb_net_address_private_replace_port(const struct PP_NetAddress_Private *src_addr, uint16_t port,
                                     struct PP_NetAddress_Private *addr_out);

void
ppb_net_address_private_get_any_address(PP_Bool is_ipv6, struct PP_NetAddress_Private *addr);

PP_NetAddressFamily_Private
ppb_net_address_private_get_family(const struct PP_NetAddress_Private *addr);

uint16_t
ppb_net_address_private_get_port(const struct PP_NetAddress_Private *addr);

PP_Bool
ppb_net_address_private_get_address(const struct PP_NetAddress_Private *addr, void *address,
                                    uint16_t address_size);

uint32_t
ppb_net_address_private_get_scope_id(const struct PP_NetAddress_Private *addr);

void
ppb_net_address_private_create_from_ipv4_address(const uint8_t ip[4], uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out);

void
ppb_net_address_private_create_from_ipv6_address(const uint8_t ip[16], uint32_t scope_id,
                                                 uint16_t port,
                                                 struct PP_NetAddress_Private *addr_out);

PP_Resource
ppb_net_address_create_from_ipv4_address(PP_Instance instance,
                                         const struct PP_NetAddress_IPv4 *ipv4_addr);

PP_Resource
ppb_net_address_create_from_ipv6_address(PP_Instance instance,
                                         const struct PP_NetAddress_IPv6 *ipv6_addr);

PP_Bool
ppb_net_address_is_net_address(PP_Resource resource);

PP_NetAddress_Family
ppb_net_address_get_family(PP_Resource addr);

struct PP_Var
ppb_net_address_describe_as_string(PP_Resource addr, PP_Bool include_port);

PP_Bool
ppb_net_address_describe_as_ipv4_address(PP_Resource addr, struct PP_NetAddress_IPv4 *ipv4_addr);

PP_Bool
ppb_net_address_describe_as_ipv6_address(PP_Resource addr, struct PP_NetAddress_IPv6 *ipv6_addr);


#endif // FPP_PPB_NET_ADDRESS_H
