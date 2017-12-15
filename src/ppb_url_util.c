/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "pp_interface.h"
#include "ppb_instance.h"
#include "ppb_url_util.h"
#include "ppb_var.h"
#include "tables.h"
#include "trace_core.h"
#include "uri_parser/uri_parser.h"
#include <glib.h>
#include <stdlib.h>

struct PP_Var
ppb_url_util_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    if (components)
        uri_parser_parse_uri(ppb_var_var_to_utf8(url, NULL), components);
    return url;
}

struct PP_Var
ppb_url_util_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                     struct PP_URLComponents_Dev *components)
{
    const char *s_base_url = ppb_var_var_to_utf8(base_url, NULL);
    const char *s_relative_string = ppb_var_var_to_utf8(relative_string, NULL);

    gchar *res = uri_parser_merge_uris(s_base_url, s_relative_string);

    if (components)
        uri_parser_parse_uri(res, components);

    struct PP_Var var = ppb_var_var_from_utf8_z(res);
    g_free(res);

    return var;
}

struct PP_Var
ppb_url_util_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                          struct PP_URLComponents_Dev *components)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_MakeUndefined();
    }

    return ppb_url_util_resolve_relative_to_url(pp_i->document_base_url, relative_string,
                                                components);
}

PP_Bool
ppb_url_util_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_util_document_can_request(PP_Instance instance, struct PP_Var url)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_util_document_can_access_document(PP_Instance active, PP_Instance target)
{
    return PP_TRUE;
}

struct PP_Var
ppb_url_util_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_MakeUndefined();
    }

    if (components)
        uri_parser_parse_uri(ppb_var_var_to_utf8(pp_i->document_url, NULL), components);

    return pp_i->document_url;
}

struct PP_Var
ppb_url_util_get_plugin_instance_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var var = pp_i->instance_url;
    ppb_var_add_ref(var);

    if (components)
        uri_parser_parse_uri(ppb_var_var_to_utf8(var, NULL), components);

    return var;
}

struct PP_Var
ppb_url_util_get_plugin_referrer_url(PP_Instance instance, struct PP_URLComponents_Dev *components)
{
    const char *url = "";
    if (components)
        uri_parser_parse_uri(url, components);
    return ppb_var_var_from_utf8_z(url);
}


// trace wrappers
TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_canonicalize(url, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                           struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_url_util_resolve_relative_to_url(base_url, relative_string, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                                struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_url_util_resolve_relative_to_document(instance, relative_string, components);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_is_same_security_origin(url_a, url_b);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_document_can_request(PP_Instance instance, struct PP_Var url)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_url_util_document_can_request(instance, url);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_url_util_document_can_access_document(PP_Instance active, PP_Instance target)
{
    trace_info("[PPB] {zilch} %s active=%d, target=%d\n", __func__+6, active, target);
    return ppb_url_util_document_can_access_document(active, target);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_get_document_url(PP_Instance instance,
                                    struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_get_document_url(instance, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_get_plugin_instance_url(PP_Instance instance,
                                           struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_get_plugin_instance_url(instance, components);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_util_get_plugin_referrer_url(PP_Instance instance,
                                           struct PP_URLComponents_Dev *components)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_url_util_get_plugin_referrer_url(instance, components);
}


const struct PPB_URLUtil_Dev_0_6 ppb_url_util_dev_interface_0_6 = {
    .Canonicalize =                 TWRAPZ(ppb_url_util_canonicalize),
    .ResolveRelativeToURL =         TWRAPF(ppb_url_util_resolve_relative_to_url),
    .ResolveRelativeToDocument =    TWRAPF(ppb_url_util_resolve_relative_to_document),
    .IsSameSecurityOrigin =         TWRAPZ(ppb_url_util_is_same_security_origin),
    .DocumentCanRequest =           TWRAPZ(ppb_url_util_document_can_request),
    .DocumentCanAccessDocument =    TWRAPZ(ppb_url_util_document_can_access_document),
    .GetDocumentURL =               TWRAPF(ppb_url_util_get_document_url),
    .GetPluginInstanceURL =         TWRAPF(ppb_url_util_get_plugin_instance_url),
};

const struct PPB_URLUtil_Dev_0_7 ppb_url_util_dev_interface_0_7 = {
    .Canonicalize =                 TWRAPZ(ppb_url_util_canonicalize),
    .ResolveRelativeToURL =         TWRAPF(ppb_url_util_resolve_relative_to_url),
    .ResolveRelativeToDocument =    TWRAPF(ppb_url_util_resolve_relative_to_document),
    .IsSameSecurityOrigin =         TWRAPZ(ppb_url_util_is_same_security_origin),
    .DocumentCanRequest =           TWRAPZ(ppb_url_util_document_can_request),
    .DocumentCanAccessDocument =    TWRAPZ(ppb_url_util_document_can_access_document),
    .GetDocumentURL =               TWRAPF(ppb_url_util_get_document_url),
    .GetPluginInstanceURL =         TWRAPF(ppb_url_util_get_plugin_instance_url),
    .GetPluginReferrerURL =         TWRAPZ(ppb_url_util_get_plugin_referrer_url),
};

static
void
__attribute__((constructor))
constructor_ppb_url_util(void)
{
    register_interface(PPB_URLUTIL_DEV_INTERFACE_0_6, &ppb_url_util_dev_interface_0_6);
    register_interface(PPB_URLUTIL_DEV_INTERFACE_0_7, &ppb_url_util_dev_interface_0_7);
}
