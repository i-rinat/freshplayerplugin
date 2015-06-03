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

#include "ppb_scrollbar.h"
#include <stdlib.h>
#include "trace.h"
#include "reverse_constant.h"
#include "pp_interface.h"


PP_Resource
ppb_scrollbar_create(PP_Instance instance, PP_Bool vertical)
{
    return 0;
}

PP_Bool
ppb_scrollbar_is_scrollbar(PP_Resource resource)
{
    return PP_TRUE;
}

uint32_t
ppb_scrollbar_get_thickness(PP_Resource resource)
{
    return 5;
}

PP_Bool
ppb_scrollbar_is_overlay(PP_Resource scrollbar)
{
    return PP_FALSE;
}

uint32_t
ppb_scrollbar_get_value(PP_Resource scrollbar)
{
    return 0;
}

void
ppb_scrollbar_set_value(PP_Resource scrollbar, uint32_t value)
{
}

void
ppb_scrollbar_set_document_size(PP_Resource scrollbar, uint32_t size)
{
}

void
ppb_scrollbar_set_tick_marks(PP_Resource scrollbar, const struct PP_Rect tick_marks[],
                             uint32_t count)
{
}

void
ppb_scrollbar_scroll_by(PP_Resource scrollbar, PP_ScrollBy_Dev unit, int32_t multiplier)
{
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_scrollbar_create(PP_Instance instance, PP_Bool vertical)
{
    trace_info("[PPB] {zilch} %s instance=%d, vertical=%u\n", __func__+6, instance, vertical);
    return ppb_scrollbar_create(instance, vertical);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_scrollbar_is_scrollbar(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_scrollbar_is_scrollbar(resource);
}

TRACE_WRAPPER
uint32_t
trace_ppb_scrollbar_get_thickness(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_scrollbar_get_thickness(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_scrollbar_is_overlay(PP_Resource scrollbar)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d\n", __func__+6, scrollbar);
    return ppb_scrollbar_is_overlay(scrollbar);
}

TRACE_WRAPPER
uint32_t
trace_ppb_scrollbar_get_value(PP_Resource scrollbar)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d\n", __func__+6, scrollbar);
    return ppb_scrollbar_get_value(scrollbar);
}

TRACE_WRAPPER
void
trace_ppb_scrollbar_set_value(PP_Resource scrollbar, uint32_t value)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d, value=%u\n", __func__+6, scrollbar, value);
    return ppb_scrollbar_set_value(scrollbar, value);
}

TRACE_WRAPPER
void
trace_ppb_scrollbar_set_document_size(PP_Resource scrollbar, uint32_t size)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d, size=%u\n", __func__+6, scrollbar, size);
    return ppb_scrollbar_set_document_size(scrollbar, size);
}

TRACE_WRAPPER
void
trace_ppb_scrollbar_set_tick_marks(PP_Resource scrollbar, const struct PP_Rect tick_marks[],
                                   uint32_t count)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d, tick_marks=%p, count=%u\n", __func__+6, scrollbar,
               tick_marks, count);
    return ppb_scrollbar_set_tick_marks(scrollbar, tick_marks, count);
}

TRACE_WRAPPER
void
trace_ppb_scrollbar_scroll_by(PP_Resource scrollbar, PP_ScrollBy_Dev unit, int32_t multiplier)
{
    trace_info("[PPB] {zilch} %s scrollbar=%d, unit=%s(%u), multiplier=%d\n", __func__+6,
               scrollbar, reverse_scrollby(unit), unit, multiplier);
    return ppb_scrollbar_scroll_by(scrollbar, unit, multiplier);
}

const struct PPB_Scrollbar_Dev_0_5 ppb_scrollbar_dev_interface_0_5 = {
    .Create =           TWRAPZ(ppb_scrollbar_create),
    .IsScrollbar =      TWRAPZ(ppb_scrollbar_is_scrollbar),
    .GetThickness =     TWRAPZ(ppb_scrollbar_get_thickness),
    .IsOverlay =        TWRAPZ(ppb_scrollbar_is_overlay),
    .GetValue =         TWRAPZ(ppb_scrollbar_get_value),
    .SetValue =         TWRAPZ(ppb_scrollbar_set_value),
    .SetDocumentSize =  TWRAPZ(ppb_scrollbar_set_document_size),
    .SetTickMarks =     TWRAPZ(ppb_scrollbar_set_tick_marks),
    .ScrollBy =         TWRAPZ(ppb_scrollbar_scroll_by),
};

static
void
__attribute__((constructor))
constructor_ppb_scrollbar(void)
{
    register_interface(PPB_SCROLLBAR_DEV_INTERFACE_0_5, &ppb_scrollbar_dev_interface_0_5);
}
