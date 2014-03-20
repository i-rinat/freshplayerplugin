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

#include "ppb_input_event.h"
#include <stdlib.h>
#include "trace.h"


int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    return 0;
}

int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    return 0;
}

void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    return;
}

PP_Bool
ppb_input_event_is_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

PP_InputEvent_Type
ppb_input_event_get_type(PP_Resource event)
{
    PP_InputEvent_Type t = {0};
    return t;
}

PP_TimeTicks
ppb_input_event_get_time_stamp(PP_Resource event)
{
    return 0;
}

uint32_t
ppb_input_event_get_modifiers(PP_Resource event)
{
    return 0;
}

// trace wrappers
static
int32_t
trace_ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {zilch} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_input_events(instance, event_classes);
}

static
int32_t
trace_ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {zilch} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_filtering_input_events(instance, event_classes);
}

static
void
trace_ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {zilch} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    ppb_input_event_clear_input_event_request(instance, event_classes);
}

static
PP_Bool
trace_ppb_input_event_is_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_input_event_is_input_event(resource);
}

static
PP_InputEvent_Type
trace_ppb_input_event_get_type(PP_Resource event)
{
    trace_info("[PPB] {zilch} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_type(event);
}

static
PP_TimeTicks
trace_ppb_input_event_get_time_stamp(PP_Resource event)
{
    trace_info("[PPB] {zilch} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_time_stamp(event);
}

static
uint32_t
trace_ppb_input_event_get_modifiers(PP_Resource event)
{
    trace_info("[PPB] {zilch} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_modifiers(event);
}

const struct PPB_InputEvent_1_0 ppb_input_event_interface_1_0 = {
    .RequestInputEvents =           trace_ppb_input_event_request_input_events,
    .RequestFilteringInputEvents =  trace_ppb_input_event_request_filtering_input_events,
    .ClearInputEventRequest =       trace_ppb_input_event_clear_input_event_request,
    .IsInputEvent =                 trace_ppb_input_event_is_input_event,
    .GetType =                      trace_ppb_input_event_get_type,
    .GetTimeStamp =                 trace_ppb_input_event_get_time_stamp,
    .GetModifiers =                 trace_ppb_input_event_get_modifiers
};
