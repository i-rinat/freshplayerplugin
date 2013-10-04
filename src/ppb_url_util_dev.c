#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <stddef.h>
#include <string.h>
#include "trace.h"
#include "utils.h"


static
struct PP_Var
ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
struct PP_Var
ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{zilch} %s\n", __func__);
    return var;
}

static
struct PP_Var
ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                              struct PP_URLComponents_Dev *components)
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
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{part} %s\n        instance = %d\n", __func__, instance);
    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup("http://127.0.0.1/flashtest/");
    ref_var(var);
    if (components) {
        components->scheme.begin = 1;
        components->scheme.len = 4;
        components->username.begin = 0;
        components->username.len = -1;
        components->password.begin = 0;
        components->password.len = -1;
        components->host.begin = 8;
        components->host.len = 9;
        components->port.begin = 0;
        components->port.len = -1;
        components->path.begin = 17;
        components->path.len = 11;
        components->query.begin = 0;
        components->query.len = -1;
        components->ref.begin = 0;
        components->ref.len = -1;
    }

    return var;
}

static
struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{part} %s\n        instance = %d\n", __func__, instance);
    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup("http://127.0.0.1/flashtest/flowplayer-3.2.16.swf");
    ref_var(var);
    if (components) {
        components->scheme.begin = 1;
        components->scheme.len = 4;
        components->username.begin = 0;
        components->username.len = -1;
        components->password.begin = 0;
        components->password.len = -1;
        components->host.begin = 8;
        components->host.len = 9;
        components->port.begin = 0;
        components->port.len = -1;
        components->path.begin = 17;
        components->path.len = 32;
        components->query.begin = 0;
        components->query.len = -1;
        components->ref.begin = 0;
        components->ref.len = -1;
    }

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
