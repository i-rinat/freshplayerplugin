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

#ifndef FPP__ASYNC_NETWORK_H
#define FPP__ASYNC_NETWORK_H

#include <ppapi/c/pp_completion_callback.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/pp_instance.h>
#include <ppapi/c/private/ppb_net_address_private.h>


enum async_network_task_type_e {
    ASYNC_NETWORK_TCP_CONNECT,
    ASYNC_NETWORK_TCP_CONNECT_WITH_NETADDRESS,
    ASYNC_NETWORK_TCP_DISCONNECT,
    ASYNC_NETWORK_TCP_READ,
    ASYNC_NETWORK_TCP_WRITE,
};

struct async_network_task_s {
    enum async_network_task_type_e  type;
    struct PP_CompletionCallback    callback;
    PP_Resource                     resource;
    PP_Instance                     instance;
    char                           *host;
    uint16_t                        port;
    struct PP_NetAddress_Private    netaddr;
    char                           *buffer;
    int32_t                         bufsize;
    void                           *event;
    int                             sock;
};

void
async_network_task_push(struct async_network_task_s *task);

struct async_network_task_s *
async_network_task_create(void);

#endif // FPP__ASYNC_NETWORK_H
