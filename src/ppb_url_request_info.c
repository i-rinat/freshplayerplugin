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

#include <assert.h>
#include "ppb_url_request_info.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "trace.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include "ppb_var.h"


PP_Resource
ppb_url_request_info_create(PP_Instance instance)
{
    PP_Resource request_info = pp_resource_allocate(PP_RESOURCE_URL_REQUEST_INFO, instance);
    struct pp_url_request_info_s *ri =
                            pp_resource_acquire(request_info, PP_RESOURCE_URL_REQUEST_INFO);

    ri->method = PP_METHOD_UNKNOWN;
    ri->url = NULL;
    ri->headers = NULL;
    ri->stream_to_file = PP_FALSE;
    ri->follow_redirects = PP_TRUE;
    ri->record_download_progress = PP_FALSE;
    ri->record_upload_progress = PP_FALSE;
    ri->custom_referrer_url = NULL;
    ri->allow_cross_origin_requests = PP_FALSE;
    ri->allow_credentials = PP_FALSE;
    ri->custom_content_transfer_encoding = NULL;
    ri->prefetch_buffer_upper_threshold = -1;
    ri->prefetch_buffer_lower_threshold = -1;
    ri->custom_user_agent = NULL;

    pp_resource_release(request_info);
    return request_info;
}

void
ppb_url_request_info_destroy(void *p)
{
    struct pp_url_request_info_s *ri = p;
    if (!ri)
        return;

    free_and_nullify(ri, url);
    free_and_nullify(ri, headers);
    free_and_nullify(ri, custom_referrer_url);
    free_and_nullify(ri, custom_content_transfer_encoding);
    free_and_nullify(ri, custom_user_agent);
    free_and_nullify(ri, post_data);
}

PP_Bool
ppb_url_request_info_is_url_request_info(PP_Resource resource)
{
    return PP_RESOURCE_URL_REQUEST_INFO == pp_resource_get_type(resource);
}

PP_Bool
ppb_url_request_info_set_property(PP_Resource request, PP_URLRequestProperty property,
                                  struct PP_Var value)
{
    const char *tmp;
    PP_Bool retval = PP_TRUE;
    struct pp_url_request_info_s *ri = pp_resource_acquire(request, PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, %d is not a request info\n", __func__, request);
        return PP_FALSE;
    }

#define ENSURE_TYPE(vartype) if (value.type != vartype) { retval = PP_FALSE; break; }

    switch (property) {
    case PP_URLREQUESTPROPERTY_URL:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri, url);
        ri->url = strdup(ppb_var_var_to_utf8(value, NULL));

        // replace all ' ' with '+'
        for (char *ptr = ri->url; *ptr != 0; ptr ++) {
            if (*ptr == ' ')
                *ptr = '+';
        }
        break;
    case PP_URLREQUESTPROPERTY_METHOD:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        tmp = ppb_var_var_to_utf8(value, NULL);
        if (strcmp(tmp, "GET") == 0) {
            ri->method = PP_METHOD_GET;
        } else if (strcmp(tmp, "POST") == 0) {
            ri->method = PP_METHOD_POST;
        } else {
            trace_warning("%s, unknown method %s\n", __func__, tmp);
            ri->method = PP_METHOD_UNKNOWN;
        }
        break;
    case PP_URLREQUESTPROPERTY_HEADERS:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri, headers);
        ri->headers = strdup(ppb_var_var_to_utf8(value, NULL));
        break;
    case PP_URLREQUESTPROPERTY_STREAMTOFILE:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->stream_to_file = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_FOLLOWREDIRECTS:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->follow_redirects = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->record_download_progress = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_RECORDUPLOADPROGRESS:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->record_upload_progress = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMREFERRERURL:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri, custom_referrer_url);
        ri->custom_referrer_url = strdup(ppb_var_var_to_utf8(value, NULL));
        break;
    case PP_URLREQUESTPROPERTY_ALLOWCROSSORIGINREQUESTS:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->allow_cross_origin_requests = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_ALLOWCREDENTIALS:
        ENSURE_TYPE(PP_VARTYPE_BOOL);
        ri->allow_credentials = value.value.as_bool;
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMCONTENTTRANSFERENCODING:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri, custom_content_transfer_encoding);
        ri->custom_content_transfer_encoding = strdup(ppb_var_var_to_utf8(value, NULL));
        break;
    case PP_URLREQUESTPROPERTY_PREFETCHBUFFERUPPERTHRESHOLD:
        ENSURE_TYPE(PP_VARTYPE_INT32);
        ri->prefetch_buffer_upper_threshold = value.value.as_int;
        break;
    case PP_URLREQUESTPROPERTY_PREFETCHBUFFERLOWERTHRESHOLD:
        ENSURE_TYPE(PP_VARTYPE_INT32);
        ri->prefetch_buffer_lower_threshold = value.value.as_int;
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMUSERAGENT:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri, custom_user_agent);
        ri->custom_user_agent = strdup(ppb_var_var_to_utf8(value, NULL));
        break;
    default:
        trace_error("%s, unknown url request property %d\n", __func__, property);
        retval = PP_FALSE;
        break;
    }

    pp_resource_release(request);
    return retval;
}

PP_Bool
ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len)
{
    struct pp_url_request_info_s *ri = pp_resource_acquire(request, PP_RESOURCE_URL_REQUEST_INFO);
    PP_Bool retval = PP_FALSE;

    free_and_nullify(ri, post_data);
    ri->post_len = 0;

    ri->post_data = malloc(len);
    if (ri->post_data) {
        memcpy(ri->post_data, data, len);
        ri->post_len = len;
        retval = PP_TRUE;
    } else {
        retval = PP_FALSE;
    }

    pp_resource_release(request);
    return retval;
}

PP_Bool
ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                         int64_t start_offset, int64_t number_of_bytes,
                                         PP_Time expected_last_modified_time)
{
    return PP_TRUE;
}


#ifndef NDEBUG
// trace wrappers
static
PP_Resource
trace_ppb_url_request_info_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_request_info_create(instance);
}

static
PP_Bool
trace_ppb_url_request_info_is_url_request_info(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_request_info_is_url_request_info(resource);
}

static
PP_Bool
trace_ppb_url_request_info_set_property(PP_Resource request, PP_URLRequestProperty property,
                                        struct PP_Var value)
{
    char *value_str = trace_var_as_string(value);
    trace_info("[PPB] {full} %s request=%d, property=%s, value=%s\n", __func__+6, request,
               reverse_pp_url_request_property(property), value_str);
    g_free(value_str);
    return ppb_url_request_info_set_property(request, property, value);
}

static
PP_Bool
trace_ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len)
{
    trace_info("[PPB] {full} %s request=%d, data=%p, len=%u\n", __func__+6, request, data, len);
    return ppb_url_request_info_append_data_to_body(request, data, len);
}

static
PP_Bool
trace_ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                               int64_t start_offset, int64_t number_of_bytes,
                                               PP_Time expected_last_modified_time)
{
    trace_info("[PPB] {zilch} %s request=%d, file_ref=%d, start_offset=%"PRId64", "
               "number_of_bytes=%"PRId64", expected_last_modified_time=%f\n", __func__+6, request,
               file_ref, start_offset, number_of_bytes, expected_last_modified_time);
    return ppb_url_request_info_append_file_to_body(request, file_ref, start_offset,
                                                    number_of_bytes, expected_last_modified_time);
}
#endif // NDEBUG


const struct PPB_URLRequestInfo_1_0 ppb_url_request_info_interface_1_0 = {
    .Create =           TWRAP(ppb_url_request_info_create),
    .IsURLRequestInfo = TWRAP(ppb_url_request_info_is_url_request_info),
    .SetProperty =      TWRAP(ppb_url_request_info_set_property),
    .AppendDataToBody = TWRAP(ppb_url_request_info_append_data_to_body),
    .AppendFileToBody = TWRAP(ppb_url_request_info_append_file_to_body),
};
