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

#ifndef FPP_PPB_HOST_RESOLVER_H
#define FPP_PPB_HOST_RESOLVER_H

#include <ppapi/c/private/ppb_host_resolver_private.h>
#include <ppapi/c/ppb_host_resolver.h>


PP_Resource
ppb_host_resolver_create(PP_Instance instance);

PP_Bool
ppb_host_resolver_is_host_resolver(PP_Resource resource);

int32_t
ppb_host_resolver_resolve(PP_Resource host_resolver, const char *host, uint16_t port,
                          const struct PP_HostResolver_Private_Hint *hint,
                          struct PP_CompletionCallback callback);

int32_t
ppb_host_resolver_resolve_1_0(PP_Resource host_resolver, const char *host, uint16_t port,
                              const struct PP_HostResolver_Hint *hint,
                              struct PP_CompletionCallback callback);

struct PP_Var
ppb_host_resolver_get_canonical_name(PP_Resource host_resolver);

uint32_t
ppb_host_resolver_get_size(PP_Resource host_resolver);

uint32_t
ppb_host_resolver_get_net_address_count(PP_Resource host_resolver);

PP_Bool
ppb_host_resolver_get_net_address(PP_Resource host_resolver, uint32_t index,
                                  struct PP_NetAddress_Private *addr);

PP_Resource
ppb_host_resolver_get_net_address_1_0(PP_Resource host_resolver, uint32_t index);

#endif // FPP_PPB_HOST_RESOLVER_H
