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

#include "ppb_tcp_socket.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ppapi/c/pp_errors.h>
#include "ppb_core.h"
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "async_network.h"
#include "pp_interface.h"


PP_Resource
ppb_tcp_socket_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource tcp_socket = pp_resource_allocate(PP_RESOURCE_TCP_SOCKET, pp_i);
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    ts->sock = socket(AF_INET, SOCK_STREAM, 0);
    pp_resource_release(tcp_socket);
    return tcp_socket;
}

static
void
ppb_tcp_socket_destroy(void *ptr)
{
    struct pp_tcp_socket_s *ts = ptr;
    if (!ts->destroyed) {
        struct async_network_task_s *task = async_network_task_create();

        ts->destroyed = 1;
        ts->is_connected = 0;

        task->type = ASYNC_NETWORK_DISCONNECT;
        task->resource = ts->self_id;
        task->sock = ts->sock;
        async_network_task_push(task);
    }
}

PP_Bool
ppb_tcp_socket_is_tcp_socket(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_TCP_SOCKET;
}

int32_t
ppb_tcp_socket_connect(PP_Resource tcp_socket, const char *host, uint16_t port,
                       struct PP_CompletionCallback callback)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }
    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_TCP_CONNECT;
    task->resource = tcp_socket;
    task->sock = ts->sock;
    task->host = nullsafe_strdup(host);
    task->port = port;
    task->callback = callback;
    pp_resource_release(tcp_socket);

    async_network_task_push(task);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_tcp_socket_connect_with_net_address(PP_Resource tcp_socket,
                                        const struct PP_NetAddress_Private *addr,
                                        struct PP_CompletionCallback callback)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }
    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_TCP_CONNECT_WITH_NETADDRESS;
    task->resource = tcp_socket;
    task->sock = ts->sock;
    task->netaddr = *addr;
    task->callback = callback;
    pp_resource_release(tcp_socket);

    async_network_task_push(task);
    return PP_OK_COMPLETIONPENDING;
}

PP_Bool
ppb_tcp_socket_get_local_address(PP_Resource tcp_socket, struct PP_NetAddress_Private *local_addr)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (!ts->is_connected) {
        trace_warning("%s, not connected\n", __func__);
        pp_resource_release(tcp_socket);
        return PP_FALSE;
    }

    socklen_t len = sizeof(local_addr->data);
    int ret = getsockname(ts->sock, (struct sockaddr *)local_addr->data, &len);
    local_addr->size = len;

    pp_resource_release(tcp_socket);
    return ret == 0 ? PP_TRUE : PP_FALSE;
}

PP_Bool
ppb_tcp_socket_get_remote_address(PP_Resource tcp_socket, struct PP_NetAddress_Private *remote_addr)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (!ts->is_connected) {
        trace_warning("%s, not connected\n", __func__);
        pp_resource_release(tcp_socket);
        return PP_FALSE;
    }

    socklen_t len = sizeof(remote_addr->data);
    int ret = getpeername(ts->sock, (struct sockaddr *)remote_addr->data, &len);
    remote_addr->size = len;

    pp_resource_release(tcp_socket);
    return ret == 0 ? PP_TRUE : PP_FALSE;
}

int32_t
ppb_tcp_socket_ssl_handshake(PP_Resource tcp_socket, const char *server_name, uint16_t server_port,
                             struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Resource
ppb_tcp_socket_get_server_certificate(PP_Resource tcp_socket)
{
    return 0;
}

PP_Bool
ppb_tcp_socket_add_chain_building_certificate(PP_Resource tcp_socket, PP_Resource certificate,
                                              PP_Bool is_trusted)
{
    return PP_FALSE;
}

int32_t
ppb_tcp_socket_read(PP_Resource tcp_socket, char *buffer, int32_t bytes_to_read,
                    struct PP_CompletionCallback callback)
{
    if (bytes_to_read <= 0) {
        trace_error("%s, bytes_to_read <= 0\n", __func__);
        return PP_ERROR_BADARGUMENT;
    }

    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!ts->is_connected) {
        trace_warning("%s, not connected\n", __func__);
        pp_resource_release(tcp_socket);
        return PP_ERROR_FAILED;
    }

    if (ts->seen_eof) {
        trace_warning("%s, seen eof\n", __func__);
        pp_resource_release(tcp_socket);
        return PP_ERROR_FAILED;
    }

    if (bytes_to_read > 1024 * 1024)
        bytes_to_read = 1024 * 1024;

    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_TCP_READ;
    task->resource = tcp_socket;
    task->buffer = buffer;
    task->bufsize = bytes_to_read;
    task->callback = callback;
    pp_resource_release(tcp_socket);

    async_network_task_push(task);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_tcp_socket_write(PP_Resource tcp_socket, const char *buffer, int32_t bytes_to_write,
                     struct PP_CompletionCallback callback)
{
    if (bytes_to_write <= 0) {
        trace_error("%s, bytes_to_write <= 0\n", __func__);
        return PP_ERROR_BADARGUMENT;
    }

    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!ts->is_connected) {
        trace_warning("%s, not connected\n", __func__);
        pp_resource_release(tcp_socket);
        return PP_ERROR_FAILED;
    }

    if (bytes_to_write > 1024 * 1024)
        bytes_to_write = 1024 * 1024;

    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_TCP_WRITE;
    task->resource = tcp_socket;
    task->buffer = (char *)buffer;
    task->bufsize = bytes_to_write;
    task->callback = callback;
    pp_resource_release(tcp_socket);

    async_network_task_push(task);
    return PP_OK_COMPLETIONPENDING;
}

void
ppb_tcp_socket_disconnect(PP_Resource tcp_socket)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(tcp_socket, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }
    ppb_tcp_socket_destroy(ts);
    pp_resource_release(tcp_socket);
}

int32_t
ppb_tcp_socket_set_option(PP_Resource tcp_socket, PP_TCPSocketOption_Private name,
                          struct PP_Var value, struct PP_CompletionCallback callback)
{
    return -1;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_tcp_socket_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_tcp_socket_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_tcp_socket_is_tcp_socket(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_tcp_socket_is_tcp_socket(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_connect(PP_Resource tcp_socket, const char *host, uint16_t port,
                             struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s tcp_socket=%d, host=%s, port=%u, callback={.func=%p, "
               ".user_data=%p, flags=%u}\n", __func__+6, tcp_socket, host, port, callback.func,
               callback.user_data, callback.flags);
    return ppb_tcp_socket_connect(tcp_socket, host, port, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_connect_with_net_address(PP_Resource tcp_socket,
                                              const struct PP_NetAddress_Private *addr,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s tcp_socket=%d, addr={%p}, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, tcp_socket, addr, callback.func,
               callback.user_data, callback.flags);
    return ppb_tcp_socket_connect_with_net_address(tcp_socket, addr, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_tcp_socket_get_local_address(PP_Resource tcp_socket,
                                       struct PP_NetAddress_Private *local_addr)
{
    trace_info("[PPB] {full} %s tcp_socket=%d\n", __func__+6, tcp_socket);
    return ppb_tcp_socket_get_local_address(tcp_socket, local_addr);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_tcp_socket_get_remote_address(PP_Resource tcp_socket,
                                        struct PP_NetAddress_Private *remote_addr)
{
    trace_info("[PPB] {full} %s tcp_socket=%d\n", __func__+6, tcp_socket);
    return ppb_tcp_socket_get_remote_address(tcp_socket, remote_addr);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_ssl_handshake(PP_Resource tcp_socket, const char *server_name,
                                   uint16_t server_port, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_ssl_handshake(tcp_socket, server_name, server_port, callback);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_tcp_socket_get_server_certificate(PP_Resource tcp_socket)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_get_server_certificate(tcp_socket);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_tcp_socket_add_chain_building_certificate(PP_Resource tcp_socket, PP_Resource certificate,
                                                    PP_Bool is_trusted)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_add_chain_building_certificate(tcp_socket, certificate, is_trusted);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_read(PP_Resource tcp_socket, char *buffer, int32_t bytes_to_read,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s tcp_socket=%d, bytes_to_read=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%d}\n", __func__+6, tcp_socket, bytes_to_read, callback.func,
               callback.user_data, callback.flags);
    return ppb_tcp_socket_read(tcp_socket, buffer, bytes_to_read, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_write(PP_Resource tcp_socket, const char *buffer, int32_t bytes_to_write,
                           struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s tcp_socket=%d, bytes_to_write=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%d}\n", __func__+6, tcp_socket, bytes_to_write,
               callback.func, callback.user_data, callback.flags);
    return ppb_tcp_socket_write(tcp_socket, buffer, bytes_to_write, callback);
}

TRACE_WRAPPER
void
trace_ppb_tcp_socket_disconnect(PP_Resource tcp_socket)
{
    trace_info("[PPB] {full} %s tcp_socket=%d\n", __func__+6, tcp_socket);
    ppb_tcp_socket_disconnect(tcp_socket);
}

TRACE_WRAPPER
int32_t
trace_ppb_tcp_socket_set_option(PP_Resource tcp_socket, PP_TCPSocketOption_Private name,
                                struct PP_Var value, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_tcp_socket_set_option(tcp_socket, name, value, callback);
}


const struct PPB_TCPSocket_Private_0_5 ppb_tcp_socket_private_interface_0_5 = {
    .Create =                       TWRAPF(ppb_tcp_socket_create),
    .IsTCPSocket =                  TWRAPF(ppb_tcp_socket_is_tcp_socket),
    .Connect =                      TWRAPF(ppb_tcp_socket_connect),
    .ConnectWithNetAddress =        TWRAPF(ppb_tcp_socket_connect_with_net_address),
    .GetLocalAddress =              TWRAPF(ppb_tcp_socket_get_local_address),
    .GetRemoteAddress =             TWRAPF(ppb_tcp_socket_get_remote_address),
    .SSLHandshake =                 TWRAPZ(ppb_tcp_socket_ssl_handshake),
    .GetServerCertificate =         TWRAPZ(ppb_tcp_socket_get_server_certificate),
    .AddChainBuildingCertificate =  TWRAPZ(ppb_tcp_socket_add_chain_building_certificate),
    .Read =                         TWRAPF(ppb_tcp_socket_read),
    .Write =                        TWRAPF(ppb_tcp_socket_write),
    .Disconnect =                   TWRAPF(ppb_tcp_socket_disconnect),
    .SetOption =                    TWRAPZ(ppb_tcp_socket_set_option),
};

const struct PPB_TCPSocket_Private_0_4 ppb_tcp_socket_private_interface_0_4 = {
    .Create =                       TWRAPF(ppb_tcp_socket_create),
    .IsTCPSocket =                  TWRAPF(ppb_tcp_socket_is_tcp_socket),
    .Connect =                      TWRAPF(ppb_tcp_socket_connect),
    .ConnectWithNetAddress =        TWRAPF(ppb_tcp_socket_connect_with_net_address),
    .GetLocalAddress =              TWRAPF(ppb_tcp_socket_get_local_address),
    .GetRemoteAddress =             TWRAPF(ppb_tcp_socket_get_remote_address),
    .SSLHandshake =                 TWRAPZ(ppb_tcp_socket_ssl_handshake),
    .GetServerCertificate =         TWRAPZ(ppb_tcp_socket_get_server_certificate),
    .AddChainBuildingCertificate =  TWRAPZ(ppb_tcp_socket_add_chain_building_certificate),
    .Read =                         TWRAPF(ppb_tcp_socket_read),
    .Write =                        TWRAPF(ppb_tcp_socket_write),
    .Disconnect =                   TWRAPF(ppb_tcp_socket_disconnect),
};

static
void
__attribute__((constructor))
constructor_ppb_tcp_socket(void)
{
    register_interface(PPB_TCPSOCKET_PRIVATE_INTERFACE_0_4, &ppb_tcp_socket_private_interface_0_4);
    register_interface(PPB_TCPSOCKET_PRIVATE_INTERFACE_0_5, &ppb_tcp_socket_private_interface_0_5);
    register_resource(PP_RESOURCE_TCP_SOCKET, ppb_tcp_socket_destroy);
}
