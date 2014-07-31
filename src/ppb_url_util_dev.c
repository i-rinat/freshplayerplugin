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

#include "ppb_url_util_dev.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include "ppb_message_loop.h"
#include <stdlib.h>
#include <string.h>
#include <uriparser/Uri.h>
#include "trace.h"
#include "tables.h"


static
void
reset_components(struct PP_URLComponents_Dev *components)
{
    if (!components)
        return;
    components->scheme.begin =   0;     components->scheme.len =   -1;
    components->username.begin = 0;     components->username.len = -1;
    components->password.begin = 0;     components->password.len = -1;
    components->host.begin =     0;     components->host.len =     -1;
    components->port.begin =     0;     components->port.len =     -1;
    components->path.begin =     0;     components->path.len =     -1;
    components->query.begin =    0;     components->query.len =    -1;
    components->ref.begin =      0;     components->ref.len =      -1;
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

    reset_components(components);

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
ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    return PP_MakeUndefined();
}

struct PP_Var
ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                         struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    struct PP_Var var = PP_MakeNull();

    if (base_url.type != PP_VARTYPE_STRING) {
        trace_warning("%s, base_url is not a string\n", __func__);
        return PP_MakeNull();
    }

    if (relative_string.type != PP_VARTYPE_STRING) {
        trace_warning("%s, relative_string is not a string\n", __func__);
        return PP_MakeNull();
    }

    const char *s_base_url = ppb_var_var_to_utf8(base_url, NULL);
    const char *s_relative_string = ppb_var_var_to_utf8(relative_string, NULL);

    UriParserStateA ups;
    UriUriA uri_base, uri_rel, uri_result;

    ups.uri = &uri_base;
    if (uriParseUriA(&ups, s_base_url) != URI_SUCCESS) {
        trace_warning("%s, can't parse s_base_url\n", __func__);
        goto err_1;
    }

    ups.uri = &uri_rel;
    if (uriParseUriA(&ups, s_relative_string) != URI_SUCCESS) {
        trace_warning("%s, can't parse s_relative_string\n", __func__);
        goto err_2;
    }

    if (uriAddBaseUriA(&uri_result, &uri_rel, &uri_base) != URI_SUCCESS) {
        trace_warning("%s, can't merge base and rel\n", __func__);
        goto err_3;
    }

    int len;
    uriToStringCharsRequiredA(&uri_result, &len);
    len++;
    char *str = malloc(len);
    uriToStringA(str, &uri_result, len, NULL);
    var = ppb_var_var_from_utf8_z(str);
    free(str);

    if (components)
        parse_url_string(str, components);

err_3:  uriFreeUriMembersA(&uri_result);
err_2:  uriFreeUriMembersA(&uri_rel);
err_1:  uriFreeUriMembersA(&uri_base);
    return var;
}

struct PP_Var
ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                              struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    struct PP_Var base = ppb_url_util_dev_get_document_url(instance, NULL);
    return ppb_url_util_dev_resolve_relative_to_url(base, relative_string, components);
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

struct get_document_url_param_s {
    NPP                     npp;
    NPObject               *np_window_obj;
    struct PP_Var           result;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
_get_document_url_ptac(void *user_data)
{
    struct get_document_url_param_s *p = user_data;

    p->result = ppb_var_var_from_utf8("", 0);
    NPIdentifier location_id = npn.getstringidentifier("location");
    NPIdentifier href_id = npn.getstringidentifier("href");
    NPObject *np_location_obj;
    NPVariant location_var, href_var;

    if (!npn.getproperty(p->npp, p->np_window_obj, location_id, &location_var))
        goto err_2;

    if (location_var.type != NPVariantType_Object)
        goto err_3;

    np_location_obj = location_var.value.objectValue;
    if (!npn.getproperty(p->npp, np_location_obj, href_id, &href_var))
        goto err_3;


    struct PP_Var var = np_variant_to_pp_var(href_var);
    if (var.type != PP_VARTYPE_STRING) {
        ppb_var_release(var);
        goto err_4;
    }

    ppb_var_release(p->result);
    p->result = var;


err_4:
    npn.releasevariantvalue(&href_var);
err_3:
    npn.releasevariantvalue(&location_var);
err_2:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
    return;
}

static
void
_get_document_url_comt(void *user_data, int32_t result)
{
    struct get_document_url_param_s *p = user_data;
    ppb_core_call_on_browser_thread(_get_document_url_ptac, p);
}

struct PP_Var
ppb_url_util_dev_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return PP_MakeUndefined();

    struct get_document_url_param_s p;
    p.npp = pp_i->npp;
    p.m_loop = ppb_message_loop_get_current();
    p.depth = ppb_message_loop_get_depth(p.m_loop) + 1;
    p.np_window_obj = pp_i->np_window_obj;

    ppb_message_loop_post_work(p.m_loop, PP_MakeCompletionCallback(_get_document_url_comt, &p), 0);
    ppb_message_loop_run_int(p.m_loop, 1);

    if (components)
        parse_url_string(ppb_var_var_to_utf8(p.result, NULL), components);

    return p.result;
}

struct PP_Var
ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return PP_MakeUndefined();

    struct PP_Var var = ppb_var_var_from_utf8_z(pp_i->instance_url);

    if (components)
        parse_url_string(ppb_var_var_to_utf8(var, NULL), components);

    return var;
}

struct PP_Var
ppb_url_util_dev_get_plugin_referrer_url(PP_Instance instance,
                                         struct PP_URLComponents_Dev *components)
{
    reset_components(components);
    return PP_MakeUndefined();
}


// trace wrappers
TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_canonicalize(url, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_resolve_relative_to_url(struct PP_Var base_url,
                                               struct PP_Var relative_string,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_url_util_dev_resolve_relative_to_url(base_url, relative_string, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_resolve_relative_to_document(PP_Instance instance,
                                                    struct PP_Var relative_string,
                                                    struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_url_util_dev_resolve_relative_to_document(instance, relative_string, components);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_dev_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_is_same_security_origin(url_a, url_b);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_dev_document_can_request(PP_Instance instance, struct PP_Var url)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_dev_document_can_request(instance, url);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_dev_document_can_access_document(PP_Instance active, PP_Instance target)
{
    trace_info("[PPB] {zilch} %s active=%d, target=%d\n", __func__+6, active, target);
    return ppb_url_util_dev_document_can_access_document(active, target);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_get_document_url(PP_Instance instance,
                                        struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_document_url(instance, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_get_plugin_instance_url(PP_Instance instance,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_plugin_instance_url(instance, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_dev_get_plugin_referrer_url(PP_Instance instance,
                                               struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_dev_get_plugin_referrer_url(instance, components);
}


const struct PPB_URLUtil_Dev_0_6 ppb_url_util_dev_interface_0_6 = {
    .Canonicalize =                 TWRAPZ(ppb_url_util_dev_canonicalize),
    .ResolveRelativeToURL =         TWRAPF(ppb_url_util_dev_resolve_relative_to_url),
    .ResolveRelativeToDocument =    TWRAPF(ppb_url_util_dev_resolve_relative_to_document),
    .IsSameSecurityOrigin =         TWRAPZ(ppb_url_util_dev_is_same_security_origin),
    .DocumentCanRequest =           TWRAPZ(ppb_url_util_dev_document_can_request),
    .DocumentCanAccessDocument =    TWRAPZ(ppb_url_util_dev_document_can_access_document),
    .GetDocumentURL =               TWRAPF(ppb_url_util_dev_get_document_url),
    .GetPluginInstanceURL =         TWRAPF(ppb_url_util_dev_get_plugin_instance_url),
};

const struct PPB_URLUtil_Dev_0_7 ppb_url_util_dev_interface_0_7 = {
    .Canonicalize =                 TWRAPZ(ppb_url_util_dev_canonicalize),
    .ResolveRelativeToURL =         TWRAPF(ppb_url_util_dev_resolve_relative_to_url),
    .ResolveRelativeToDocument =    TWRAPF(ppb_url_util_dev_resolve_relative_to_document),
    .IsSameSecurityOrigin =         TWRAPZ(ppb_url_util_dev_is_same_security_origin),
    .DocumentCanRequest =           TWRAPZ(ppb_url_util_dev_document_can_request),
    .DocumentCanAccessDocument =    TWRAPZ(ppb_url_util_dev_document_can_access_document),
    .GetDocumentURL =               TWRAPF(ppb_url_util_dev_get_document_url),
    .GetPluginInstanceURL =         TWRAPF(ppb_url_util_dev_get_plugin_instance_url),
    .GetPluginReferrerURL =         TWRAPZ(ppb_url_util_dev_get_plugin_referrer_url),
};
