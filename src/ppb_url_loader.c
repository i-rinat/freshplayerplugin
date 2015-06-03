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

#define _XOPEN_SOURCE   600
#include "ppb_url_loader.h"
#include "ppb_core.h"
#include "ppb_url_util.h"
#include "ppb_var.h"
#include "ppb_message_loop.h"
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
#include "ppb_url_request_info.h"
#include "config.h"
#include "pp_interface.h"


PP_Resource
ppb_url_loader_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource url_loader = pp_resource_allocate(PP_RESOURCE_URL_LOADER, pp_i);
    struct pp_url_loader_s *ul = pp_resource_acquire(url_loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    // all fields are zeroed by default
    ul->response_size = -1;
    ul->method = PP_METHOD_GET;
    ul->fd = -1;

    pp_resource_release(url_loader);
    return url_loader;
}

static
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
    free_and_nullify(ul->headers);
    free_and_nullify(ul->url);
    free_and_nullify(ul->status_line);
    free_and_nullify(ul->request_headers);
    free_and_nullify(ul->custom_referrer_url);
    free_and_nullify(ul->custom_content_transfer_encoding);
    free_and_nullify(ul->custom_user_agent);
    free_and_nullify(ul->target);

    post_data_free(ul->post_data);
    ul->post_data = NULL;

    while (ul->read_tasks) {
        GList *llink = g_list_first(ul->read_tasks);
        struct url_loader_read_task_s *rt = llink->data;
        ul->read_tasks = g_list_delete_link(ul->read_tasks, llink);

        g_slice_free1(sizeof(*rt), rt);
    }
}

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource)
{
    return PP_RESOURCE_URL_LOADER == pp_resource_get_type(resource);
}

struct url_loader_open_param_s {
    const char                 *url;
    PP_Resource                 loader;
    PP_Instance                 instance_id;
    enum pp_request_method_e    method;
    const char                 *request_headers;
    const char                 *custom_referrer_url;
    const char                 *custom_content_transfer_encoding;
    const char                 *custom_user_agent;
    const char                 *target;
    GArray                     *post_data;
    size_t                      post_len;
    PP_Resource                 m_loop;
    int                         depth;
    int                         retval;
};

static
void
url_loader_open_ptac(void *user_data)
{
    struct url_loader_open_param_s *p = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance_id);

    if (!pp_i) {
        p->retval = NPERR_INVALID_INSTANCE_ERROR;
        goto quit;
    }

    if (p->method == PP_METHOD_POST) {
        // POST request
        char   *tmpfname;
        int     fd;
        FILE   *fp = NULL;
        int     need_newline = 0;

        tmpfname = g_strdup_printf("/tmp/FreshPostBodyXXXXXX");    // TODO: make configurable

        fd = mkstemp(tmpfname);
        if (fd < 0) {
            p->retval = NPERR_GENERIC_ERROR;
            goto err;
        }

        fp = fdopen(fd, "wb+");
        if (!fp) {
            close(fd);
            p->retval = NPERR_GENERIC_ERROR;
            goto err;
        }

        if (p->request_headers) {
            fprintf(fp, "%s\n", p->request_headers);
            need_newline = 1;
        }
        if (p->custom_referrer_url) {
            // the header name should be "referer", that's how it's spelled in HTTP spec
            fprintf(fp, "Referer: %s\n", p->custom_referrer_url);
            need_newline = 1;
        }
        if (p->custom_content_transfer_encoding) {
            fprintf(fp, "Content-Transfer-Encoding: %s\n", p->custom_content_transfer_encoding);
            need_newline = 1;
        }
        if (p->custom_user_agent) {
            fprintf(fp, "User-Agent: %s\n", p->custom_user_agent);
            need_newline = 1;
        }

        if (p->post_data) {
            size_t post_len = post_data_get_all_item_length(p->post_data);

            if (post_len == (size_t)-1) {
                // PP_ERROR_FILECHANGED?
                goto err;
            }

            if (post_len > 0) {
                fprintf(fp, "Content-Length: %"PRIu64"\n", (uint64_t)post_len);
                need_newline = 1;
            }
        }

        if (need_newline)
            fprintf(fp, "\n");

        if (p->post_data) {
            for (guint k = 0; k < p->post_data->len; k ++)
                post_data_write_to_fp(p->post_data, k, fp);
        }

        fclose(fp); // flush all unwritten data and close the file
        fp = NULL;  // avoid calling fclose() twice

        if (p->target) {
            p->retval = npn.posturl(pp_i->npp, p->url, p->target, strlen(tmpfname), tmpfname, true);
        } else {
            p->retval = npn.posturlnotify(pp_i->npp, p->url, NULL, strlen(tmpfname), tmpfname, true,
                                          (void*)(size_t)p->loader);
        }
err:
        if (fp)
            fclose(fp);
        unlink(tmpfname);
        g_free(tmpfname);
    } else {
        // GET request
        if (p->target) {
            p->retval = npn.geturl(pp_i->npp, p->url, p->target);
        } else {
            p->retval = npn.geturlnotify(pp_i->npp, p->url, NULL, (void*)(size_t)p->loader);
        }
    }

quit:
    ppb_core_release_resource(p->loader);
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
url_loader_open_comt(void *user_data, int32_t result)
{
    struct url_loader_open_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, url_loader_open_ptac, p);
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
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r')) {
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
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }
    struct pp_url_request_info_s *ri = pp_resource_acquire(request_info,
                                                           PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        pp_resource_release(loader);
        return PP_ERROR_BADRESOURCE;
    }
    struct PP_Var full_url;

    if (ri->is_immediate_javascript) {
        full_url = ppb_var_var_from_utf8_z(ri->url);
    } else {
        struct PP_Var rel_url = ppb_var_var_from_utf8_z(ri->url);
        full_url = ppb_url_util_resolve_relative_to_document(ul->instance->id, rel_url, NULL);
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

    post_data_free(ul->post_data);
    ul->post_data = post_data_duplicate(ri->post_data);

    ul->fd = open_temporary_file();
    ul->ccb = callback;

    ppb_var_release(full_url);
    pp_resource_release(request_info);

    if (config.quirks.connect_first_loader_to_unrequested_stream) {
        if (ul->instance->content_url_loader == 0) {
            ul->instance->content_url_loader = loader;
            pp_resource_release(loader);
            return PP_OK_COMPLETIONPENDING;
        }
    }

    struct url_loader_open_param_s *p = g_slice_alloc(sizeof(*p));
    p->url =                ul->url;
    p->loader =             loader;
    p->instance_id =        ul->instance->id;
    p->method =             ul->method;
    p->request_headers =    ul->request_headers;
    p->custom_referrer_url = ul->custom_referrer_url;
    p->custom_content_transfer_encoding = ul->custom_content_transfer_encoding;
    p->custom_user_agent =  ul->custom_user_agent;
    p->target =         ul->target;
    p->post_data =      ul->post_data;
    p->m_loop =         ppb_message_loop_get_current();
    p->depth =          ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_core_add_ref_resource(loader);  // add ref to ensure data in ul remain accessible
    pp_resource_release(loader);

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(url_loader_open_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    int retval = p->retval;
    g_slice_free1(sizeof(*p), p);

    if (retval != NPERR_NO_ERROR)
        return PP_ERROR_FAILED;

    if (callback.func == NULL) {
        int done = 0;
        while (!done) {
            ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
            if (ul) {
                done = ul->finished_loading;
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
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }
    char *new_url = nullsafe_strdup(ul->redirect_url);

    free_and_nullify(ul->url);
    free_and_nullify(ul->redirect_url);
    free_and_nullify(ul->status_line);
    free_and_nullify(ul->headers);
    free_and_nullify(ul->request_headers);
    post_data_free(ul->post_data);
    ul->post_data = NULL;

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

    struct url_loader_open_param_s *p = g_slice_alloc(sizeof(*p));
    p->url =                ul->url;
    p->loader =             loader;
    p->instance_id =        ul->instance->id;
    p->method =             ul->method;
    p->request_headers =    ul->request_headers;
    p->custom_referrer_url = ul->custom_referrer_url;
    p->custom_content_transfer_encoding =  ul->custom_content_transfer_encoding;
    p->custom_user_agent =  ul->custom_user_agent;
    p->target =             NULL;
    p->post_len =           0;
    p->post_data =          NULL;
    p->m_loop =             ppb_message_loop_get_current();
    p->depth =              ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_core_add_ref_resource(loader);  // add ref to ensure data in ul remain accessible
    pp_resource_release(loader);

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(url_loader_open_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    int retval = p->retval;
    g_slice_free1(sizeof(*p), p);

    if (retval != NPERR_NO_ERROR)
        return PP_ERROR_FAILED;

    if (callback.func == NULL) {
        int done = 0;
        while (!done) {
            ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
            if (ul) {
                done = ul->finished_loading;
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
    // TODO: how to get upload progress
    // Apparently there is no way to determine upload progress using NPAPI.
    // However, one can try to create anonymous fifo and push data to it.
    // How particularly browser handles file is still to be discovered. Note,
    // each browser can do it differently.
    return PP_FALSE;
}

PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

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
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    PP_Resource response_info = pp_resource_allocate(PP_RESOURCE_URL_RESPONSE_INFO, ul->instance);
    struct pp_url_response_info_s *ri = pp_resource_acquire(response_info,
                                                            PP_RESOURCE_URL_RESPONSE_INFO);
    if (!ri) {
        trace_error("%s, resource allocation failure\n", __func__);
        pp_resource_release(loader);
        return 0;
    }

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
    struct url_loader_read_task_s *rt;
    int32_t read_bytes = PP_ERROR_FAILED;
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (ul->fd == -1) {
        trace_error("%s, fd==-1\n", __func__);
        pp_resource_release(loader);
        return PP_ERROR_FAILED;
    }

    if (ul->read_tasks) {
        // schedule task instead of immediate reading if there is another task
        // in the queue already
        goto schedule_read_task;
    }

    read_bytes = -1;
    off_t ofs = lseek(ul->fd, ul->read_pos, SEEK_SET);
    if (ofs != (off_t)-1)
        read_bytes = RETRY_ON_EINTR(read(ul->fd, buffer, bytes_to_read));

    if (read_bytes < 0)
        read_bytes = PP_ERROR_FAILED;
    else
        ul->read_pos += read_bytes;

    if (read_bytes == 0 && !ul->finished_loading) {
        // no data ready, schedule read task
        goto schedule_read_task;
    }

    pp_resource_release(loader);
    if (callback.flags & PP_COMPLETIONCALLBACK_FLAG_OPTIONAL)
        return read_bytes;

    ppb_core_call_on_main_thread2(0, callback, read_bytes, __func__);
    return PP_OK_COMPLETIONPENDING;

schedule_read_task:
    rt = g_slice_alloc(sizeof(*rt));
    rt->url_loader =    loader;
    rt->buffer =        buffer;
    rt->bytes_to_read = bytes_to_read;
    rt->ccb =           callback;

    ul->read_tasks = g_list_append(ul->read_tasks, rt);
    pp_resource_release(loader);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback)
{
    struct pp_url_loader_s *ul = pp_resource_acquire(loader, PP_RESOURCE_URL_LOADER);
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!ul->stream_to_file) {
        trace_error("%s, not streaming to file\n", __func__);
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
    if (!ul) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    if (ul->fd >= 0) {
        close(ul->fd);
        ul->fd = -1;
    }
    free_and_nullify(ul->headers);
    free_and_nullify(ul->url);
    pp_resource_release(loader);
    return;
}

void
ppb_url_loader_grant_universal_access(PP_Resource loader)
{
    trace_info_f("      no-op implementation, all request are always allowed\n");
    return;
}

void
ppb_url_loader_register_status_callback(PP_Resource loader, PP_URLLoaderTrusted_StatusCallback cb)
{
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
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
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

TRACE_WRAPPER
void
trace_ppb_url_loader_grant_universal_access(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    ppb_url_loader_grant_universal_access(loader);
}

TRACE_WRAPPER
void
trace_ppb_url_loader_register_status_callback(PP_Resource loader,
                                              PP_URLLoaderTrusted_StatusCallback cb)
{
    trace_info("[PPB] {zilch} %s loader=%d, cb=%p\n", __func__+6, loader, cb);
    ppb_url_loader_register_status_callback(loader, cb);
}


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create =                   TWRAPF(ppb_url_loader_create),
    .IsURLLoader =              TWRAPF(ppb_url_loader_is_url_loader),
    .Open =                     TWRAPF(ppb_url_loader_open),
    .FollowRedirect =           TWRAPF(ppb_url_loader_follow_redirect),
    .GetUploadProgress =        TWRAPF(ppb_url_loader_get_upload_progress),
    .GetDownloadProgress =      TWRAPF(ppb_url_loader_get_download_progress),
    .GetResponseInfo =          TWRAPF(ppb_url_loader_get_response_info),
    .ReadResponseBody =         TWRAPF(ppb_url_loader_read_response_body),
    .FinishStreamingToFile =    TWRAPF(ppb_url_loader_finish_streaming_to_file),
    .Close =                    TWRAPF(ppb_url_loader_close),
};

const struct PPB_URLLoaderTrusted_0_3 ppb_url_loader_trusted_interface_0_3 = {
    .GrantUniversalAccess =     TWRAPF(ppb_url_loader_grant_universal_access),
    .RegisterStatusCallback =   TWRAPZ(ppb_url_loader_register_status_callback),
};

static
void
__attribute__((constructor))
constructor_ppb_url_loader(void)
{
    register_interface(PPB_URLLOADER_INTERFACE_1_0, &ppb_url_loader_interface_1_0);
    register_interface(PPB_URLLOADERTRUSTED_INTERFACE_0_3, &ppb_url_loader_trusted_interface_0_3);
    register_resource(PP_RESOURCE_URL_LOADER, ppb_url_loader_destroy);
}
