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

#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <stddef.h>
#include <string.h>
#include <Uri.h>
#include "trace.h"
#include "tables.h"


struct PP_Var
ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                              struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    return var;
}

PP_Bool
ppb_url_util_dev_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_util_dev_document_can_request(PP_Instance instance, struct PP_Var url)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_util_dev_document_can_access_document(PP_Instance active, PP_Instance target)
{
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
    components->path.len = uri.pathHead ? uri.pathTail->text.afterLast - uri.pathHead->text.first
                                        : -1;
    if (components->path.begin > 0) {
        components->path.begin -= 1;
        components->path.len += 1;
    }
    C_PARSE(query, query);
    C_PARSE(ref, fragment);

#undef C_PARSE

    uriFreeUriMembersA(&uri);
}

struct PP_Var
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};

    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup("http://127.0.0.1/flashtest/");
    tables_ref_var(var);
    if (components) {
        parse_url_string((void*)(size_t)var.value.as_id, components);
    }

    return var;
}

struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    struct pp_instance_s *pp_i = tables_pp_instance_to_np_priv(instance);

    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (int64_t)(void*)strdup(pp_i->instance_url);
    tables_ref_var(var);
    if (components) {
        parse_url_string((void*)(size_t)var.value.as_id, components);
    }

    return var;
}

struct PP_Var
ppb_url_util_dev_get_plugin_referrer_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    struct PP_Var var = {0};
    return var;
}

// trace wrappers
static
struct PP_Var
trace_ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_canonicalize(url, components);
}

static
struct PP_Var
trace_ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url,
                                               struct PP_Var relative_string,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_resolve_relative_to_url(base_url, relative_string, components);
}

static
struct PP_Var
trace_ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance,
                                                    struct PP_Var relative_string,
                                                    struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_resolve_relative_to_document(instance, relative_string, components);
}

static
PP_Bool
trace_ppb_url_util_dev_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_is_same_security_origin(url_a, url_b);
}

static
PP_Bool
trace_ppb_url_util_dev_document_can_request(PP_Instance instance, struct PP_Var url)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_document_can_request(instance, url);
}

static
PP_Bool
trace_ppb_url_util_dev_document_can_access_document(PP_Instance active, PP_Instance target)
{
    trace_info("[PPB] {zilch} %s active=%d, target=%d\n", __func__+6, active, target);
    return ppb_url_util_dev_document_can_access_document(active, target);
}

static
struct PP_Var
trace_ppb_url_util_dev_get_document_url(PP_Instance instance,
                                        struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {fake} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_document_url(instance, components);
}

static
struct PP_Var
trace_ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_plugin_instance_url(instance, components);
}

static
struct PP_Var
trace_ppb_url_util_dev_get_plugin_referrer_url(PP_Instance instance,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_plugin_referrer_url(instance, components);
}


const struct PPB_URLUtil_Dev_0_6 ppb_url_util_dev_interface_0_6 = {
    .Canonicalize =             trace_ppb_url_util_dev_canonicalize,
    .ResolveRelativeToURL =     trace_ppb_url_util_dev_resolve_relative_to_url,
    .ResolveRelativeToDocument = trace_ppb_url_util_dev_resolve_relative_to_document,
    .IsSameSecurityOrigin =     trace_ppb_url_util_dev_is_same_security_origin,
    .DocumentCanRequest =       trace_ppb_url_util_dev_document_can_request,
    .DocumentCanAccessDocument = trace_ppb_url_util_dev_document_can_access_document,
    .GetDocumentURL =           trace_ppb_url_util_dev_get_document_url,
    .GetPluginInstanceURL =     trace_ppb_url_util_dev_get_plugin_instance_url
};

const struct PPB_URLUtil_Dev_0_7 ppb_url_util_dev_interface_0_7 = {
    .Canonicalize =             trace_ppb_url_util_dev_canonicalize,
    .ResolveRelativeToURL =     trace_ppb_url_util_dev_resolve_relative_to_url,
    .ResolveRelativeToDocument = trace_ppb_url_util_dev_resolve_relative_to_document,
    .IsSameSecurityOrigin =     trace_ppb_url_util_dev_is_same_security_origin,
    .DocumentCanRequest =       trace_ppb_url_util_dev_document_can_request,
    .DocumentCanAccessDocument = trace_ppb_url_util_dev_document_can_access_document,
    .GetDocumentURL =           trace_ppb_url_util_dev_get_document_url,
    .GetPluginInstanceURL =     trace_ppb_url_util_dev_get_plugin_instance_url,
    .GetPluginReferrerURL =     trace_ppb_url_util_dev_get_plugin_referrer_url,
};
