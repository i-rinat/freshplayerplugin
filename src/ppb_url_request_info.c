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


PP_Resource
ppb_url_request_info_create(PP_Instance instance)
{
    return pp_resource_allocate(PP_RESOURCE_URL_REQUEST_INFO, instance);
}

void
ppb_url_request_info_destroy(void *p)
{
    struct pp_url_request_info_s *ri = p;
    if (!ri)
        return;

    FREE_HELPER(ri, url);
    FREE_HELPER(ri, headers);
    FREE_HELPER(ri, custom_referrer_url);
    FREE_HELPER(ri, custom_content_transfer_encoding);
    FREE_HELPER(ri, custom_user_agent);
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
    char *tmp;
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
        if (ri->url)
            free(ri->url);
        ri->url = strdup((void*)(size_t)value.value.as_id);
        break;
    case PP_URLREQUESTPROPERTY_METHOD:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        tmp = (void *)(size_t)value.value.as_id;
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
        if (ri->headers)
            free(ri->headers);
        ri->headers = strdup((void *)(size_t)value.value.as_id);
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
        if (ri->custom_referrer_url)
            free(ri->custom_referrer_url);
        ri->custom_referrer_url = strdup((void *)(size_t)value.value.as_id);
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
        if (ri->custom_content_transfer_encoding)
            free(ri->custom_content_transfer_encoding);
        ri->custom_content_transfer_encoding = strdup((void *)(size_t)value.value.as_id);
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
        if (ri->custom_user_agent)
            free(ri->custom_user_agent);
        ri->custom_user_agent = strdup((void *)(size_t)value.value.as_id);
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
    return PP_TRUE;
}

PP_Bool
ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                         int64_t start_offset, int64_t number_of_bytes,
                                         PP_Time expected_last_modified_time)
{
    return PP_TRUE;
}


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
    free(value_str);
    return ppb_url_request_info_set_property(request, property, value);
}

static
PP_Bool
trace_ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len)
{
    trace_info("[PPB] {zilch} %s request=%d, data=%p, len=%u\n", __func__+6, request, data, len);
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


const struct PPB_URLRequestInfo_1_0 ppb_url_request_info_interface_1_0 = {
    .Create =           trace_ppb_url_request_info_create,
    .IsURLRequestInfo = trace_ppb_url_request_info_is_url_request_info,
    .SetProperty =      trace_ppb_url_request_info_set_property,
    .AppendDataToBody = trace_ppb_url_request_info_append_data_to_body,
    .AppendFileToBody = trace_ppb_url_request_info_append_file_to_body,
};
