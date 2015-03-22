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

#ifndef FPP_PPB_WIDGET_H
#define FPP_PPB_WIDGET_H

#include <ppapi/c/dev/ppb_widget_dev.h>


PP_Bool
ppb_widget_is_widget(PP_Resource resource);

PP_Bool
ppb_widget_paint(PP_Resource widget, const struct PP_Rect *rect, PP_Resource image);

PP_Bool
ppb_widget_handle_event(PP_Resource widget, PP_Resource input_event);

PP_Bool
ppb_widget_get_location(PP_Resource widget, struct PP_Rect *location);

void
ppb_widget_set_location(PP_Resource widget, const struct PP_Rect *location);

void
ppb_widget_set_scale(PP_Resource widget, float scale);


#endif // FPP_PPB_WIDGET_H
