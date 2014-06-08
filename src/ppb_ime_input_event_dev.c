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

#include "ppb_ime_input_event_dev.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_ime_input_event_dev_create(PP_Instance instance, PP_InputEvent_Type type,
                               PP_TimeTicks time_stamp, struct PP_Var text, uint32_t segment_number,
                               const uint32_t segment_offsets[], int32_t target_segment,
                               uint32_t selection_start, uint32_t selection_end)
{
    return 0;
}

PP_Bool
ppb_ime_input_event_dev_is_ime_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

struct PP_Var
ppb_ime_input_event_dev_get_text(PP_Resource ime_event)
{
    return PP_MakeUndefined();
}

uint32_t
ppb_ime_input_event_dev_get_segment_number(PP_Resource ime_event)
{
    return 0;
}

uint32_t
ppb_ime_input_event_dev_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    return 0;
}

int32_t
ppb_ime_input_event_dev_get_target_segment(PP_Resource ime_event)
{
    return -1;
}

void
ppb_ime_input_event_dev_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
}


#ifndef NDEBUG
// trace wrappers
static
PP_Resource
trace_ppb_ime_input_event_dev_create(PP_Instance instance, PP_InputEvent_Type type,
                                     PP_TimeTicks time_stamp, struct PP_Var text,
                                     uint32_t segment_number, const uint32_t segment_offsets[],
                                     int32_t target_segment, uint32_t selection_start,
                                     uint32_t selection_end)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_create(instance, type, time_stamp, text, segment_number,
                                          segment_offsets, target_segment, selection_start,
                                          selection_end);
}

static
PP_Bool
trace_ppb_ime_input_event_dev_is_ime_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_is_ime_input_event(resource);
}

static
struct PP_Var
trace_ppb_ime_input_event_dev_get_text(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_get_text(ime_event);
}

static
uint32_t
trace_ppb_ime_input_event_dev_get_segment_number(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_get_segment_number(ime_event);
}

static
uint32_t
trace_ppb_ime_input_event_dev_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_get_segment_offset(ime_event, index);
}

static
int32_t
trace_ppb_ime_input_event_dev_get_target_segment(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_dev_get_target_segment(ime_event);
}

static
void
trace_ppb_ime_input_event_dev_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_ime_input_event_dev_get_selection(ime_event, start, end);
}
#endif // NDEBUG


const struct PPB_IMEInputEvent_Dev_0_2 ppb_ime_input_event_dev_interface_0_2 = {
    .Create =           TWRAP(ppb_ime_input_event_dev_create),
    .IsIMEInputEvent =  TWRAP(ppb_ime_input_event_dev_is_ime_input_event),
    .GetText =          TWRAP(ppb_ime_input_event_dev_get_text),
    .GetSegmentNumber = TWRAP(ppb_ime_input_event_dev_get_segment_number),
    .GetSegmentOffset = TWRAP(ppb_ime_input_event_dev_get_segment_offset),
    .GetTargetSegment = TWRAP(ppb_ime_input_event_dev_get_target_segment),
    .GetSelection =     TWRAP(ppb_ime_input_event_dev_get_selection),
};
