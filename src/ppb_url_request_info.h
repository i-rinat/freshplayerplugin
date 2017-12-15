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
#include <ppapi/c/ppb_url_request_info.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct pp_url_request_info_s {
    COMMON_STRUCTURE_FIELDS
    enum pp_request_method_e    method;
    char                       *url;
    char                       *headers;
    unsigned int                is_immediate_javascript; // data in url is javascript line
    PP_Bool                     stream_to_file;
    PP_Bool                     follow_redirects;
    PP_Bool                     record_download_progress;
    PP_Bool                     record_upload_progress;
    char                       *custom_referrer_url;
    PP_Bool                     allow_cross_origin_requests;
    PP_Bool                     allow_credentials;
    char                       *custom_content_transfer_encoding;
    int32_t                     prefetch_buffer_upper_threshold;
    int32_t                     prefetch_buffer_lower_threshold;
    char                       *custom_user_agent;
    GArray                     *post_data;
};

/// descriptor of post data item
struct post_data_item_s {
    char       *data;
    size_t      len;

    PP_Resource file_ref;           ///< non-zero if the item is file slice
    int64_t     start_offset;
    int64_t     number_of_bytes;
    PP_Time     expected_last_modified_time;
};

GArray *
post_data_new(void);

GArray *
post_data_duplicate(GArray *post_data);

void
post_data_free(GArray *post_data);

size_t
post_data_get_all_item_length(GArray *post_data);

void
post_data_write_to_gstring(GArray *post_data, guint idx, GString *buf);

PP_Resource
ppb_url_request_info_create(PP_Instance instance);

PP_Bool
ppb_url_request_info_is_url_request_info(PP_Resource resource);

PP_Bool
ppb_url_request_info_set_property(PP_Resource request, PP_URLRequestProperty property,
                                  struct PP_Var value);

PP_Bool
ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len);

PP_Bool
ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                         int64_t start_offset, int64_t number_of_bytes,
                                         PP_Time expected_last_modified_time);
