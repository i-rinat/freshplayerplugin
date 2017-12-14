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

#pragma once

#include "glx.h"
#include "pp_resource.h"
#include <X11/Xlib.h>
#include <X11/extensions/Xrender.h>
#include <glib.h>
#include <ppapi/c/ppb_graphics_3d.h>

struct pp_graphics3d_s {
    COMMON_STRUCTURE_FIELDS
    GLXContext          glc;
    GLXFBConfig         fb_config;
    int32_t             depth;          ///< depth of the pixmap, 32 for transparent, 24 otherwise
    GLXPixmap           glx_pixmap;
    Pixmap              pixmap[2];      ///< first for immediate drawing, second - to store copy
    Picture             xr_pict[2];     ///< XRender pictures for X pixmaps
    XRenderPictFormat  *xr_pictfmt;
    int32_t             width;
    int32_t             height;
    GHashTable         *sub_maps;
};

int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value);

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[]);

PP_Bool
ppb_graphics3d_is_graphics3d(PP_Resource resource);

int32_t
ppb_graphics3d_get_attribs(PP_Resource context, int32_t attrib_list[]);

int32_t
ppb_graphics3d_set_attribs(PP_Resource context, const int32_t attrib_list[]);

int32_t
ppb_graphics3d_get_error(PP_Resource context);

int32_t
ppb_graphics3d_resize_buffers(PP_Resource context, int32_t width, int32_t height);

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback);
