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

#ifndef FPP_PPB_URL_REQUEST_INFO_H
#define FPP_PPB_URL_REQUEST_INFO_H

#include <ppapi/c/ppb_url_request_info.h>
#include <stdlib.h>
#include <glib.h>
#include <stdio.h>


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
post_data_write_to_fp(GArray *post_data, guint idx, FILE *fp);

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

#endif // FPP_PPB_URL_REQUEST_INFO_H
