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

#ifndef FPP_PPB_TCP_SOCKET_H
#define FPP_PPB_TCP_SOCKET_H

#include <ppapi/c/private/ppb_tcp_socket_private.h>


PP_Resource
ppb_tcp_socket_create(PP_Instance instance);

PP_Bool
ppb_tcp_socket_is_tcp_socket(PP_Resource resource);

int32_t
ppb_tcp_socket_connect(PP_Resource tcp_socket, const char *host, uint16_t port,
                       struct PP_CompletionCallback callback);

int32_t
ppb_tcp_socket_connect_with_net_address(PP_Resource tcp_socket,
                                        const struct PP_NetAddress_Private *addr,
                                        struct PP_CompletionCallback callback);

PP_Bool
ppb_tcp_socket_get_local_address(PP_Resource tcp_socket,
                                 struct PP_NetAddress_Private *local_addr);

PP_Bool
ppb_tcp_socket_get_remote_address(PP_Resource tcp_socket,
                                  struct PP_NetAddress_Private *remote_addr);

int32_t
ppb_tcp_socket_ssl_handshake(PP_Resource tcp_socket, const char *server_name, uint16_t server_port,
                             struct PP_CompletionCallback callback);

PP_Resource
ppb_tcp_socket_get_server_certificate(PP_Resource tcp_socket);

PP_Bool
ppb_tcp_socket_add_chain_building_certificate(PP_Resource tcp_socket, PP_Resource certificate,
                                              PP_Bool is_trusted);

int32_t
ppb_tcp_socket_read(PP_Resource tcp_socket, char *buffer, int32_t bytes_to_read,
                    struct PP_CompletionCallback callback);

int32_t
ppb_tcp_socket_write(PP_Resource tcp_socket, const char *buffer, int32_t bytes_to_write,
                     struct PP_CompletionCallback callback);

void
ppb_tcp_socket_disconnect(PP_Resource tcp_socket);

int32_t
ppb_tcp_socket_set_option(PP_Resource tcp_socket, PP_TCPSocketOption_Private name,
                          struct PP_Var value, struct PP_CompletionCallback callback);

#endif // FPP_PPB_TCP_SOCKET_H
