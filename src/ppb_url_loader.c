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

#define _XOPEN_SOURCE   500
#define _GNU_SOURCE
#include "ppb_url_loader.h"
#include "ppb_core.h"
#include "ppb_url_util_dev.h"
#include "ppb_var.h"
#include <ppapi/c/pp_errors.h>
#include <stddef.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "pp_resource.h"
#include "tables.h"


PP_Resource
ppb_url_loader_create(PP_Instance instance)
{
    PP_Resource url_loader = pp_resource_allocate(PP_RESOURCE_URL_LOADER, instance);
    struct pp_url_loader_s *ul = pp_resource_acquire(url_loader, PP_RESOURCE_URL_LOADER);

    // all fields are zeroed by default
    ul->response_size = -1;
    ul->method = PP_METHOD_GET;

    pp_resource_release(url_loader);
    return url_loader;
}

void
ppb_url_loader_destroy(void *p)
{
    if (!p)
        return;
    struct pp_url_loader_s *ul = p;

    if (ul->fp) {
        fclose(ul->fp);
        ul->fp = NULL;
    }
    free_and_nullify(ul, headers);
    free_and_nullify(ul, url);
    free_and_nullify(ul, request_headers);
    free_and_nullify(ul, custom_referrer_url);
    free_and_nullify(ul, custom_content_transfer_encoding);
    free_and_nullify(ul, custom_user_agent);
}

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource)
{
    return PP_RESOURCE_URL_LOADER == pp_resource_get_type(resource);
}

struct triple_s {
        const char *url;
        PP_Resource loader;
        PP_Instance instance;
};

static
void
_url_loader_open_comt(void *user_data, int32_t result)
{
    struct triple_s *triple = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(triple->instance);

    // called on main thread
    tables_push_url_pair(triple->url, triple->loader);
    npn.geturl(pp_i->npp, triple->url, NULL);
    free(triple);
}

FILE *
open_temporary_file_stream(void)
{
    char *tmpfname;
    // TODO: make temp path configurable
    asprintf(&tmpfname, "/tmp/FreshStreamXXXXXX");
    int fd = mkstemp(tmpfname);
    unlink(tmpfname);
    free(tmpfname);
    FILE *fp = fdopen(fd, "wb+");
    return fp;
}

int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    struct pp_url_request_info_s *ri = pp_resource_acquire(request_info,
                                                           PP_RESOURCE_URL_REQUEST_INFO);

    struct PP_Var rel_url = PP_MakeString(ri->url);
    struct PP_Var full_url =
        ppb_url_util_dev_resolve_relative_to_document(ul->_.instance, rel_url, NULL);
    ppb_var_release(rel_url);

    uint32_t len;
    struct triple_s *triple = malloc(sizeof(*triple));
    triple->url = strdup(ppb_var_var_to_utf8(full_url, &len));
    triple->loader = loader;
    triple->instance = ul->_.instance;
    ppb_var_release(full_url);

    ul->method = ri->method;
    ul->url = strdup(triple->url);
    ul->read_pos = 0;
    ul->request_headers = ri->headers ? strdup(ri->headers) : NULL;
    ul->follow_redirects = ri->follow_redirects;
    ul->record_download_progress = ri->record_download_progress;
    ul->record_upload_progress = ri->record_upload_progress;
    ul->custom_referrer_url = ri->custom_referrer_url ? strdup(ri->custom_referrer_url) : NULL;
    ul->allow_cross_origin_requests = ri->allow_cross_origin_requests;
    ul->allow_credentials = ri->allow_credentials;
    ul->custom_content_transfer_encoding = ri->custom_content_transfer_encoding
                                           ? strdup(ri->custom_content_transfer_encoding) : NULL;
    ul->custom_user_agent = ri->custom_user_agent ? strdup(ri->custom_user_agent) : NULL;

    ul->fp = open_temporary_file_stream();

    struct PP_CompletionCallback mt_cb = {.func = _url_loader_open_comt, .user_data = triple};
    ppb_core_call_on_main_thread(0, mt_cb, 0);
    ul->ccb = callback;

    pp_resource_release(request_info);
    pp_resource_release(loader);

    if (callback.func == NULL) {
        int done = 0;
        while (!done) {
            ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
            if (ul) {
                done = ul->loaded;
                pp_resource_release(loader);
            } else {
                break;
            }
            printf("waitin'\n");
            usleep(10000);
        }
        return PP_OK;
    }

    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);

    struct PP_Var rel_url = PP_MakeString(ul->redirect_url);
    struct PP_Var full_url =
        ppb_url_util_dev_resolve_relative_to_document(ul->_.instance, rel_url, NULL);
    ppb_var_release(rel_url);

    free_and_nullify(ul, url);
    free_and_nullify(ul, redirect_url);
    free_and_nullify(ul, status_line);
    free_and_nullify(ul, headers);
    free_and_nullify(ul, request_headers);

    fclose(ul->fp);
    ul->fp = open_temporary_file_stream();

    // abort further handling of the NPStream
    if (ul->np_stream) {
        ul->np_stream->pdata = NULL;
        ul->np_stream = NULL;
    }

    struct triple_s *triple = malloc(sizeof(*triple));
    triple->url = strdup(ppb_var_var_to_utf8(full_url, NULL));
    triple->loader = loader;
    triple->instance = ul->_.instance;
    ul->url = strdup(triple->url);
    ul->read_pos = 0;

    struct PP_CompletionCallback mt_cb = {.func = _url_loader_open_comt, .user_data = triple};
    ppb_core_call_on_main_thread(0, mt_cb, 0);
    ul->ccb = callback;

    pp_resource_release(loader);

    if (callback.func == NULL) {
        int done = 0;
        while (!done) {
            ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
            if (ul) {
                done = ul->loaded;
                pp_resource_release(loader);
            } else {
                break;
            }
            printf("waitin'\n");
            usleep(10000);
        }
        return PP_OK;
    }

    return PP_OK_COMPLETIONPENDING;
}

PP_Bool
ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                   int64_t *total_bytes_to_be_sent)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);

    *total_bytes_to_be_received = ul->response_size;
    *bytes_received = 0;
    if (ul->fp) {
        fseek(ul->fp, 0, SEEK_END);
        *bytes_received = ftell(ul->fp);
    }

    pp_resource_release(loader);
    return PP_TRUE;
}

PP_Resource
ppb_url_loader_get_response_info(PP_Resource loader)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    PP_Resource response_info = pp_resource_allocate(PP_RESOURCE_URL_RESPONSE_INFO, ul->_.instance);
    struct pp_url_response_info_s *ri = pp_resource_acquire(response_info,
                                                            PP_RESOURCE_URL_RESPONSE_INFO);
    pp_resource_release(loader);
    pp_resource_ref(loader);
    ri->url_loader = loader;
    pp_resource_release(response_info);
    return response_info;
}

int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    int read_bytes = 0;

    if (ul->fp) {
        fseek(ul->fp, ul->read_pos, SEEK_SET);
        read_bytes = fread(buffer, 1, bytes_to_read, ul->fp);
        ul->read_pos += read_bytes;
    }

    if (read_bytes == 0) {
        // no data ready, schedule read task
        struct url_loader_read_task_s *rt = malloc(sizeof(*rt));
        rt->buffer = buffer;
        rt->bytes_to_read = bytes_to_read;
        rt->ccb = callback;

        ul->read_tasks = g_list_append(ul->read_tasks, rt);
        pp_resource_release(loader);
        return PP_OK_COMPLETIONPENDING;
    }

    pp_resource_release(loader);
    return read_bytes;
}

int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback)
{
    return 0;
}

void
ppb_url_loader_close(PP_Resource loader)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul)
        return;

    if (ul->fp) {
        fclose(ul->fp);
        ul->fp = NULL;
    }
    free_and_nullify(ul, headers);
    free_and_nullify(ul, url);
    pp_resource_release(loader);
    return;
}

// trace wrappers
static
PP_Resource
trace_ppb_url_loader_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_loader_create(instance);
}

static
PP_Bool
trace_ppb_url_loader_is_url_loader(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_loader_is_url_loader(resource);
}

static
int32_t
trace_ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {part} %s loader=%d, request_info=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}\n", __func__+6, loader, request_info, callback.func, callback.user_data,
               callback.flags);
    return ppb_url_loader_open(loader, request_info, callback);
}

static
int32_t
trace_ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s loader=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_follow_redirect(loader, callback);
}

static
PP_Bool
trace_ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                         int64_t *total_bytes_to_be_sent)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_upload_progress(loader, bytes_sent, total_bytes_to_be_sent);
}

static
PP_Bool
trace_ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                           int64_t *total_bytes_to_be_received)
{
    trace_info("[PPB] {part} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_download_progress(loader, bytes_received, total_bytes_to_be_received);
}

static
PP_Resource
trace_ppb_url_loader_get_response_info(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_response_info(loader);
}

static
int32_t
trace_ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {part} %s loader=%d, buffer=%p, bytes_to_read=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%d}\n", __func__+6, loader, buffer, bytes_to_read,
               callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_read_response_body(loader, buffer, bytes_to_read, callback);
}

static
int32_t
trace_ppb_url_loader_finish_streaming_to_file(PP_Resource loader,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d callback={.func=%p, .user_data=%p, flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_finish_streaming_to_file(loader, callback);
}

static
void
trace_ppb_url_loader_close(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    ppb_url_loader_close(loader);
}


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create =               trace_ppb_url_loader_create,
    .IsURLLoader =          trace_ppb_url_loader_is_url_loader,
    .Open =                 trace_ppb_url_loader_open,
    .FollowRedirect =       trace_ppb_url_loader_follow_redirect,
    .GetUploadProgress =    trace_ppb_url_loader_get_upload_progress,
    .GetDownloadProgress =  trace_ppb_url_loader_get_download_progress,
    .GetResponseInfo =      trace_ppb_url_loader_get_response_info,
    .ReadResponseBody =     trace_ppb_url_loader_read_response_body,
    .FinishStreamingToFile = trace_ppb_url_loader_finish_streaming_to_file,
    .Close =                trace_ppb_url_loader_close,
};
