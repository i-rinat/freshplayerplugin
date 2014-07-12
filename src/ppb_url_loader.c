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

#define _XOPEN_SOURCE   600
#include "ppb_url_loader.h"
#include "ppb_core.h"
#include "ppb_url_util_dev.h"
#include "ppb_var.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include "trace.h"
#include "pp_resource.h"
#include "tables.h"
#include "eintr_retry.h"


PP_Resource
ppb_url_loader_create(PP_Instance instance)
{
    PP_Resource url_loader = pp_resource_allocate(PP_RESOURCE_URL_LOADER, instance);
    struct pp_url_loader_s *ul = pp_resource_acquire(url_loader, PP_RESOURCE_URL_LOADER);

    // all fields are zeroed by default
    ul->response_size = -1;
    ul->method = PP_METHOD_GET;
    ul->fd = -1;

    pp_resource_release(url_loader);
    return url_loader;
}

void
ppb_url_loader_destroy(void *p)
{
    if (!p)
        return;
    struct pp_url_loader_s *ul = p;

    if (ul->fd >= 0) {
        close(ul->fd);
        ul->fd = -1;
    }
    free_and_nullify(ul, headers);
    free_and_nullify(ul, url);
    free_and_nullify(ul, request_headers);
    free_and_nullify(ul, custom_referrer_url);
    free_and_nullify(ul, custom_content_transfer_encoding);
    free_and_nullify(ul, custom_user_agent);
    free_and_nullify(ul, target);
}

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource)
{
    return PP_RESOURCE_URL_LOADER == pp_resource_get_type(resource);
}

struct comt_param_s {
    const char                 *url;
    PP_Resource                 loader;
    PP_Instance                 instance;
    enum pp_request_method_e    method;
    const char                 *request_headers;
    const char                 *custom_referrer_url;
    const char                 *custom_content_transfer_encoding;
    const char                 *custom_user_agent;
    const char                 *target;
    const char                 *post_data;
    size_t                      post_len;
    pthread_barrier_t           barrier;
    size_t                      should_wait;
    int                         retval;
};

static
void
_url_loader_open_comt(void *user_data)
{
    struct comt_param_s *comt_params = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(comt_params->instance);

    // called on main thread
    if (comt_params->method == PP_METHOD_POST) {
        // POST request
        char   *tmpfname;
        int     fd;
        FILE   *fp;
        int     need_newline = 0;

        tmpfname = g_strdup_printf("/tmp/FreshPostBodyXXXXXX");    // TODO: make configurable
        // TODO: error handling
        fd = mkstemp(tmpfname);
        fp = fdopen(fd, "wb+");

        if (comt_params->request_headers) {
            fprintf(fp, "%s\n", comt_params->request_headers);
            need_newline = 1;
        }
        if (comt_params->custom_referrer_url) {
            fprintf(fp, "Referrer: %s\n", comt_params->custom_referrer_url);
            need_newline = 1;
        }
        if (comt_params->custom_content_transfer_encoding) {
            fprintf(fp, "Content-Transfer-Encoding: %s\n",
                    comt_params->custom_content_transfer_encoding);
            need_newline = 1;
        }
        if (comt_params->custom_user_agent) {
            fprintf(fp, "User-Agent: %s\n", comt_params->custom_user_agent);
            need_newline = 1;
        }
        if (comt_params->post_len > 0) {
            fprintf(fp, "Content-Length: %"PRIu64"\n", (uint64_t)comt_params->post_len);
            need_newline = 1;
        }

        if (need_newline)
            fprintf(fp, "\n");

        fwrite(comt_params->post_data, 1, comt_params->post_len, fp);
        fclose(fp);

        if (comt_params->target) {
            comt_params->retval = npn.posturl(pp_i->npp, comt_params->url, comt_params->target,
                                              strlen(tmpfname), tmpfname, true);
        } else {
            comt_params->retval = npn.posturlnotify(pp_i->npp, comt_params->url, NULL,
                                                    strlen(tmpfname), tmpfname, true,
                                                    (void*)(size_t)comt_params->loader);
        }
        unlink(tmpfname);
        g_free(tmpfname);
    } else {
        // GET request
        if (comt_params->target) {
            comt_params->retval = npn.geturl(pp_i->npp, comt_params->url, comt_params->target);
        } else {
            comt_params->retval = npn.geturlnotify(pp_i->npp, comt_params->url, NULL,
                                                   (void*)(size_t)comt_params->loader);
        }
    }

    if (comt_params->should_wait)
        pthread_barrier_wait(&comt_params->barrier);
}

int
open_temporary_file(void)
{
    char *tmpfname;
    // TODO: make temp path configurable
    tmpfname = g_strdup_printf("/tmp/FreshStreamXXXXXX");
    int fd = mkstemp(tmpfname);
    unlink(tmpfname);
    g_free(tmpfname);
    return fd;
}

/// trim new line characters from the end of the string
char *
trim_nl(char *s)
{
    if (!s)
        return NULL;
    size_t len = strlen(s);
    while (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = 0;
        len --;
    }
    return s;
}

int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback)
{
    return ppb_url_loader_open_target(loader, request_info, callback, NULL);
}

int32_t
ppb_url_loader_open_target(PP_Resource loader, PP_Resource request_info,
                           struct PP_CompletionCallback callback, const char *target)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    struct pp_url_request_info_s *ri = pp_resource_acquire(request_info,
                                                           PP_RESOURCE_URL_REQUEST_INFO);

    struct PP_Var full_url;

    if (ri->is_immediate_javascript) {
        full_url = PP_MakeString(ri->url);
    } else {
        struct PP_Var rel_url = PP_MakeString(ri->url);
        full_url = ppb_url_util_dev_resolve_relative_to_document(ul->_.instance, rel_url, NULL);
        ppb_var_release(rel_url);
    }

    ul->url =              nullsafe_strdup(ppb_var_var_to_utf8(full_url, NULL));
    ul->method =           ri->method;
    ul->read_pos =         0;
    ul->request_headers =  nullsafe_strdup(ri->headers);
    ul->follow_redirects = ri->follow_redirects;
    ul->stream_to_file =   ri->stream_to_file;

    ul->record_download_progress =         ri->record_download_progress;
    ul->record_upload_progress =           ri->record_upload_progress;
    ul->custom_referrer_url =              nullsafe_strdup(ri->custom_referrer_url);
    ul->allow_cross_origin_requests =      ri->allow_cross_origin_requests;
    ul->allow_credentials =                ri->allow_credentials;
    ul->custom_content_transfer_encoding = nullsafe_strdup(ri->custom_content_transfer_encoding);
    ul->custom_user_agent =                nullsafe_strdup(ri->custom_user_agent);
    ul->target =                           nullsafe_strdup(target);

#define TRIM_NEWLINE(s)     s = trim_nl(s)

    TRIM_NEWLINE(ul->request_headers);
    TRIM_NEWLINE(ul->custom_referrer_url);
    TRIM_NEWLINE(ul->custom_content_transfer_encoding);
    TRIM_NEWLINE(ul->custom_user_agent);

    ul->post_len = ri->post_len;
    if (ri->post_len > 0) {
        ul->post_data = malloc(ri->post_len);
        memcpy(ul->post_data, ri->post_data, ri->post_len);
    }

    ul->fd = open_temporary_file();
    ul->ccb = callback;

    ppb_var_release(full_url);
    pp_resource_release(request_info);

    struct comt_param_s *comt_params = g_slice_alloc(sizeof(*comt_params));
    comt_params->url =                              strdup(ul->url);
    comt_params->loader =                           loader;
    comt_params->instance =                         ul->_.instance;
    comt_params->method =                           ul->method;
    comt_params->request_headers =                  ul->request_headers;
    comt_params->custom_referrer_url =              ul->custom_referrer_url;
    comt_params->custom_content_transfer_encoding = ul->custom_content_transfer_encoding;
    comt_params->custom_user_agent =                ul->custom_user_agent;
    comt_params->target =                           ul->target;
    comt_params->post_len =                         ul->post_len;
    comt_params->post_data =                        ul->post_data;

    struct pp_instance_s *pp_i = tables_get_pp_instance(ul->_.instance);
    if (ppb_core_is_main_thread()) {
        comt_params->should_wait = 0;
        _url_loader_open_comt(comt_params);
    } else {
        comt_params->should_wait = 1;
        pthread_barrier_init(&comt_params->barrier, NULL, 2);
        npn.pluginthreadasynccall(pp_i->npp, _url_loader_open_comt, comt_params);
        pthread_barrier_wait(&comt_params->barrier);
        pthread_barrier_destroy(&comt_params->barrier);
    }

    int retval = comt_params->retval;
    pp_resource_release(loader);
    g_slice_free(struct comt_param_s, comt_params);

    if (retval != NPERR_NO_ERROR)
        return PP_ERROR_FAILED;

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

    char *new_url = nullsafe_strdup(ul->redirect_url);

    free_and_nullify(ul, url);
    free_and_nullify(ul, redirect_url);
    free_and_nullify(ul, status_line);
    free_and_nullify(ul, headers);
    free_and_nullify(ul, request_headers);

    if (ul->fd >= 0) {
        close(ul->fd);
        ul->fd = -1;
    }

    // abort further handling of the NPStream
    if (ul->np_stream) {
        ul->np_stream->pdata = NULL;
        ul->np_stream = NULL;
    }

    ul->fd = open_temporary_file();
    ul->url = new_url;
    ul->read_pos = 0;
    ul->method = PP_METHOD_GET;
    ul->ccb = callback;

    struct comt_param_s *comt_params = g_slice_alloc(sizeof(*comt_params));
    comt_params->url =                              ul->url;
    comt_params->loader =                           loader;
    comt_params->instance =                         ul->_.instance;
    comt_params->method =                           ul->method;
    comt_params->request_headers =                  ul->request_headers;
    comt_params->custom_referrer_url =              ul->custom_referrer_url;
    comt_params->custom_content_transfer_encoding = ul->custom_content_transfer_encoding;
    comt_params->custom_user_agent =                ul->custom_user_agent;
    comt_params->target =                           NULL;
    comt_params->post_len =                         0;
    comt_params->post_data =                        NULL;

    struct pp_instance_s *pp_i = tables_get_pp_instance(ul->_.instance);
    if (ppb_core_is_main_thread()) {
        comt_params->should_wait = 0;
        _url_loader_open_comt(comt_params);
    } else {
        comt_params->should_wait = 1;
        pthread_barrier_init(&comt_params->barrier, NULL, 2);
        npn.pluginthreadasynccall(pp_i->npp, _url_loader_open_comt, comt_params);
        pthread_barrier_wait(&comt_params->barrier);
        pthread_barrier_destroy(&comt_params->barrier);
    }

    int retval = comt_params->retval;
    pp_resource_release(loader);
    g_slice_free(struct comt_param_s, comt_params);

    if (retval != NPERR_NO_ERROR)
        return PP_ERROR_FAILED;

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
    if (ul->fd >= 0) {
        struct stat sb;
        if (fstat(ul->fd, &sb) != 0) {
            pp_resource_release(loader);
            *bytes_received = -1;
            return PP_FALSE;
        }
        *bytes_received = sb.st_size;
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

    pp_resource_ref(loader);
    ri->url_loader_id = loader;
    ri->url_loader = ul;

    pp_resource_release(response_info);
    pp_resource_release(loader);
    return response_info;
}

int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    int read_bytes = 0;

    if (ul->fd >= 0) {
        lseek(ul->fd, ul->read_pos, SEEK_SET);
        read_bytes = RETRY_ON_EINTR(read(ul->fd, buffer, bytes_to_read));
        ul->read_pos += read_bytes;
    }

    if (read_bytes == 0) {
        // no data ready, schedule read task
        struct url_loader_read_task_s *rt = g_slice_alloc(sizeof(*rt));
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
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul)
        return PP_ERROR_BADRESOURCE;

    if (!ul->stream_to_file) {
        pp_resource_release(loader);
        return PP_ERROR_FAILED;
    }

    ul->stream_to_file_ccb = callback;
    // TODO: handle callback.func == NULL case

    pp_resource_release(loader);
    return PP_OK_COMPLETIONPENDING;
}

void
ppb_url_loader_close(PP_Resource loader)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul)
        return;

    if (ul->fd >= 0) {
        close(ul->fd);
        ul->fd = -1;
    }
    free_and_nullify(ul, headers);
    free_and_nullify(ul, url);
    pp_resource_release(loader);
    return;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_url_loader_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_loader_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_loader_is_url_loader(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_loader_is_url_loader(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s loader=%d, request_info=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}\n", __func__+6, loader, request_info, callback.func, callback.user_data,
               callback.flags);
    return ppb_url_loader_open(loader, request_info, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s loader=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_follow_redirect(loader, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                         int64_t *total_bytes_to_be_sent)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_upload_progress(loader, bytes_sent, total_bytes_to_be_sent);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                           int64_t *total_bytes_to_be_received)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_download_progress(loader, bytes_received, total_bytes_to_be_received);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_url_loader_get_response_info(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_response_info(loader);
}

TRACE_WRAPPER
int32_t
trace_ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s loader=%d, buffer=%p, bytes_to_read=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%d}\n", __func__+6, loader, buffer, bytes_to_read,
               callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_read_response_body(loader, buffer, bytes_to_read, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_url_loader_finish_streaming_to_file(PP_Resource loader,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s loader=%d callback={.func=%p, .user_data=%p, flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_finish_streaming_to_file(loader, callback);
}

TRACE_WRAPPER
void
trace_ppb_url_loader_close(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    ppb_url_loader_close(loader);
}


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create =                   TWRAPF(ppb_url_loader_create),
    .IsURLLoader =              TWRAPF(ppb_url_loader_is_url_loader),
    .Open =                     TWRAPF(ppb_url_loader_open),
    .FollowRedirect =           TWRAPF(ppb_url_loader_follow_redirect),
    .GetUploadProgress =        TWRAPZ(ppb_url_loader_get_upload_progress),
    .GetDownloadProgress =      TWRAPF(ppb_url_loader_get_download_progress),
    .GetResponseInfo =          TWRAPF(ppb_url_loader_get_response_info),
    .ReadResponseBody =         TWRAPF(ppb_url_loader_read_response_body),
    .FinishStreamingToFile =    TWRAPF(ppb_url_loader_finish_streaming_to_file),
    .Close =                    TWRAPF(ppb_url_loader_close),
};
