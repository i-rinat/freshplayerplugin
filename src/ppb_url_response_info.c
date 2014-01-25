#include <ppapi/c/ppb_url_response_info.h>
#include <stddef.h>
#include "trace.h"
#include "reverse_constant.h"


PP_Bool
ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    return PP_TRUE;
}

struct PP_Var
ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    struct PP_Var var = {0};
    return var;
}

PP_Resource
ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    return 0;
}

// trace wrappers
static
PP_Bool
trace_ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_url_response_info_is_url_response_info(resource);
}

static
struct PP_Var
trace_ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    trace_info("[PPB] {zilch} %s response=%d, property=%s\n", __func__+6, response,
               reverse_pp_url_response_property(property));
    return ppb_url_response_info_get_property(response, property);
}

static
PP_Resource
trace_ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    trace_info("[PPB] {zilch} %s response=%d\n", __func__+6, response);
    return ppb_url_response_info_get_body_as_file_ref(response);
}


const struct PPB_URLResponseInfo_1_0 ppb_url_response_info_interface_1_0 = {
    .IsURLResponseInfo =    trace_ppb_url_response_info_is_url_response_info,
    .GetProperty =          trace_ppb_url_response_info_get_property,
    .GetBodyAsFileRef =     trace_ppb_url_response_info_get_body_as_file_ref,
};
