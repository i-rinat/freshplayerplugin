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

#include "ppb_tcp_socket_private.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_tcp_socket_private_create(PP_Instance instance)
{
    return 0;
}

PP_Bool
ppb_tcp_socket_private_is_tcp_socket(PP_Resource resource)
{
    return PP_TRUE;
}

int32_t
ppb_tcp_socket_private_connect(PP_Resource tcp_socket, const char *host, uint16_t port,
                               struct PP_CompletionCallback callback)
{
    return -1;
}

int32_t
ppb_tcp_socket_private_connect_with_net_address(PP_Resource tcp_socket,
                                                const struct PP_NetAddress_Private *addr,
                                                struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Bool
ppb_tcp_socket_private_get_local_address(PP_Resource tcp_socket,
                                         struct PP_NetAddress_Private *local_addr)
{
    return PP_FALSE;
}

PP_Bool
ppb_tcp_socket_private_get_remote_address(PP_Resource tcp_socket,
                                          struct PP_NetAddress_Private *remote_addr)
{
    return PP_FALSE;
}

int32_t
ppb_tcp_socket_private_ssl_handshake(PP_Resource tcp_socket, const char *server_name,
                                     uint16_t server_port, struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Resource
ppb_tcp_socket_private_get_server_certificate(PP_Resource tcp_socket)
{
    return 0;
}

PP_Bool
ppb_tcp_socket_private_add_chain_building_certificate(PP_Resource tcp_socket,
                                                      PP_Resource certificate, PP_Bool is_trusted)
{
    return PP_FALSE;
}

int32_t
ppb_tcp_socket_private_read(PP_Resource tcp_socket, char *buffer, int32_t bytes_to_read,
                            struct PP_CompletionCallback callback)
{
    return -1;
}

int32_t
ppb_tcp_socket_private_write(PP_Resource tcp_socket, const char *buffer, int32_t bytes_to_write,
                             struct PP_CompletionCallback callback)
{
    return -1;
}

void
ppb_tcp_socket_private_disconnect(PP_Resource tcp_socket)
{
}

int32_t
ppb_tcp_socket_private_set_option(PP_Resource tcp_socket, PP_TCPSocketOption_Private name,
                                  struct PP_Var value, struct PP_CompletionCallback callback)
{
    return -1;
}


// trace wrappers
static
PP_Resource
trace_ppb_tcp_socket_private_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_create(instance);
}

static
PP_Bool
trace_ppb_tcp_socket_private_is_tcp_socket(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_is_tcp_socket(resource);
}

static
int32_t
trace_ppb_tcp_socket_private_connect(PP_Resource tcp_socket, const char *host, uint16_t port,
                                     struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_connect(tcp_socket, host, port, callback);
}

static
int32_t
trace_ppb_tcp_socket_private_connect_with_net_address(PP_Resource tcp_socket,
                                                      const struct PP_NetAddress_Private *addr,
                                                      struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_connect_with_net_address(tcp_socket, addr, callback);
}

static
PP_Bool
trace_ppb_tcp_socket_private_get_local_address(PP_Resource tcp_socket,
                                               struct PP_NetAddress_Private *local_addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_get_local_address(tcp_socket, local_addr);
}

static
PP_Bool
trace_ppb_tcp_socket_private_get_remote_address(PP_Resource tcp_socket,
                                                struct PP_NetAddress_Private *remote_addr)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_get_remote_address(tcp_socket, remote_addr);
}

static
int32_t
trace_ppb_tcp_socket_private_ssl_handshake(PP_Resource tcp_socket, const char *server_name,
                                           uint16_t server_port,
                                           struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_ssl_handshake(tcp_socket, server_name, server_port, callback);
}

static
PP_Resource
trace_ppb_tcp_socket_private_get_server_certificate(PP_Resource tcp_socket)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_get_server_certificate(tcp_socket);
}

static
PP_Bool
trace_ppb_tcp_socket_private_add_chain_building_certificate(PP_Resource tcp_socket,
                                                            PP_Resource certificate,
                                                            PP_Bool is_trusted)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_add_chain_building_certificate(tcp_socket, certificate,
                                                                 is_trusted);
}

static
int32_t
trace_ppb_tcp_socket_private_read(PP_Resource tcp_socket, char *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_read(tcp_socket, buffer, bytes_to_read, callback);
}

static
int32_t
trace_ppb_tcp_socket_private_write(PP_Resource tcp_socket, const char *buffer,
                                   int32_t bytes_to_write, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_write(tcp_socket, buffer, bytes_to_write, callback);
}

static
void
trace_ppb_tcp_socket_private_disconnect(PP_Resource tcp_socket)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_tcp_socket_private_disconnect(tcp_socket);
}

static
int32_t
trace_ppb_tcp_socket_private_set_option(PP_Resource tcp_socket, PP_TCPSocketOption_Private name,
                                        struct PP_Var value, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_private_set_option(tcp_socket, name, value, callback);
}


const struct PPB_TCPSocket_Private_0_5 ppb_tcp_socket_private_interface_0_5 = {
    .Create =                       trace_ppb_tcp_socket_private_create,
    .IsTCPSocket =                  trace_ppb_tcp_socket_private_is_tcp_socket,
    .Connect =                      trace_ppb_tcp_socket_private_connect,
    .ConnectWithNetAddress =        trace_ppb_tcp_socket_private_connect_with_net_address,
    .GetLocalAddress =              trace_ppb_tcp_socket_private_get_local_address,
    .GetRemoteAddress =             trace_ppb_tcp_socket_private_get_remote_address,
    .SSLHandshake =                 trace_ppb_tcp_socket_private_ssl_handshake,
    .GetServerCertificate =         trace_ppb_tcp_socket_private_get_server_certificate,
    .AddChainBuildingCertificate =  trace_ppb_tcp_socket_private_add_chain_building_certificate,
    .Read =                         trace_ppb_tcp_socket_private_read,
    .Write =                        trace_ppb_tcp_socket_private_write,
    .Disconnect =                   trace_ppb_tcp_socket_private_disconnect,
    .SetOption =                    trace_ppb_tcp_socket_private_set_option,
};

const struct PPB_TCPSocket_Private_0_4 ppb_tcp_socket_private_interface_0_4 = {
    .Create =                       trace_ppb_tcp_socket_private_create,
    .IsTCPSocket =                  trace_ppb_tcp_socket_private_is_tcp_socket,
    .Connect =                      trace_ppb_tcp_socket_private_connect,
    .ConnectWithNetAddress =        trace_ppb_tcp_socket_private_connect_with_net_address,
    .GetLocalAddress =              trace_ppb_tcp_socket_private_get_local_address,
    .GetRemoteAddress =             trace_ppb_tcp_socket_private_get_remote_address,
    .SSLHandshake =                 trace_ppb_tcp_socket_private_ssl_handshake,
    .GetServerCertificate =         trace_ppb_tcp_socket_private_get_server_certificate,
    .AddChainBuildingCertificate =  trace_ppb_tcp_socket_private_add_chain_building_certificate,
    .Read =                         trace_ppb_tcp_socket_private_read,
    .Write =                        trace_ppb_tcp_socket_private_write,
    .Disconnect =                   trace_ppb_tcp_socket_private_disconnect,
};
