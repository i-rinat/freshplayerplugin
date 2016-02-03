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

#include "ppb_udp_socket.h"
#include "ppb_core.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"
#include "async_network.h"
#include "pp_interface.h"
#include "ppb_message_loop.h"


PP_Resource
ppb_udp_socket_create(PP_Instance instance_id)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance_id);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource udp_socket = pp_resource_allocate(PP_RESOURCE_UDP_SOCKET, pp_i);
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    us->sock = socket(AF_INET, SOCK_DGRAM, 0);
    pp_resource_release(udp_socket);
    return udp_socket;
}

static
void
ppb_udp_socket_destroy(void *p)
{
    struct pp_udp_socket_s *us = p;

    if (!us->destroyed) {
        struct async_network_task_s *task = async_network_task_create();

        us->destroyed = 1;

        task->type = ASYNC_NETWORK_DISCONNECT;
        task->resource = us->self_id;
        task->sock =     us->sock;
        async_network_task_push(task);
    }
}

PP_Bool
ppb_udp_socket_is_udp_socket(PP_Resource resource_id)
{
    return pp_resource_get_type(resource_id) == PP_RESOURCE_UDP_SOCKET;
}

int32_t
ppb_udp_socket_join_group(PP_Resource udp_socket, PP_Resource group,
                          struct PP_CompletionCallback callback)
{
    // TODO: impl
    return PP_ERROR_FAILED;
}

int32_t
ppb_udp_socket_leave_group(PP_Resource udp_socket, PP_Resource group,
                           struct PP_CompletionCallback callback)
{
    // TODO: impl
    return PP_ERROR_FAILED;
}

int32_t
ppb_udp_socket_set_option(PP_Resource udp_socket, PP_UDPSocket_Option name, struct PP_Var value,
                          struct PP_CompletionCallback callback)
{
    // TODO: impl
    return PP_ERROR_FAILED;
}

int32_t
ppb_udp_socket_set_socket_feature(PP_Resource udp_socket, PP_UDPSocketFeature_Private name,
                                  struct PP_Var value)
{
    return -1;
}

int32_t
ppb_udp_socket_bind(PP_Resource udp_socket, const struct PP_NetAddress_Private *addr,
                    struct PP_CompletionCallback callback)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    memcpy(&us->addr, addr, sizeof(struct PP_NetAddress_Private));

    if (bind(us->sock, (struct sockaddr *)addr->data, addr->size) != 0) {
        trace_warning("%s, bind failed\n", __func__);
        pp_resource_release(udp_socket);
        return PP_ERROR_FAILED;
    }

    us->bound = 1;
    pp_resource_release(udp_socket);
    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, PP_OK, 0,
                                           __func__);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_udp_socket_bind_1_0(PP_Resource udp_socket, PP_Resource addr,
                        struct PP_CompletionCallback callback)
{
    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    int32_t ret = ppb_udp_socket_bind(udp_socket, &na->addr, callback);
    pp_resource_release(addr);
    return ret;
}

PP_Bool
ppb_udp_socket_get_bound_address(PP_Resource udp_socket, struct PP_NetAddress_Private *addr)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (!us->bound) {
        // not bound socket have no address
        pp_resource_release(udp_socket);
        return PP_FALSE;
    }

    memcpy(addr, &us->addr, sizeof(struct PP_NetAddress_Private));

    pp_resource_release(udp_socket);
    return PP_TRUE;
}

PP_Resource
ppb_udp_socket_get_bound_address_1_0(PP_Resource udp_socket)
{
    PP_Resource net_address = 0;
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        goto err_1;
    }

    if (!us->bound) {
        // not bound socket have no address
        goto err_2;
    }

    net_address = pp_resource_allocate(PP_RESOURCE_NET_ADDRESS, us->instance);
    struct pp_net_address_s *na = pp_resource_acquire(net_address, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, resource allocation failed\n", __func__);
        net_address = 0;
        goto err_2;
    }

    memcpy(&na->addr, &us->addr, sizeof(struct PP_NetAddress_Private));
    pp_resource_release(net_address);

err_2:
    pp_resource_release(udp_socket);
err_1:
    return net_address;
}

int32_t
ppb_udp_socket_recv_from(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                         struct PP_CompletionCallback callback)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_UDP_RECV;
    task->resource = udp_socket;
    task->buffer =   buffer;
    task->bufsize =  num_bytes;
    task->callback = callback;
    task->callback_ml = ppb_message_loop_get_current();
    task->addr_from = &us->addr_from;

    pp_resource_release(udp_socket);
    async_network_task_push(task);

    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_udp_socket_recv_from_1_0(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                             PP_Resource *addr, struct PP_CompletionCallback callback)
{
    int ret;
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        ret = PP_ERROR_BADRESOURCE;
        goto err_1;
    }

    PP_Resource net_address = pp_resource_allocate(PP_RESOURCE_NET_ADDRESS, us->instance);
    struct pp_net_address_s *na = pp_resource_acquire(net_address, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, resource allocation failed\n", __func__);
        ret = PP_ERROR_NOMEMORY;
        goto err_2;
    }

    struct async_network_task_s *task = async_network_task_create();

    task->type = ASYNC_NETWORK_UDP_RECV;
    task->resource = udp_socket;
    task->buffer =   buffer;
    task->bufsize =  num_bytes;
    task->callback = callback;
    task->callback_ml = ppb_message_loop_get_current();
    task->addr_from = &na->addr;
    task->addr_from_resource = net_address;

    ppb_core_add_ref_resource(net_address);
    pp_resource_release(net_address);
    pp_resource_release(udp_socket);
    async_network_task_push(task);

    *addr = net_address;

    return PP_OK_COMPLETIONPENDING;

err_2:
    pp_resource_release(udp_socket);
err_1:
    return ret;
}

PP_Bool
ppb_udp_socket_get_recv_from_address(PP_Resource udp_socket, struct PP_NetAddress_Private *addr)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    if (!us->addr_from.size) {
        // size should be larger than zero, otherwise there is no address
        pp_resource_release(udp_socket);
        return PP_FALSE;
    }

    memcpy(addr, &us->addr_from, sizeof(struct PP_NetAddress_Private));

    pp_resource_release(udp_socket);
    return PP_TRUE;
}

int32_t
ppb_udp_socket_send_to(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                       const struct PP_NetAddress_Private *addr,
                       struct PP_CompletionCallback callback)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    num_bytes = MIN(num_bytes, 128 * 1024);

    struct async_network_task_s *task = async_network_task_create();
    task->type =     ASYNC_NETWORK_UDP_SEND;
    task->resource = udp_socket;
    task->buffer =   (char *)buffer;
    task->bufsize =  num_bytes;
    task->callback = callback;
    task->callback_ml = ppb_message_loop_get_current();
    memcpy(&task->netaddr, addr, sizeof(*addr));

    pp_resource_release(udp_socket);
    async_network_task_push(task);

    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_udp_socket_send_to_1_0(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                           PP_Resource addr, struct PP_CompletionCallback callback)
{
    struct pp_net_address_s *na = pp_resource_acquire(addr, PP_RESOURCE_NET_ADDRESS);
    if (!na) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    int32_t ret = ppb_udp_socket_send_to(udp_socket, buffer, num_bytes, &na->addr, callback);
    pp_resource_release(addr);
    return ret;
}

void
ppb_udp_socket_close(PP_Resource udp_socket)
{
    struct pp_udp_socket_s *us = pp_resource_acquire(udp_socket, PP_RESOURCE_UDP_SOCKET);
    if (!us) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    ppb_udp_socket_destroy(us);
    pp_resource_release(udp_socket);
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_udp_socket_create(PP_Instance instance_id)
{
    trace_info("[PPB] {full} %s instance_id=%d\n", __func__+6, instance_id);
    return ppb_udp_socket_create(instance_id);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_udp_socket_is_udp_socket(PP_Resource resource_id)
{
    trace_info("[PPB] {full} %s resource_id=%d\n", __func__+6, resource_id);
    return ppb_udp_socket_is_udp_socket(resource_id);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_join_group(PP_Resource udp_socket, PP_Resource group,
                                struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s udp_socket=%d, group=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, udp_socket, group, callback.func, callback.user_data,
               callback.flags);
    return ppb_udp_socket_join_group(udp_socket, group, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_leave_group(PP_Resource udp_socket, PP_Resource group,
                                 struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s udp_socket=%d, group=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, udp_socket, group, callback.func, callback.user_data,
               callback.flags);
    return ppb_udp_socket_leave_group(udp_socket, group, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_set_option(PP_Resource udp_socket, PP_UDPSocket_Option name,
                                struct PP_Var value, struct PP_CompletionCallback callback)
{
    gchar *s_value = trace_var_as_string(value);
    trace_info("[PPB] {zilch} %s udp_socket=%d, name=%d, value=%s, vallback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, udp_socket, name, s_value, callback.func,
               callback.user_data, callback.flags);
    g_free(s_value);
    return ppb_udp_socket_set_option(udp_socket, name, value, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_set_socket_feature(PP_Resource udp_socket, PP_UDPSocketFeature_Private name,
                                        struct PP_Var value)
{
    gchar *s_value = trace_var_as_string(value);
    trace_info("[PPB] {zilch} %s udp_socket=%d, name=%s(%u), value=%s\n", __func__+6, udp_socket,
               reverse_udp_socket_feature(name), name, s_value);
    g_free(s_value);
    return ppb_udp_socket_set_socket_feature(udp_socket, name, value);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_bind(PP_Resource udp_socket, const struct PP_NetAddress_Private *addr,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, addr=%p, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, udp_socket, addr, callback.func, callback.user_data,
               callback.flags);
    return ppb_udp_socket_bind(udp_socket, addr, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_bind_1_0(PP_Resource udp_socket, PP_Resource addr,
                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, addr=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, udp_socket, addr, callback.func, callback.user_data,
               callback.flags);
    return ppb_udp_socket_bind_1_0(udp_socket, addr, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_udp_socket_get_bound_address(PP_Resource udp_socket, struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s udp_socket=%d, addr=%p\n", __func__+6, udp_socket, addr);
    return ppb_udp_socket_get_bound_address(udp_socket, addr);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_udp_socket_get_bound_address_1_0(PP_Resource udp_socket)
{
    trace_info("[PPB] {full} %s udp_socket=%d\n", __func__+6, udp_socket);
    return ppb_udp_socket_get_bound_address_1_0(udp_socket);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_recv_from(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                               struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, buffer=%p, num_bytes=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, udp_socket, buffer, num_bytes,
               callback.func, callback.user_data, callback.flags);
    return ppb_udp_socket_recv_from(udp_socket, buffer, num_bytes, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_recv_from_1_0(PP_Resource udp_socket, char *buffer, int32_t num_bytes,
                                   PP_Resource *addr, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, buffer=%p, num_bytes=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, udp_socket, buffer, num_bytes,
               callback.func, callback.user_data, callback.flags);
    return ppb_udp_socket_recv_from_1_0(udp_socket, buffer, num_bytes, addr, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_udp_socket_get_recv_from_address(PP_Resource udp_socket,
                                           struct PP_NetAddress_Private *addr)
{
    trace_info("[PPB] {full} %s udp_socket=%d, addr=%p\n", __func__+6, udp_socket, addr);
    return ppb_udp_socket_get_recv_from_address(udp_socket, addr);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_send_to(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                             const struct PP_NetAddress_Private *addr,
                             struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, buffer=%p, num_bytes=%d, addr=%p, callback={"
               ".func=%p, .user_data=%p, .flags=%u}\n", __func__+6, udp_socket, buffer, num_bytes,
               addr, callback.func, callback.user_data, callback.flags);
    return ppb_udp_socket_send_to(udp_socket, buffer, num_bytes, addr, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_udp_socket_send_to_1_0(PP_Resource udp_socket, const char *buffer, int32_t num_bytes,
                                 PP_Resource addr, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s udp_socket=%d, buffer=%p, num_bytes=%d, addr=%d, callback={"
               ".func=%p, .user_data=%p, .flags=%u}\n", __func__+6, udp_socket, buffer, num_bytes,
               addr, callback.func, callback.user_data, callback.flags);
    return ppb_udp_socket_send_to_1_0(udp_socket, buffer, num_bytes, addr, callback);
}

TRACE_WRAPPER
void
trace_ppb_udp_socket_close(PP_Resource udp_socket)
{
    trace_info("[PPB] {full} %s udp_socket=%d\n", __func__+6, udp_socket);
    return ppb_udp_socket_close(udp_socket);
}


const struct PPB_UDPSocket_Private_0_4 ppb_udp_socket_private_interface_0_4 = {
    .Create =               TWRAPF(ppb_udp_socket_create),
    .IsUDPSocket =          TWRAPF(ppb_udp_socket_is_udp_socket),
    .SetSocketFeature =     TWRAPZ(ppb_udp_socket_set_socket_feature),
    .Bind =                 TWRAPF(ppb_udp_socket_bind),
    .GetBoundAddress =      TWRAPF(ppb_udp_socket_get_bound_address),
    .RecvFrom =             TWRAPF(ppb_udp_socket_recv_from),
    .GetRecvFromAddress =   TWRAPF(ppb_udp_socket_get_recv_from_address),
    .SendTo =               TWRAPF(ppb_udp_socket_send_to),
    .Close =                TWRAPF(ppb_udp_socket_close),
};

const struct PPB_UDPSocket_1_2 ppb_udp_socket_interface_1_2 = {
    .Create =           TWRAPF(ppb_udp_socket_create),
    .IsUDPSocket =      TWRAPF(ppb_udp_socket_is_udp_socket),
    .Bind =             TWRAPF(ppb_udp_socket_bind_1_0),
    .GetBoundAddress =  TWRAPF(ppb_udp_socket_get_bound_address_1_0),
    .RecvFrom =         TWRAPF(ppb_udp_socket_recv_from_1_0),
    .SendTo =           TWRAPF(ppb_udp_socket_send_to_1_0),
    .Close =            TWRAPF(ppb_udp_socket_close),
    .SetOption =        TWRAPZ(ppb_udp_socket_set_option),
    .JoinGroup =        TWRAPZ(ppb_udp_socket_join_group),
    .LeaveGroup =       TWRAPZ(ppb_udp_socket_leave_group),
};
const struct PPB_UDPSocket_1_1 ppb_udp_socket_interface_1_1 = {
    .Create =           TWRAPF(ppb_udp_socket_create),
    .IsUDPSocket =      TWRAPF(ppb_udp_socket_is_udp_socket),
    .Bind =             TWRAPF(ppb_udp_socket_bind_1_0),
    .GetBoundAddress =  TWRAPF(ppb_udp_socket_get_bound_address_1_0),
    .RecvFrom =         TWRAPF(ppb_udp_socket_recv_from_1_0),
    .SendTo =           TWRAPF(ppb_udp_socket_send_to_1_0),
    .Close =            TWRAPF(ppb_udp_socket_close),
    .SetOption =        TWRAPZ(ppb_udp_socket_set_option),
};

const struct PPB_UDPSocket_1_0 ppb_udp_socket_interface_1_0 = {
    .Create =           TWRAPF(ppb_udp_socket_create),
    .IsUDPSocket =      TWRAPF(ppb_udp_socket_is_udp_socket),
    .Bind =             TWRAPF(ppb_udp_socket_bind_1_0),
    .GetBoundAddress =  TWRAPF(ppb_udp_socket_get_bound_address_1_0),
    .RecvFrom =         TWRAPF(ppb_udp_socket_recv_from_1_0),
    .SendTo =           TWRAPF(ppb_udp_socket_send_to_1_0),
    .Close =            TWRAPF(ppb_udp_socket_close),
    .SetOption =        TWRAPZ(ppb_udp_socket_set_option),
};

static
void
__attribute__((constructor))
constructor_ppb_udp_socket(void)
{
    register_interface(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4, &ppb_udp_socket_private_interface_0_4);
    register_interface(PPB_UDPSOCKET_INTERFACE_1_0, &ppb_udp_socket_interface_1_0);
    register_interface(PPB_UDPSOCKET_INTERFACE_1_1, &ppb_udp_socket_interface_1_1);
    register_interface(PPB_UDPSOCKET_INTERFACE_1_2, &ppb_udp_socket_interface_1_2);
    register_resource(PP_RESOURCE_UDP_SOCKET, ppb_udp_socket_destroy);
}
