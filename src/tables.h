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

#ifndef FPP_TABLES_H
#define FPP_TABLES_H

#include <pthread.h>
#include <ppapi/c/pp_var.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include "pp_resource.h"
#include <npapi/npruntime.h>
#include <npapi/npfunctions.h>
#include <va/va.h>
#include <va/va_x11.h>


#define NPString_literal(str) { .UTF8Characters = str, .UTF8Length = strlen(str) }


typedef GLXContext
(*glx_create_context_attribs_arb_f)(Display *dpy, GLXFBConfig config,
                                    GLXContext share_context, Bool direct,
                                    const int *attrib_list);

typedef void
(*glx_bind_tex_image_ext_f)(Display *dpy, GLXDrawable drawable, int buffer, const int *attrib_list);

typedef void
(*glx_release_tex_image_ext_f)(Display *dpy, GLXDrawable drawable, int buffer);


struct display_s {
    Display                            *x;
    VADisplay                           va;
    int                                 va_available;
    Cursor                              transparent_cursor;
    pthread_mutex_t                     lock;
    XRenderPictFormat                  *pictfmt_rgb24;
    XRenderPictFormat                  *pictfmt_argb32;
    uint32_t                            min_width;  ///< smallest screen width
    uint32_t                            min_height; ///< smallest screen height
    uint32_t                            screensaver_types;
    glx_create_context_attribs_arb_f    glXCreateContextAttribsARB;
    glx_bind_tex_image_ext_f            glXBindTexImageEXT;
    glx_release_tex_image_ext_f         glXReleaseTexImageEXT;
    uint32_t                            glx_arb_create_context;
    uint32_t                            glx_arb_create_context_profile;
    uint32_t                            glx_ext_create_context_es2_profile;
};

extern NPNetscapeFuncs  npn;
extern struct display_s display;

int         tables_get_urandom_fd(void);

struct pp_instance_s   *tables_get_pp_instance(PP_Instance instance);
void                    tables_add_pp_instance(PP_Instance instance, struct pp_instance_s *pp_i);
void                    tables_remove_pp_instance(PP_Instance instance);
struct pp_instance_s   *tables_get_some_pp_instance(void);
PP_Instance             tables_generate_new_pp_instance_id(void);

PangoContext   *tables_get_pango_ctx(void);
PangoFontMap   *tables_get_pango_font_map(void);

void    tables_add_npobj_npp_mapping(NPObject *npobj, NPP npp);
NPP     tables_get_npobj_npp_mapping(NPObject *npobj);
void    tables_remove_npobj_npp_mapping(NPObject *npobj);

int     tables_open_display(void);
void    tables_close_display(void);

#endif // FPP_TABLES_H
