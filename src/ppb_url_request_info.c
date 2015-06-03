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

#include <assert.h>
#include "ppb_url_request_info.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include "ppb_var.h"
#include <ppapi/c/pp_file_info.h>
#include <ppapi/c/ppb_file_io.h>
#include <ppapi/c/pp_errors.h>
#include "ppb_flash_file.h"
#include "ppb_core.h"
#include "eintr_retry.h"
#include "pp_interface.h"


PP_Resource
ppb_url_request_info_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource request_info = pp_resource_allocate(PP_RESOURCE_URL_REQUEST_INFO, pp_i);
    struct pp_url_request_info_s *ri =
                            pp_resource_acquire(request_info, PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

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
    ri->post_data = post_data_new();

    pp_resource_release(request_info);
    return request_info;
}

static
void
ppb_url_request_info_destroy(void *p)
{
    struct pp_url_request_info_s *ri = p;
    if (!ri)
        return;

    free_and_nullify(ri->url);
    free_and_nullify(ri->headers);
    free_and_nullify(ri->custom_referrer_url);
    free_and_nullify(ri->custom_content_transfer_encoding);
    free_and_nullify(ri->custom_user_agent);
    post_data_free(ri->post_data);
    ri->post_data = NULL;
}

PP_Bool
ppb_url_request_info_is_url_request_info(PP_Resource resource)
{
    return PP_RESOURCE_URL_REQUEST_INFO == pp_resource_get_type(resource);
}

static
const char *
ltrim(const char *s)
{
    while (*s && isspace(*s))
        s++;
    return s;
}

PP_Bool
ppb_url_request_info_set_property(PP_Resource request, PP_URLRequestProperty property,
                                  struct PP_Var value)
{
    const char *tmp, *tmp2;
    PP_Bool retval = PP_TRUE;
    struct pp_url_request_info_s *ri = pp_resource_acquire(request, PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

#define ENSURE_TYPE(vartype) if (value.type != vartype) { retval = PP_FALSE; break; }

    switch (property) {
    case PP_URLREQUESTPROPERTY_URL:
        ENSURE_TYPE(PP_VARTYPE_STRING);
        free_and_nullify(ri->url);
        tmp = ppb_var_var_to_utf8(value, NULL);
        tmp2 = ltrim(tmp);
        ri->url = strdup(tmp2);
        ri->is_immediate_javascript = (strncasecmp(tmp2, "javascript:", strlen("javascript:"))==0);
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
        free_and_nullify(ri->headers);
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
        free_and_nullify(ri->custom_referrer_url);
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
        free_and_nullify(ri->custom_content_transfer_encoding);
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
        free_and_nullify(ri->custom_user_agent);
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
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    PP_Bool retval = PP_FALSE;
    struct post_data_item_s pdi = { 0 };
    pdi.data = g_memdup(data, len);
    if (!pdi.data) {
        retval = PP_FALSE;
        goto err;
    }

    pdi.len = len;
    g_array_append_val(ri->post_data, pdi);
    retval = PP_TRUE;

err:
    pp_resource_release(request);
    return retval;
}

PP_Bool
ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                         int64_t start_offset, int64_t number_of_bytes,
                                         PP_Time expected_last_modified_time)
{
    struct pp_url_request_info_s *ri = pp_resource_acquire(request, PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    struct post_data_item_s pdi = { 0 };
    ppb_core_add_ref_resource(file_ref);
    pdi.file_ref = file_ref;
    pdi.start_offset = start_offset;
    pdi.number_of_bytes = number_of_bytes;
    pdi.expected_last_modified_time = expected_last_modified_time;
    g_array_append_val(ri->post_data, pdi);

    pp_resource_release(request);
    return PP_TRUE;
}

GArray *
post_data_new(void)
{
    return g_array_new(FALSE, TRUE, sizeof(struct post_data_item_s));
}

GArray *
post_data_duplicate(GArray *post_data)
{
    GArray *post_data2;

    if (!post_data)
        return NULL;

    post_data2 = post_data_new();
    if (!post_data2)
        return NULL;

    for (guint k = 0; k < post_data->len; k ++) {
        struct post_data_item_s *pdi = &g_array_index(post_data, struct post_data_item_s, k);
        struct post_data_item_s pdi2 = *pdi;

        if (pdi2.file_ref != 0) {
            ppb_core_add_ref_resource(pdi2.file_ref);
        } else {
            pdi2.data = g_memdup(pdi->data, pdi->len);
            if (!pdi2.data) {
                trace_error("%s, can't allocate memory\n", __func__);
                continue;
            }
        }

        g_array_append_val(post_data2, pdi2);
    }

    return post_data2;
}

static size_t
post_data_get_item_length(struct post_data_item_s *pdi)
{
    if (pdi->file_ref != 0) {
        size_t start_offset = MAX(pdi->start_offset, 0);
        struct PP_FileInfo finfo;

        if (ppb_flash_file_file_ref_query_file(pdi->file_ref, &finfo) != PP_OK)
            return (size_t)-1;

        if (pdi->expected_last_modified_time != 0
            && finfo.last_modified_time != pdi->expected_last_modified_time)
        {
            return (size_t)-1;
        }

        if (pdi->number_of_bytes >= 0) {
            return (size_t)pdi->number_of_bytes;
        } else if (finfo.size <= start_offset) {
            return 0;
        } else {
            return finfo.size - start_offset;
        }
    } else {
        return pdi->len;
    }
}

size_t
post_data_get_all_item_length(GArray *post_data)
{
    size_t total_len = 0;

    for (guint k = 0; k < post_data->len; k ++) {
        struct post_data_item_s *pdi = &g_array_index(post_data, struct post_data_item_s, k);
        size_t item_len = post_data_get_item_length(pdi);

        if (item_len == (size_t)-1)
            goto err;

        total_len += item_len;
    }

    return total_len;
err:
    return (size_t)-1;
}

void
post_data_free(GArray *post_data)
{
    if (!post_data)
        return;

    for (guint k = 0; k < post_data->len; k ++) {
        struct post_data_item_s *pdi = &g_array_index(post_data, struct post_data_item_s, k);
        if (pdi->file_ref != 0)
            ppb_core_release_resource(pdi->file_ref);
        else
            free(pdi->data);
    }

    post_data->len = 0; // set length to zero to avoid double freing
    g_array_unref(post_data);
}

void
post_data_write_to_fp(GArray *post_data, guint idx, FILE *fp)
{
    char buf[128 * 1024];
    struct post_data_item_s *pdi = &g_array_index(post_data, struct post_data_item_s, idx);

    if (pdi->file_ref != 0) {
        int fd = -1;
        if (ppb_flash_file_file_ref_open_file(pdi->file_ref, PP_FILEOPENFLAG_READ, &fd) != PP_OK) {
            // some error, skipping this one
            goto err;
        }

        size_t to_write = post_data_get_item_length(pdi);
        while (to_write > 0) {
            ssize_t read_bytes = RETRY_ON_EINTR(read(fd, buf, MIN(to_write, sizeof(buf))));
            if (read_bytes == -1)
                goto err;
            fwrite(buf, 1, (size_t)read_bytes, fp);
            to_write -= read_bytes;
        }
err:
        if (fd >= 0)
            close(fd);
    } else {
        fwrite(pdi->data, 1, pdi->len, fp);
    }
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_url_request_info_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_request_info_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_request_info_is_url_request_info(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_request_info_is_url_request_info(resource);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
PP_Bool
trace_ppb_url_request_info_append_data_to_body(PP_Resource request, const void *data, uint32_t len)
{
    trace_info("[PPB] {full} %s request=%d, data=%p, len=%u\n", __func__+6, request, data, len);
    return ppb_url_request_info_append_data_to_body(request, data, len);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_request_info_append_file_to_body(PP_Resource request, PP_Resource file_ref,
                                               int64_t start_offset, int64_t number_of_bytes,
                                               PP_Time expected_last_modified_time)
{
    trace_info("[PPB] {full} %s request=%d, file_ref=%d, start_offset=%"PRId64", "
               "number_of_bytes=%"PRId64", expected_last_modified_time=%f\n", __func__+6, request,
               file_ref, start_offset, number_of_bytes, expected_last_modified_time);
    return ppb_url_request_info_append_file_to_body(request, file_ref, start_offset,
                                                    number_of_bytes, expected_last_modified_time);
}


const struct PPB_URLRequestInfo_1_0 ppb_url_request_info_interface_1_0 = {
    .Create =           TWRAPF(ppb_url_request_info_create),
    .IsURLRequestInfo = TWRAPF(ppb_url_request_info_is_url_request_info),
    .SetProperty =      TWRAPF(ppb_url_request_info_set_property),
    .AppendDataToBody = TWRAPF(ppb_url_request_info_append_data_to_body),
    .AppendFileToBody = TWRAPF(ppb_url_request_info_append_file_to_body),
};

static
void
__attribute__((constructor))
constructor_ppb_url_request_info(void)
{
    register_interface(PPB_URLREQUESTINFO_INTERFACE_1_0, &ppb_url_request_info_interface_1_0);
    register_resource(PP_RESOURCE_URL_REQUEST_INFO, ppb_url_request_info_destroy);
}
