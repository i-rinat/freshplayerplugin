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
#include <ppapi/c/pp_errors.h>
#include "trace.h"
#include "tables.h"


int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return PP_ERROR_BADARGUMENT;

    pp_i->event_mask |= event_classes;
    return PP_OK;
}

int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return PP_ERROR_BADARGUMENT;

    pp_i->filtered_event_mask |= event_classes;
    return PP_OK;
}

void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i)
        return;

    pp_i->event_mask &= ~event_classes;
    pp_i->filtered_event_mask &= ~event_classes;
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

PP_Resource
ppb_mouse_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                             uint32_t modifiers, PP_InputEvent_MouseButton mouse_button,
                             const struct PP_Point *mouse_position, int32_t click_count,
                             const struct PP_Point *mouse_movement)
{
    return 0;
}

PP_Bool
ppb_mouse_input_event_is_mouse_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

PP_InputEvent_MouseButton
ppb_mouse_input_event_get_button(PP_Resource mouse_event)
{
    return 0;
}

struct PP_Point
ppb_mouse_input_event_get_position(PP_Resource mouse_event)
{
    return PP_MakePoint(0, 0);
}

int32_t
ppb_mouse_input_event_get_click_count(PP_Resource mouse_event)
{
    return 0;
}

struct PP_Point
ppb_mouse_input_event_get_movement(PP_Resource mouse_event)
{
    return PP_MakePoint(0, 0);
}

PP_Resource
ppb_wheel_input_event_create(PP_Instance instance, PP_TimeTicks time_stamp, uint32_t modifiers,
                             const struct PP_FloatPoint *wheel_delta,
                             const struct PP_FloatPoint *wheel_ticks, PP_Bool scroll_by_page)
{
    return 0;
}

PP_Bool
ppb_wheel_input_event_is_wheel_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

struct PP_FloatPoint
ppb_wheel_input_event_get_delta(PP_Resource wheel_event)
{
    return PP_MakeFloatPoint(0, 0);
}

struct PP_FloatPoint
ppb_wheel_input_event_get_ticks(PP_Resource wheel_event)
{
    return PP_MakeFloatPoint(0, 0);
}

PP_Bool
ppb_wheel_input_event_get_scroll_by_page(PP_Resource wheel_event)
{
    return PP_FALSE;
}

PP_Resource
ppb_keyboard_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                PP_TimeTicks time_stamp, uint32_t modifiers, uint32_t key_code,
                                struct PP_Var character_text)
{
    return 0;
}

PP_Bool
ppb_keyboard_input_event_is_keyboard_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

uint32_t
ppb_keyboard_input_event_get_key_code(PP_Resource key_event)
{
    return 0;
}

struct PP_Var
ppb_keyboard_input_event_get_character_text(PP_Resource character_event)
{
    return PP_MakeUndefined();
}

PP_Resource
ppb_touch_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                             uint32_t modifiers)
{
    return 0;
}

void
ppb_touch_input_event_add_touch_point(PP_Resource touch_event, PP_TouchListType list,
                                      const struct PP_TouchPoint *point)
{
}

PP_Bool
ppb_touch_input_event_is_touch_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

uint32_t
ppb_touch_input_event_get_touch_count(PP_Resource resource, PP_TouchListType list)
{
    return 0;
}

struct PP_TouchPoint
ppb_touch_input_event_get_touch_by_index(PP_Resource resource, PP_TouchListType list,
                                         uint32_t index)
{
    return PP_MakeTouchPoint();
}

struct PP_TouchPoint
ppb_touch_input_event_get_touch_by_id(PP_Resource resource, PP_TouchListType list,
                                      uint32_t touch_id)
{
    return PP_MakeTouchPoint();
}

PP_Resource
ppb_ime_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                           struct PP_Var text, uint32_t segment_number,
                           const uint32_t segment_offsets[], int32_t target_segment,
                           uint32_t selection_start, uint32_t selection_end)
{
    return 0;
}

PP_Bool
ppb_ime_input_event_is_ime_input_event(PP_Resource resource)
{
    return PP_TRUE;
}

struct PP_Var
ppb_ime_input_event_get_text(PP_Resource ime_event)
{
    return PP_MakeUndefined();
}

uint32_t
ppb_ime_input_event_get_segment_number(PP_Resource ime_event)
{
    return 0;
}

uint32_t
ppb_ime_input_event_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    return 0;
}

int32_t
ppb_ime_input_event_get_target_segment(PP_Resource ime_event)
{
    return 0;
}

void
ppb_ime_input_event_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
}


// trace wrappers
static
int32_t
trace_ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_input_events(instance, event_classes);
}

static
int32_t
trace_ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_filtering_input_events(instance, event_classes);
}

static
void
trace_ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
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

static
PP_Resource
trace_ppb_mouse_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                   PP_TimeTicks time_stamp, uint32_t modifiers,
                                   PP_InputEvent_MouseButton mouse_button,
                                   const struct PP_Point *mouse_position, int32_t click_count,
                                   const struct PP_Point *mouse_movement)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_create(instance, type, time_stamp, modifiers, mouse_button,
                                        mouse_position, click_count, mouse_movement);
}

static
PP_Bool
trace_ppb_mouse_input_event_is_mouse_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_is_mouse_input_event(resource);
}

static
PP_InputEvent_MouseButton
trace_ppb_mouse_input_event_get_button(PP_Resource mouse_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_get_button(mouse_event);
}

static
struct PP_Point
trace_ppb_mouse_input_event_get_position(PP_Resource mouse_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_get_position(mouse_event);
}

static
int32_t
trace_ppb_mouse_input_event_get_click_count(PP_Resource mouse_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_get_click_count(mouse_event);
}

static
struct PP_Point
trace_ppb_mouse_input_event_get_movement(PP_Resource mouse_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_mouse_input_event_get_movement(mouse_event);
}

static
PP_Resource
trace_ppb_wheel_input_event_create(PP_Instance instance, PP_TimeTicks time_stamp,
                                   uint32_t modifiers, const struct PP_FloatPoint *wheel_delta,
                                   const struct PP_FloatPoint *wheel_ticks, PP_Bool scroll_by_page)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_wheel_input_event_create(instance, time_stamp, modifiers, wheel_delta, wheel_ticks,
                                        scroll_by_page);
}

static
PP_Bool
trace_ppb_wheel_input_event_is_wheel_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_wheel_input_event_is_wheel_input_event(resource);
}

static
struct PP_FloatPoint
trace_ppb_wheel_input_event_get_delta(PP_Resource wheel_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_wheel_input_event_get_delta(wheel_event);
}

static
struct PP_FloatPoint
trace_ppb_wheel_input_event_get_ticks(PP_Resource wheel_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_wheel_input_event_get_ticks(wheel_event);
}

static
PP_Bool
trace_ppb_wheel_input_event_get_scroll_by_page(PP_Resource wheel_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_wheel_input_event_get_scroll_by_page(wheel_event);
}

static
PP_Resource
trace_ppb_keyboard_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                      PP_TimeTicks time_stamp, uint32_t modifiers,
                                      uint32_t key_code, struct PP_Var character_text)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_keyboard_input_event_create(instance, type, time_stamp, modifiers, key_code,
                                           character_text);
}

static
PP_Bool
trace_ppb_keyboard_input_event_is_keyboard_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_keyboard_input_event_is_keyboard_input_event(resource);
}

static
uint32_t
trace_ppb_keyboard_input_event_get_key_code(PP_Resource key_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_keyboard_input_event_get_key_code(key_event);
}

static
struct PP_Var
trace_ppb_keyboard_input_event_get_character_text(PP_Resource character_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_keyboard_input_event_get_character_text(character_event);
}

static
PP_Resource
trace_ppb_touch_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                   PP_TimeTicks time_stamp, uint32_t modifiers)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_create(instance, type, time_stamp, modifiers);
}

static
void
trace_ppb_touch_input_event_add_touch_point(PP_Resource touch_event, PP_TouchListType list,
                                            const struct PP_TouchPoint *point)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_add_touch_point(touch_event, list, point);
}

static
PP_Bool
trace_ppb_touch_input_event_is_touch_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_is_touch_input_event(resource);
}

static
uint32_t
trace_ppb_touch_input_event_get_touch_count(PP_Resource resource, PP_TouchListType list)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_get_touch_count(resource, list);
}

static
struct PP_TouchPoint
trace_ppb_touch_input_event_get_touch_by_index(PP_Resource resource, PP_TouchListType list,
                                               uint32_t index)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_get_touch_by_index(resource, list, index);
}

static
struct PP_TouchPoint
trace_ppb_touch_input_event_get_touch_by_id(PP_Resource resource, PP_TouchListType list,
                                            uint32_t touch_id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_touch_input_event_get_touch_by_id(resource, list, touch_id);
}

static
PP_Resource
trace_ppb_ime_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                 PP_TimeTicks time_stamp, struct PP_Var text,
                                 uint32_t segment_number, const uint32_t segment_offsets[],
                                 int32_t target_segment, uint32_t selection_start,
                                 uint32_t selection_end)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_create(instance, type, time_stamp, text, segment_number,
                                      segment_offsets, target_segment, selection_start,
                                      selection_end);
}

static
PP_Bool
trace_ppb_ime_input_event_is_ime_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_is_ime_input_event(resource);
}

static
struct PP_Var
trace_ppb_ime_input_event_get_text(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_get_text(ime_event);
}

static
uint32_t
trace_ppb_ime_input_event_get_segment_number(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_get_segment_number(ime_event);
}

static
uint32_t
trace_ppb_ime_input_event_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_get_segment_offset(ime_event, index);
}

static
int32_t
trace_ppb_ime_input_event_get_target_segment(PP_Resource ime_event)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_get_target_segment(ime_event);
}

static
void
trace_ppb_ime_input_event_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_ime_input_event_get_selection(ime_event, start, end);
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

const struct PPB_MouseInputEvent_1_1 ppb_mouse_input_event_interface_1_1 = {
    .Create =               trace_ppb_mouse_input_event_create,
    .IsMouseInputEvent =    trace_ppb_mouse_input_event_is_mouse_input_event,
    .GetButton =            trace_ppb_mouse_input_event_get_button,
    .GetPosition =          trace_ppb_mouse_input_event_get_position,
    .GetClickCount =        trace_ppb_mouse_input_event_get_click_count,
    .GetMovement =          trace_ppb_mouse_input_event_get_movement,
};

const struct PPB_WheelInputEvent_1_0 ppb_wheel_input_event_interface_1_0 = {
    .Create =               trace_ppb_wheel_input_event_create,
    .IsWheelInputEvent =    trace_ppb_wheel_input_event_is_wheel_input_event,
    .GetDelta =             trace_ppb_wheel_input_event_get_delta,
    .GetTicks =             trace_ppb_wheel_input_event_get_ticks,
    .GetScrollByPage =      trace_ppb_wheel_input_event_get_scroll_by_page,
};

const struct PPB_KeyboardInputEvent_1_0 ppb_keyboard_input_event_interface_1_0 = {
    .Create =               trace_ppb_keyboard_input_event_create,
    .IsKeyboardInputEvent = trace_ppb_keyboard_input_event_is_keyboard_input_event,
    .GetKeyCode =           trace_ppb_keyboard_input_event_get_key_code,
    .GetCharacterText =     trace_ppb_keyboard_input_event_get_character_text,
};

const struct PPB_TouchInputEvent_1_0 ppb_touch_input_event_interface_1_0 = {
    .Create =           trace_ppb_touch_input_event_create,
    .AddTouchPoint =    trace_ppb_touch_input_event_add_touch_point,
    .IsTouchInputEvent = trace_ppb_touch_input_event_is_touch_input_event,
    .GetTouchCount =    trace_ppb_touch_input_event_get_touch_count,
    .GetTouchByIndex =  trace_ppb_touch_input_event_get_touch_by_index,
    .GetTouchById =     trace_ppb_touch_input_event_get_touch_by_id,
};

const struct PPB_IMEInputEvent_1_0 ppb_ime_input_event_interface_1_0 = {
    .Create =           trace_ppb_ime_input_event_create,
    .IsIMEInputEvent =  trace_ppb_ime_input_event_is_ime_input_event,
    .GetText =          trace_ppb_ime_input_event_get_text,
    .GetSegmentNumber = trace_ppb_ime_input_event_get_segment_number,
    .GetSegmentOffset = trace_ppb_ime_input_event_get_segment_offset,
    .GetTargetSegment = trace_ppb_ime_input_event_get_target_segment,
    .GetSelection =     trace_ppb_ime_input_event_get_selection,
};
