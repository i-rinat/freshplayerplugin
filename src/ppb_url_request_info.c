#include <assert.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include "trace.h"
#include "pp_resource.h"
#include "reverse_constant.h"


PP_Resource
ppb_url_request_info_create(PP_Instance instance)
{
    return pp_resource_allocate(PP_RESOURCE_URL_REQUEST_INFO);
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
    switch (property) {
    case PP_URLREQUESTPROPERTY_URL:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_METHOD:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_HEADERS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_STREAMTOFILE:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_FOLLOWREDIRECTS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_RECORDUPLOADPROGRESS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMREFERRERURL:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_ALLOWCROSSORIGINREQUESTS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_ALLOWCREDENTIALS:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMCONTENTTRANSFERENCODING:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_PREFETCHBUFFERUPPERTHRESHOLD:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_PREFETCHBUFFERLOWERTHRESHOLD:
        assert(0);
        break;
    case PP_URLREQUESTPROPERTY_CUSTOMUSERAGENT:
        assert(0);
        break;
    default:
        assert(0);
        break;
    }
    return PP_TRUE;
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
    trace_info("[PPB] {zilch} %s request=%d, property=%s, value=%s\n", __func__+6, request,
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
