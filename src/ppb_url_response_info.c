#include <ppapi/c/ppb_url_response_info.h>
#include <stddef.h>
#include "trace.h"
#include "reverse_constant.h"


static
PP_Bool
ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__, resource);
    return PP_TRUE;
}

static
struct PP_Var
ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    struct PP_Var var = {0};
    trace_info("[PPB] {zilch} %s response=%d, property=%s\n", __func__, response,
               reverse_pp_url_response_property(property));
    return var;
}

static
PP_Resource
ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    trace_info("[PPB] {zilch} %s response=%d\n", __func__, response);
    return 0;
}


const struct PPB_URLResponseInfo_1_0 ppb_url_response_info_interface_1_0 = {
    .IsURLResponseInfo =    ppb_url_response_info_is_url_response_info,
    .GetProperty =          ppb_url_response_info_get_property,
    .GetBodyAsFileRef =     ppb_url_response_info_get_body_as_file_ref,
};
