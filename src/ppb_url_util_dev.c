#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <stddef.h>
#include "trace.h"


static
struct PP_Var
ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev* components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
struct PP_Var
ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                         struct PP_URLComponents_Dev* components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
struct PP_Var
ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                              struct PP_URLComponents_Dev* components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
PP_Bool
ppb_url_util_dev_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
PP_Bool
ppb_url_util_dev_document_can_request(PP_Instance instance, struct PP_Var url)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
PP_Bool
ppb_url_util_dev_document_can_access_document(PP_Instance active, PP_Instance target)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
struct PP_Var
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev* components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev* components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

const struct PPB_URLUtil_Dev_0_6 ppb_url_util_dev_interface_0_6 = {
    .Canonicalize = ppb_url_util_dev_canonicalize,
    .ResolveRelativeToURL = ppb_url_util_dev_resolve_relative_to_url,
    .ResolveRelativeToDocument = ppb_url_util_dev_resolve_relative_to_document,
    .IsSameSecurityOrigin = ppb_url_util_dev_is_same_security_origin,
    .DocumentCanRequest = ppb_url_util_dev_document_can_request,
    .DocumentCanAccessDocument = ppb_url_util_dev_document_can_access_document,
    .GetDocumentURL = ppb_url_util_dev_get_document_url,
    .GetPluginInstanceURL = ppb_url_util_dev_get_plugin_instance_url
};
