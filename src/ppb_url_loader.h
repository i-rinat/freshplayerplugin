/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#pragma once

#include "pp_resource.h"
#include <glib.h>
#include <npapi/npapi.h>
#include <npapi/npruntime.h>
#include <ppapi/c/ppb_url_loader.h>
#include <ppapi/c/trusted/ppb_url_loader_trusted.h>

struct pp_url_loader_s {
    COMMON_STRUCTURE_FIELDS
    char                   *status_line;    ///< HTTP/1.1 200 OK
    char                   *headers;        ///< response headers
    int                     http_code;      ///< HTTP response code
    int                     fd;             ///< file used to store response
    size_t                  read_pos;       ///< reading position
    enum pp_request_method_e method;        ///< GET/POST
    char                   *url;            ///< request URL
    char                   *redirect_url;   ///< value of the Location header if this is
                                            ///< a redirection response
    int                     finished_loading;   ///< if whole stream loaded already
    int64_t                 response_size;  ///< Content-Length value of -1 if absent
    int                     stream_to_file; ///< whenever streaming to file is allowed
    struct PP_CompletionCallback    stream_to_file_ccb; ///< callback to call on end of streaming
    PP_Resource             stream_to_file_ccb_ml; ///< message loop to call callback on

    char                   *request_headers;
    PP_Bool                 follow_redirects;   ///< handle redirections internally
    PP_Bool                 record_download_progress;
    PP_Bool                 record_upload_progress;
    char                   *custom_referrer_url;
    PP_Bool                 allow_cross_origin_requests;
    PP_Bool                 allow_credentials;
    char                   *custom_content_transfer_encoding;
    char                   *custom_user_agent;
    char                   *target;
    GArray                 *post_data;
    GList                  *read_tasks;     ///< list of url_loader_read_task_s
    NPStream               *np_stream;      ///< associated NPStream
    struct PP_CompletionCallback    ccb;    ///< callback to call on headers arrival
    PP_Resource                     ccb_ml; ///< message loop to call callback on
};

struct url_loader_read_task_s {
    PP_Resource                     url_loader;
    void                           *buffer;
    int32_t                         bytes_to_read;
    struct PP_CompletionCallback    ccb;
    PP_Resource                     ccb_ml;
};

PP_Resource
ppb_url_loader_create(PP_Instance instance);

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource);

int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback);

int32_t
ppb_url_loader_open_target(PP_Resource loader, PP_Resource request_info,
                           struct PP_CompletionCallback callback, const char *target);

int32_t
ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback);

PP_Bool
ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                   int64_t *total_bytes_to_be_sent);

PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received);

PP_Resource
ppb_url_loader_get_response_info(PP_Resource loader);

int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback);

int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback);

void
ppb_url_loader_close(PP_Resource loader);

void
ppb_url_loader_grant_universal_access(PP_Resource loader);

void
ppb_url_loader_register_status_callback(PP_Resource loader,
                                        PP_URLLoaderTrusted_StatusCallback cb);
