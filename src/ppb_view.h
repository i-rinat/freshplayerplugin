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

#include "pp_resource.h"
#include <ppapi/c/dev/ppb_view_dev.h>
#include <ppapi/c/ppb_view.h>

struct pp_view_s {
    COMMON_STRUCTURE_FIELDS
    struct PP_Rect      rect;
};

PP_Bool
ppb_view_is_view(PP_Resource resource);

PP_Bool
ppb_view_get_rect(PP_Resource resource, struct PP_Rect *rect);

PP_Bool
ppb_view_is_fullscreen(PP_Resource resource);

PP_Bool
ppb_view_is_visible(PP_Resource resource);

PP_Bool
ppb_view_is_page_visible(PP_Resource resource);

PP_Bool
ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect *clip);

float
ppb_view_get_device_scale(PP_Resource resource);

float
ppb_view_get_css_scale(PP_Resource resource);

PP_Bool
ppb_view_get_scroll_offset(PP_Resource resource, struct PP_Point *offset);
