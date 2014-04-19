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

#include <assert.h>
#include "ppb_url_response_info.h"
#include <stddef.h>
#include <string.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"
#include "pp_resource.h"


PP_Bool
ppb_url_response_info_is_url_response_info(PP_Resource resource)
{
    return PP_TRUE;
}

struct PP_Var
ppb_url_response_info_get_property(PP_Resource response, PP_URLResponseProperty property)
{
    struct PP_Var var = PP_MakeUndefined();
    struct pp_url_response_info_s *ri = pp_resource_acquire(response,PP_RESOURCE_URL_RESPONSE_INFO);
    struct pp_url_loader_s *ul = pp_resource_acquire(ri->url_loader, PP_RESOURCE_URL_LOADER);

    switch (property) {
    case PP_URLRESPONSEPROPERTY_URL:
        var = PP_MakeString(ul->url);
        break;
    case PP_URLRESPONSEPROPERTY_REDIRECTURL:
        var = PP_MakeString(ul->redirect_url);
        break;
    case PP_URLRESPONSEPROPERTY_REDIRECTMETHOD:
        // redirection is believed to be always GET
        // TODO: check whenever it may be HEAD
        var = PP_MakeString("GET");
        break;
    case PP_URLRESPONSEPROPERTY_STATUSCODE:
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = ul->http_code;
        break;
    case PP_URLRESPONSEPROPERTY_STATUSLINE:
        var = PP_MakeString(ul->status_line);
        break;
    case PP_URLRESPONSEPROPERTY_HEADERS:
        var = PP_MakeString(ul->headers);
        break;
    }

    pp_resource_release(ri->url_loader);
    pp_resource_release(response);
    return var;
}

PP_Resource
ppb_url_response_info_get_body_as_file_ref(PP_Resource response)
{
    return 123003;
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
    trace_info("[PPB] {part} %s response=%d, property=%s\n", __func__+6, response,
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
