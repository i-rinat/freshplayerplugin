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

#include <pthread.h>
#include "async_network.h"
#include <glib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <event2/event.h>
#include <event2/util.h>
#include <event2/dns.h>
#include <event2/thread.h>
#include <ppapi/c/pp_errors.h>
#include "pp_resource.h"
#include "ppb_core.h"
#include "trace.h"
#include "config.h"


static struct event_base *event_b = NULL;
static struct evdns_base *evdns_b = NULL;
static GHashTable        *tasks_ht = NULL;
static pthread_mutex_t    lock;

static const struct timeval connect_timeout = { .tv_sec =  60, .tv_usec = 0 };

static
void
__attribute__((constructor))
async_network_constructor(void)
{
    tasks_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pthread_mutex_init(&lock, NULL);
}

static
void
__attribute__((destructor))
async_network_destructor(void)
{
    g_hash_table_unref(tasks_ht);
    pthread_mutex_destroy(&lock);
}

static
int32_t
get_pp_errno(void)
{
    int retval = PP_ERROR_FAILED;
    switch (errno) {
    case EACCES:
    case EPERM:
        return PP_ERROR_NOACCESS;
    case EADDRINUSE:
        return PP_ERROR_ADDRESS_IN_USE;
    case ECONNREFUSED:
        return PP_ERROR_CONNECTION_REFUSED;
    case ENETUNREACH:
        return PP_ERROR_ADDRESS_UNREACHABLE;
    case ETIMEDOUT:
        return PP_ERROR_CONNECTION_TIMEDOUT;
    case ENOTCONN:
        return PP_ERROR_CONNECTION_CLOSED;
    case ECONNRESET:
        return PP_ERROR_CONNECTION_RESET;
    case EAGAIN:
    case EBADF:
        return PP_ERROR_FAILED;
    default:
        trace_error("%s, no conversion for %d\n", __func__, errno);
    }
    return retval;
}

static
void
add_event_mapping(struct async_network_task_s *task, struct event *ev)
{
    pthread_mutex_lock(&lock);
    task->event = ev;
    g_hash_table_replace(tasks_ht, task, task);
    pthread_mutex_unlock(&lock);
}

struct async_network_task_s *
async_network_task_create(void)
{
    return g_slice_new0(struct async_network_task_s);
}

static
void
task_destroy(struct async_network_task_s *task)
{
    pthread_mutex_lock(&lock);
    g_hash_table_remove(tasks_ht, task);
    if (task->event) {
        event_free(task->event);
        task->event = NULL;
    }
    free(task->host);
    g_slice_free(struct async_network_task_s, task);
    pthread_mutex_unlock(&lock);
}

static
void
handle_tcp_connect_stage3(struct async_network_task_s *task);

static
void
handle_tcp_connect_stage4(int sock, short event_flags, void *arg)
{
    struct async_network_task_s *task = arg;
    struct pp_tcp_socket_s *ts = pp_resource_acquire(task->resource, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_warning("%s, tcp socket resource was closed during request (%s:%u)\n", __func__,
                      task->host, (unsigned int)task->port);
        free(task->addr);
        task_destroy(task);
        return;
    }
    char buf[200];
    socklen_t len = sizeof(buf);

    if (event_flags & EV_TIMEOUT)
        ts->is_connected = 0;
    else
        ts->is_connected = (getpeername(ts->sock, (struct sockaddr *)buf, &len) == 0);

    if (ts->is_connected) {
        ppb_core_call_on_main_thread(0, task->callback, PP_OK);
        pp_resource_release(task->resource);
        free(task->addr);
        task_destroy(task);
        return;
    }

    // try other addresses, one by one
    task->addr_ptr++;
    if (task->addr_ptr < task->addr_count) {
        pp_resource_release(task->resource);
        handle_tcp_connect_stage3(task);
        return;
    }

    // no addresses left, fail gracefully
    trace_warning("%s, connection failed to all addresses (%s:%u)\n", __func__, task->host,
                  (unsigned int)task->port);
    ppb_core_call_on_main_thread(0, task->callback, get_pp_errno());
    pp_resource_release(task->resource);
    free(task->addr);
    task_destroy(task);
}

static
void
handle_tcp_connect_stage3(struct async_network_task_s *task)
{
    int res = -1;
    if (task->addr_type == DNS_IPv4_A) {
        struct sockaddr_in sai;
        memset(&sai, 0, sizeof(sai));
        sai.sin_family = AF_INET;
        sai.sin_addr.s_addr = *((uint32_t *)task->addr + task->addr_ptr);
        sai.sin_port = htons(task->port);
        res = connect(task->sock, (struct sockaddr *)&sai, sizeof(sai));
    } else if (task->addr_type == DNS_IPv6_AAAA) {
        struct sockaddr_in6 sai;
        memset(&sai, 0, sizeof(sai));
        sai.sin6_family = AF_INET6;
        memcpy(&sai.sin6_addr, (char*)task->addr + task->addr_ptr * sizeof(sai.sin6_addr),
                                                                    sizeof(sai.sin6_addr));
        sai.sin6_port = htons(task->port);
        res = connect(task->sock, (struct sockaddr *)&sai, sizeof(sai));
    } else {
        // handled in stage2
        trace_error("%s, never reached\n", __func__);
    }

    if (res != 0 && errno != EINPROGRESS) {
        trace_error("%s, res = %d, errno = %d (%s:%u)\n", __func__, res, errno, task->host,
                    (unsigned int)task->port);
        ppb_core_call_on_main_thread(0, task->callback, get_pp_errno());
        free(task->addr);
        task_destroy(task);
        return;
    }

    struct event *ev = event_new(event_b, task->sock, EV_WRITE, handle_tcp_connect_stage4, task);
    add_event_mapping(task, ev);
    event_add(ev, &connect_timeout);
}

static
void
handle_tcp_connect_stage2(int result, char type, int count, int ttl, void *addresses, void *arg)
{
    struct async_network_task_s *task = arg;

    if (result != DNS_ERR_NONE || count < 1) {
        trace_warning("%s, evdns returned code %d, count = %d (%s:%u)\n", __func__, result, count,
                      task->host, (unsigned int)task->port);
        ppb_core_call_on_main_thread(0, task->callback, PP_ERROR_NAME_NOT_RESOLVED);
        task_destroy(task);
        return;
    }

    evutil_make_socket_nonblocking(task->sock);

    task->addr_count = count;
    task->addr_ptr = 0;
    task->addr_type = type;

    if (type == DNS_IPv4_A) {
        task->addr = malloc(4 * count);
        memcpy(task->addr, addresses, 4 * count);
    } else if (type == DNS_IPv6_AAAA) {
        task->addr = malloc(16 * count);
        memcpy(task->addr, addresses, 16 * count);
    } else {
        trace_error("%s, bad evdns type %d (%s:%u)\n", __func__, type, task->host,
                    (unsigned int)task->port);
        ppb_core_call_on_main_thread(0, task->callback, PP_ERROR_FAILED);
        task_destroy(task);
        return;
    }

    handle_tcp_connect_stage3(task);
}

static
void
handle_tcp_connect_stage1(struct async_network_task_s *task)
{
    struct evdns_request *req;
    struct sockaddr_in sai;

    memset(&sai, 0, sizeof(sai));
    if (inet_pton(AF_INET, task->host, &sai.sin_addr) == 1) {
        // already a valid IP address
        handle_tcp_connect_stage2(DNS_ERR_NONE, DNS_IPv4_A, 1, 300, &sai.sin_addr, task);
        return;
    }

    // queue DNS request
    req = evdns_base_resolve_ipv4(evdns_b, task->host, DNS_QUERY_NO_SEARCH,
                                  handle_tcp_connect_stage2, task);
    // TODO: what about ipv6?

    if (!req) {
        trace_warning("%s, early dns resolution failure (%s:%u)\n", __func__, task->host,
                      (unsigned int)task->port);
        ppb_core_call_on_main_thread(0, task->callback, PP_ERROR_NAME_NOT_RESOLVED);
        task_destroy(task);
        return;
    }
}

static
void
handle_tcp_connect_with_net_address(struct async_network_task_s *task)
{
    if (task->netaddr.size == sizeof(struct sockaddr_in)) {
        struct sockaddr_in *sai = (void *)task->netaddr.data;
        task->port = ntohs(sai->sin_port);
        handle_tcp_connect_stage2(DNS_ERR_NONE, DNS_IPv4_A, 1, 3600, &sai->sin_addr, task);
    } else if (task->netaddr.size == sizeof(struct sockaddr_in6)) {
        struct sockaddr_in6 *sai = (void *)task->netaddr.data;
        task->port = ntohs(sai->sin6_port);
        handle_tcp_connect_stage2(DNS_ERR_NONE, DNS_IPv6_AAAA, 1, 3600, &sai->sin6_addr, task);
    } else {
        trace_error("%s, bad address type\n", __func__);
        ppb_core_call_on_main_thread(0, task->callback, PP_ERROR_NAME_NOT_RESOLVED);
        task_destroy(task);
    }
}

static
void
handle_tcp_read_stage2(int sock, short event_flags, void *arg)
{
    struct async_network_task_s *task = arg;
    int32_t retval;

    retval = recv(sock, task->buffer, task->bufsize, 0);
    if (retval < 0)
        retval = get_pp_errno();
    else if (retval == 0) {
        struct pp_tcp_socket_s *ts = pp_resource_acquire(task->resource, PP_RESOURCE_TCP_SOCKET);
        if (ts) {
            ts->seen_eof = 1;
            pp_resource_release(task->resource);
        }
    }

    ppb_core_call_on_main_thread(0, task->callback, retval);
    task_destroy(task);
}

static
void
handle_tcp_read_stage1(struct async_network_task_s *task)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(task->resource, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        task_destroy(task);
        return;
    }

    struct event *ev = event_new(event_b, ts->sock, EV_READ, handle_tcp_read_stage2, task);
    pp_resource_release(task->resource);
    add_event_mapping(task, ev);
    event_add(ev, NULL);
}

static
void
handle_tcp_write_stage2(int sock, short event_flags, void *arg)
{
    struct async_network_task_s *task = arg;

    int32_t retval = send(sock, task->buffer, task->bufsize, 0);
    if (retval < 0)
        retval = get_pp_errno();

    ppb_core_call_on_main_thread(0, task->callback, retval);
    task_destroy(task);
}

static
void
handle_tcp_write_stage1(struct async_network_task_s *task)
{
    struct pp_tcp_socket_s *ts = pp_resource_acquire(task->resource, PP_RESOURCE_TCP_SOCKET);
    if (!ts) {
        trace_error("%s, bad resource\n", __func__);
        task_destroy(task);
        return;
    }

    struct event *ev = event_new(event_b, ts->sock, EV_WRITE, handle_tcp_write_stage2, task);
    pp_resource_release(task->resource);
    add_event_mapping(task, ev);
    event_add(ev, NULL);
}

static
void
handle_tcp_disconnect_stage2(int sock, short event_flags, void *arg)
{
    struct async_network_task_s *task = arg;
    GHashTableIter iter;
    gpointer key, val;

    pthread_mutex_lock(&lock);
    g_hash_table_iter_init(&iter, tasks_ht);
    while (g_hash_table_iter_next(&iter, &key, &val)) {
        struct async_network_task_s *cur = key;
        if (cur->resource == task->resource) {
            g_hash_table_iter_remove(&iter);
            event_free(cur->event);
            ppb_core_call_on_main_thread(0, cur->callback, PP_ERROR_ABORTED);
            g_slice_free(struct async_network_task_s, cur);
        }
    }
    pthread_mutex_unlock(&lock);

    close(task->sock);
    task_destroy(task);
}

static
void
handle_tcp_disconnect_stage1(struct async_network_task_s *task)
{
    struct event *ev = evtimer_new(event_b, handle_tcp_disconnect_stage2, task);
    struct timeval timeout = {.tv_sec = 0};
    evtimer_add(ev, &timeout);
}

static
void *
network_worker_thread(void *param)
{
    event_base_dispatch(event_b);
    event_base_free(event_b);
    trace_error("%s, thread terminated\n", __func__);
    return NULL;
}

void
async_network_task_push(struct async_network_task_s *task)
{
    static int thread_started = 0;

    if (!thread_started) {
        evthread_use_pthreads();
        event_b = event_base_new();
        evdns_b = evdns_base_new(event_b, 0);
        evdns_base_resolv_conf_parse(evdns_b, DNS_OPTIONS_ALL, "/etc/resolv.conf");
        if (config.randomize_dns_case == 0)
            evdns_base_set_option(evdns_b, "randomize-case:", "0");
        g_thread_create ((GThreadFunc)network_worker_thread, NULL, FALSE, NULL);
        thread_started = 1;
    }

    switch (task->type) {
    case ASYNC_NETWORK_TCP_CONNECT:
        handle_tcp_connect_stage1(task);
        break;
    case ASYNC_NETWORK_TCP_CONNECT_WITH_NETADDRESS:
        handle_tcp_connect_with_net_address(task);
        break;
    case ASYNC_NETWORK_TCP_DISCONNECT:
        handle_tcp_disconnect_stage1(task);
        break;
    case ASYNC_NETWORK_TCP_READ:
        handle_tcp_read_stage1(task);
        break;
    case ASYNC_NETWORK_TCP_WRITE:
        handle_tcp_write_stage1(task);
        break;
    }
}
