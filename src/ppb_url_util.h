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

#ifndef FPP_PPB_URL_UTIL_H
#define FPP_PPB_URL_UTIL_H

#include <ppapi/c/dev/ppb_url_util_dev.h>


struct PP_Var
ppb_url_util_canonicalize(struct PP_Var url, struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_resolve_relative_to_url(struct PP_Var base_url, struct PP_Var relative_string,
                                     struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_resolve_relative_to_document(PP_Instance instance, struct PP_Var relative_string,
                                          struct PP_URLComponents_Dev *components);

PP_Bool
ppb_url_util_is_same_security_origin(struct PP_Var url_a, struct PP_Var url_b);

PP_Bool
ppb_url_util_document_can_request(PP_Instance instance, struct PP_Var url);

PP_Bool
ppb_url_util_document_can_access_document(PP_Instance active, PP_Instance target);

struct PP_Var
ppb_url_util_get_document_url(PP_Instance instance, struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_get_plugin_instance_url(PP_Instance instance, struct PP_URLComponents_Dev *components);

struct PP_Var
ppb_url_util_get_plugin_referrer_url(PP_Instance instance, struct PP_URLComponents_Dev *components);

#endif // FPP_PPB_URL_UTIL_H
