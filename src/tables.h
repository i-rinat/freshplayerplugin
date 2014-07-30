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

#ifndef FPP__TABLES_H
#define FPP__TABLES_H

#include <ppapi/c/pp_var.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include "pp_resource.h"
#include <npapi/npruntime.h>
#include <npapi/npfunctions.h>


extern NPNetscapeFuncs  npn;

int         tables_get_urandom_fd(void);

struct pp_instance_s   *tables_get_pp_instance(PP_Instance instance);
void                    tables_add_pp_instance(PP_Instance instance, struct pp_instance_s *pp_i);
void                    tables_remove_pp_instance(PP_Instance instance);

struct PP_Var       np_variant_to_pp_var(NPVariant v);

PangoContext   *tables_get_pango_ctx(void);
PangoFontMap   *tables_get_pango_font_map(void);

PangoFontDescription *
pp_font_desc_to_pango_font_desc(const struct PP_BrowserFont_Trusted_Description *description);

void    tables_add_npobj_npp_mapping(NPObject *npobj, NPP npp);
NPP     tables_get_npobj_npp_mapping(NPObject *npobj);
void    tables_remove_npobj_npp_mapping(NPObject *npobj);

#endif // FPP__TABLES_H
