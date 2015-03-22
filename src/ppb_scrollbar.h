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

#ifndef FPP_PPB_SCROLLBAR_H
#define FPP_PPB_SCROLLBAR_H

#include <ppapi/c/dev/ppb_scrollbar_dev.h>


PP_Resource
ppb_scrollbar_create(PP_Instance instance, PP_Bool vertical);

PP_Bool
ppb_scrollbar_is_scrollbar(PP_Resource resource);

uint32_t
ppb_scrollbar_get_thickness(PP_Resource resource);

PP_Bool
ppb_scrollbar_is_overlay(PP_Resource scrollbar);

uint32_t
ppb_scrollbar_get_value(PP_Resource scrollbar);

void
ppb_scrollbar_set_value(PP_Resource scrollbar, uint32_t value);

void
ppb_scrollbar_set_document_size(PP_Resource scrollbar, uint32_t size);

void
ppb_scrollbar_set_tick_marks(PP_Resource scrollbar, const struct PP_Rect tick_marks[],
                             uint32_t count);

void
ppb_scrollbar_scroll_by(PP_Resource scrollbar, PP_ScrollBy_Dev unit, int32_t multiplier);


#endif // FPP_PPB_SCROLLBAR_H
