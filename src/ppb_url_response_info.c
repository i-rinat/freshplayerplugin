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
#include "ppb_url_response_info.h"
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"
#include "pp_resource.h"
#include "ppb_var.h"
#include "pp_interface.h"


PP_Bool
ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_URL_RESPONSE_INFO;
}

static
void
ppb_url_response_info_destroy(void *p)
{
    struct pp_url_response_info_s *ri = p;
    pp_resource_unref(ri->url_loader_id);
}

struct PP_Var
ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    struct pp_url_response_info_s *ri = pp_resource_acquire(response,PP_RESOURCE_URL_RESPONSE_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }
    struct pp_url_loader_s *ul = ri->url_loader;
    struct PP_Var var = PP_MakeUndefined();

    switch (property) {
    case PP_URLRESPONSEPROPERTY_URL:
        var = ppb_var_var_from_utf8_z(ul->url);
        break;
    case PP_URLRESPONSEPROPERTY_REDIRECTURL:
        var = ppb_var_var_from_utf8_z(ul->redirect_url);
        break;
    case PP_URLRESPONSEPROPERTY_REDIRECTMETHOD:
        // redirection is believed to be always GET
        // TODO: check whenever it may be HEAD
        var = ppb_var_var_from_utf8_z("GET");
        break;
    case PP_URLRESPONSEPROPERTY_STATUSCODE:
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = ul->http_code;
        break;
    case PP_URLRESPONSEPROPERTY_STATUSLINE:
        var = ppb_var_var_from_utf8_z(ul->status_line);
        break;
    case PP_URLRESPONSEPROPERTY_HEADERS:
        var = ppb_var_var_from_utf8_z(ul->headers);
        break;
    }

    pp_resource_release(response);
    return var;
}

PP_Resource
ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    struct pp_url_response_info_s *ri = pp_resource_acquire(response,PP_RESOURCE_URL_RESPONSE_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    struct pp_url_loader_s *ul = ri->url_loader;

    PP_Resource file_ref = pp_resource_allocate(PP_RESOURCE_FILE_REF, ri->instance);
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, resource allocation failure\n", __func__);
        pp_resource_release(response);
        return 0;
    }

    fr->fd = dup(ul->fd);
    fr->type = PP_FILE_REF_TYPE_FD;

    pp_resource_release(file_ref);
    pp_resource_release(response);

    return file_ref;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_response_info_is_url_response_info(resource);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    trace_info("[PPB] {full} %s response=%d, property=%s\n", __func__+6, response,
               reverse_pp_url_response_property(property));
    return ppb_url_response_info_get_property(response, property);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    trace_info("[PPB] {full} %s response=%d\n", __func__+6, response);
    return ppb_url_response_info_get_body_as_file_ref(response);
}


const struct PPB_URLResponseInfo_1_0 ppb_url_response_info_interface_1_0 = {
    .IsURLResponseInfo =    TWRAPF(ppb_url_response_info_is_url_response_info),
    .GetProperty =          TWRAPF(ppb_url_response_info_get_property),
    .GetBodyAsFileRef =     TWRAPF(ppb_url_response_info_get_body_as_file_ref),
};

static
void
__attribute__((constructor))
constructor_ppb_url_response_info(void)
{
    register_interface(PPB_URLRESPONSEINFO_INTERFACE_1_0, &ppb_url_response_info_interface_1_0);
    register_resource(PP_RESOURCE_URL_RESPONSE_INFO, ppb_url_response_info_destroy);
}
