#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <stddef.h>
#include <string.h>
#include <Uri.h>
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
void
parse_url_string(const char *s, struct PP_URLComponents_Dev *components)
{
    UriParserStateA ups;
    UriUriA uri;

    ups.uri = &uri;
    if (uriParseUriA(&ups, s) != URI_SUCCESS) {
        uriFreeUriMembersA(&uri);
        trace_warning("failure at %s\n", __func__);
        return;
    }

#ifdef C_PARSE
#error conflicting defininition of C_PARSE
#endif

#define C_PARSE(c1, c2) \
    components->c1.begin = uri.c2.first ? uri.c2.first - s + 1 : 0; \
    components->c1.len = uri.c2.first ? uri.c2.afterLast - uri.c2.first : -1;

    C_PARSE(scheme, scheme);
    C_PARSE(username, userInfo);
    C_PARSE(password, userInfo);
    if (components->username.begin > 0) {
        const char *first = s + components->username.begin - 1;
        const char *last = first + components->username.len;
        const char *ptr = first;
        while (ptr < last) {
            if (*ptr == ':') {
                components->username.len = ptr - first;
                components->password.begin += ptr - first + 1;
                components->password.len -= ptr - first + 1;
                if (components->username.len == 0) {
                    components->username.begin = 0;
                    components->username.len = -1;
                }
                if (components->password.len == 0) {
                    components->password.begin = 0;
                    components->password.len = -1;
                }
                break;
            }
            ptr ++;
        }
    }
    C_PARSE(host, hostText);
    C_PARSE(port, portText);
    components->path.begin = uri.pathHead ? uri.pathHead->text.first - s + 1 : 0;
    components->path.len = uri.pathHead ? uri.pathTail->text.afterLast - uri.pathHead->text.first : -1;
    if (components->path.begin > 0) {
        components->path.begin -= 1;
        components->path.len += 1;
    }
    C_PARSE(query, query);
    C_PARSE(ref, fragment);

#undef C_PARSE

    uriFreeUriMembersA(&uri);
}

static
struct PP_Var
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{fake} %s\n        instance = %d\n", __func__, instance);
    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup("http://127.0.0.1/flashtest/");
    ref_var(var);
    if (components) {
        parse_url_string((void*)(size_t)var.value.as_id, components);
    }

    return var;
}

static
struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    trace_info("{fake} %s\n        instance = %d\n", __func__, instance);
    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup("http://127.0.0.1/flashtest/flowplayer-3.2.16.swf");
    ref_var(var);
    if (components) {
        parse_url_string((void*)(size_t)var.value.as_id, components);
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
