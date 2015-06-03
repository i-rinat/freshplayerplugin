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

#include "ppb_widget.h"
#include <stdlib.h>
#include "trace.h"
#include <glib.h>
#include "pp_interface.h"


PP_Bool
ppb_widget_is_widget(PP_Resource resource)
{
    return PP_FALSE;
}

PP_Bool
ppb_widget_paint(PP_Resource widget, const struct PP_Rect *rect, PP_Resource image)
{
    return PP_FALSE;
}

PP_Bool
ppb_widget_handle_event(PP_Resource widget, PP_Resource input_event)
{
    return PP_FALSE;
}

PP_Bool
ppb_widget_get_location(PP_Resource widget, struct PP_Rect *location)
{
    return PP_FALSE;
}

void
ppb_widget_set_location(PP_Resource widget, const struct PP_Rect *location)
{
}

void
ppb_widget_set_scale(PP_Resource widget, float scale)
{
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_widget_is_widget(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_widget_is_widget(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_widget_paint(PP_Resource widget, const struct PP_Rect *rect, PP_Resource image)
{
    gchar *s_rect = trace_rect_as_string(rect);
    trace_info("[PPB] {zilch} %s widget=%d, rect=%s, image=%d\n", __func__+6, widget, s_rect,
               image);
    g_free(s_rect);
    return ppb_widget_paint(widget, rect, image);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_widget_handle_event(PP_Resource widget, PP_Resource input_event)
{
    trace_info("[PPB] {zilch} %s widget=%d, input_event=%d\n", __func__+6, widget, input_event);
    return ppb_widget_handle_event(widget, input_event);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_widget_get_location(PP_Resource widget, struct PP_Rect *location)
{
    trace_info("[PPB] {zilch} %s widget=%d\n", __func__+6, widget);
    return ppb_widget_get_location(widget, location);
}

TRACE_WRAPPER
void
trace_ppb_widget_set_location(PP_Resource widget, const struct PP_Rect *location)
{
    gchar *s_location = trace_rect_as_string(location);
    trace_info("[PPB] {zilch} %s widget=%d, location=%s\n", __func__+6, widget, s_location);
    g_free(s_location);
    return ppb_widget_set_location(widget, location);
}

TRACE_WRAPPER
void
trace_ppb_widget_set_scale(PP_Resource widget, float scale)
{
    trace_info("[PPB] {zilch} %s widget=%d, scale=%f\n", __func__+6, widget, scale);
    return ppb_widget_set_scale(widget, scale);
}


const struct PPB_Widget_Dev_0_3 ppb_widget_dev_interface_0_3 = {
    .IsWidget =     TWRAPZ(ppb_widget_is_widget),
    .Paint =        TWRAPZ(ppb_widget_paint),
    .HandleEvent =  TWRAPZ(ppb_widget_handle_event),
    .GetLocation =  TWRAPZ(ppb_widget_get_location),
    .SetLocation =  TWRAPZ(ppb_widget_set_location),
};

const struct PPB_Widget_Dev_0_4 ppb_widget_dev_interface_0_4 = {
    .IsWidget =     TWRAPZ(ppb_widget_is_widget),
    .Paint =        TWRAPZ(ppb_widget_paint),
    .HandleEvent =  TWRAPZ(ppb_widget_handle_event),
    .GetLocation =  TWRAPZ(ppb_widget_get_location),
    .SetLocation =  TWRAPZ(ppb_widget_set_location),
    .SetScale =     TWRAPZ(ppb_widget_set_scale),
};

static
void
__attribute__((constructor))
constructor_ppb_widget(void)
{
    register_interface(PPB_WIDGET_DEV_INTERFACE_0_3, &ppb_widget_dev_interface_0_3);
    register_interface(PPB_WIDGET_DEV_INTERFACE_0_4, &ppb_widget_dev_interface_0_4);
}
