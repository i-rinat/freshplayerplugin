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

#ifndef FPP_PPB_GRAPHICS2D_H
#define FPP_PPB_GRAPHICS2D_H

#include <ppapi/c/ppb_graphics_2d.h>


PP_Resource
ppb_graphics2d_create(PP_Instance instance, const struct PP_Size *size, PP_Bool is_always_opaque);

PP_Bool
ppb_graphics2d_is_graphics2d(PP_Resource resource);

PP_Bool
ppb_graphics2d_describe(PP_Resource graphics_2d, struct PP_Size *size, PP_Bool *is_always_opaque);

void
ppb_graphics2d_paint_image_data(PP_Resource graphics_2d, PP_Resource image_data,
                                const struct PP_Point *top_left, const struct PP_Rect *src_rect);

void
ppb_graphics2d_scroll(PP_Resource graphics_2d, const struct PP_Rect *clip_rect,
                      const struct PP_Point *amount);

void
ppb_graphics2d_replace_contents(PP_Resource graphics_2d, PP_Resource image_data);

int32_t
ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback);

PP_Bool
ppb_graphics2d_set_scale(PP_Resource resource, float scale);

float
ppb_graphics2d_get_scale(PP_Resource resource);

#endif // FPP_PPB_GRAPHICS2D_H
