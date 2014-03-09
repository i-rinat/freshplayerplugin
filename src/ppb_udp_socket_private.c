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

#include "ppb_udp_socket_private.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_udp_socket_private_create(PP_Instance instance_id)
{
    return 0;
}

PP_Bool
ppb_udp_socket_private_is_udp_socket(PP_Resource resource_id)
{
    return PP_TRUE;
}

int32_t
ppb_udp_socket_private_set_socket_feature(PP_Resource udp_socket, PP_UDPSocketFeature_Private name,
                                          struct PP_Var value)
{
    return -1;
}

int32_t
ppb_udp_socket_private_bind(PP_Resource udp_socket, const struct PP_NetAddress_Private *addr,
                            struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Bool
ppb_udp_socket_private_get_bound_address(PP_Resource udp_socket, struct PP_NetAddress_Private *addr)
{
    return PP_FALSE;
}

int32_t
ppb_udp_socket_private_recv_from(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                                 struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Bool
ppb_udp_socket_private_get_recv_from_address(PP_Resource udp_socket,
                                             struct PP_NetAddress_Private *addr)
{
    return PP_FALSE;
}

int32_t
ppb_udp_socket_private_send_to(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                               const struct PP_NetAddress_Private *addr,
                               struct PP_CompletionCallback callback)
{
    return -1;
}

void
ppb_udp_socket_private_close(PP_Resource udp_socket)
{
}


// trace wrappers
static
PP_Resource
trace_ppb_udp_socket_private_create(PP_Instance instance_id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_create(instance_id);
}

static
PP_Bool
trace_ppb_udp_socket_private_is_udp_socket(PP_Resource resource_id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_is_udp_socket(resource_id);
}

static
int32_t
trace_ppb_udp_socket_private_set_socket_feature(PP_Resource udp_socket,
                                                PP_UDPSocketFeature_Private name,
                                                struct PP_Var value)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_set_socket_feature(udp_socket, name, value);
}

static
int32_t
trace_ppb_udp_socket_private_bind(PP_Resource udp_socket, const struct PP_NetAddress_Private *addr,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_bind(udp_socket, addr, callback);
}

static
PP_Bool
trace_ppb_udp_socket_private_get_bound_address(PP_Resource udp_socket,
                                               struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_get_bound_address(udp_socket, addr);
}

static
int32_t
trace_ppb_udp_socket_private_recv_from(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                                       struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_recv_from(udp_socket, buffer, num_bytes, callback);
}

static
PP_Bool
trace_ppb_udp_socket_private_get_recv_from_address(PP_Resource udp_socket,
                                                   struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_get_recv_from_address(udp_socket, addr);
}

static
int32_t
trace_ppb_udp_socket_private_send_to(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                                     const struct PP_NetAddress_Private *addr,
                                     struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_send_to(udp_socket, buffer, num_bytes, addr, callback);
}

static
void
trace_ppb_udp_socket_private_close(PP_Resource udp_socket)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_udp_socket_private_close(udp_socket);
}


const struct PPB_UDPSocket_Private_0_4 ppb_udp_socket_private_interface_0_4 = {
    .Create =               trace_ppb_udp_socket_private_create,
    .IsUDPSocket =          trace_ppb_udp_socket_private_is_udp_socket,
    .SetSocketFeature =     trace_ppb_udp_socket_private_set_socket_feature,
    .Bind =                 trace_ppb_udp_socket_private_bind,
    .GetBoundAddress =      trace_ppb_udp_socket_private_get_bound_address,
    .RecvFrom =             trace_ppb_udp_socket_private_recv_from,
    .GetRecvFromAddress =   trace_ppb_udp_socket_private_get_recv_from_address,
    .SendTo =               trace_ppb_udp_socket_private_send_to,
    .Close =                trace_ppb_udp_socket_private_close,
};
