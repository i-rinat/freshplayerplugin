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

#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/c/ppp_input_event.h>

int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes);

int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes);

void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes);

PP_Bool
ppb_input_event_is_input_event(PP_Resource resource);

PP_InputEvent_Type
ppb_input_event_get_type(PP_Resource event);

PP_TimeTicks
ppb_input_event_get_time_stamp(PP_Resource event);

uint32_t
ppb_input_event_get_modifiers(PP_Resource event);

PP_Resource
ppb_mouse_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                             PP_TimeTicks time_stamp, uint32_t modifiers,
                             PP_InputEvent_MouseButton mouse_button,
                             const struct PP_Point *mouse_position, int32_t click_count,
                             const struct PP_Point *mouse_movement);

PP_Bool
ppb_mouse_input_event_is_mouse_input_event(PP_Resource resource);

PP_InputEvent_MouseButton
ppb_mouse_input_event_get_button(PP_Resource mouse_event);

struct PP_Point
ppb_mouse_input_event_get_position(PP_Resource mouse_event);

int32_t
ppb_mouse_input_event_get_click_count(PP_Resource mouse_event);

struct PP_Point
ppb_mouse_input_event_get_movement(PP_Resource mouse_event);

PP_Resource
ppb_wheel_input_event_create(PP_Instance instance, PP_TimeTicks time_stamp, uint32_t modifiers,
                             const struct PP_FloatPoint *wheel_delta,
                             const struct PP_FloatPoint *wheel_ticks, PP_Bool scroll_by_page);

PP_Bool
ppb_wheel_input_event_is_wheel_input_event(PP_Resource resource);

struct PP_FloatPoint
ppb_wheel_input_event_get_delta(PP_Resource wheel_event);

struct PP_FloatPoint
ppb_wheel_input_event_get_ticks(PP_Resource wheel_event);

PP_Bool
ppb_wheel_input_event_get_scroll_by_page(PP_Resource wheel_event);

PP_Resource
ppb_keyboard_input_event_create_1_0(PP_Instance instance, PP_InputEvent_Type type,
                                    PP_TimeTicks time_stamp, uint32_t modifiers, uint32_t key_code,
                                    struct PP_Var character_text);

PP_Resource
ppb_keyboard_input_event_create_1_2(PP_Instance instance, PP_InputEvent_Type type,
                                    PP_TimeTicks time_stamp, uint32_t modifiers, uint32_t key_code,
                                    struct PP_Var character_text, struct PP_Var code);

PP_Bool
ppb_keyboard_input_event_is_keyboard_input_event(PP_Resource resource);

uint32_t
ppb_keyboard_input_event_get_key_code(PP_Resource key_event);

struct PP_Var
ppb_keyboard_input_event_get_character_text(PP_Resource character_event);

PP_Resource
ppb_touch_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                             uint32_t modifiers);

void
ppb_touch_input_event_add_touch_point(PP_Resource touch_event, PP_TouchListType list,
                                      const struct PP_TouchPoint *point);

PP_Bool
ppb_touch_input_event_is_touch_input_event(PP_Resource resource);

uint32_t
ppb_touch_input_event_get_touch_count(PP_Resource resource, PP_TouchListType list);

struct PP_TouchPoint
ppb_touch_input_event_get_touch_by_index(PP_Resource resource, PP_TouchListType list,
                                         uint32_t index);

struct PP_TouchPoint
ppb_touch_input_event_get_touch_by_id(PP_Resource resource, PP_TouchListType list,
                                      uint32_t touch_id);

PP_Resource
ppb_ime_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                           struct PP_Var text, uint32_t segment_number,
                           const uint32_t segment_offsets[], int32_t target_segment,
                           uint32_t selection_start, uint32_t selection_end);

PP_Bool
ppb_ime_input_event_is_ime_input_event(PP_Resource resource);

struct PP_Var
ppb_ime_input_event_get_text(PP_Resource ime_event);

uint32_t
ppb_ime_input_event_get_segment_number(PP_Resource ime_event);

uint32_t
ppb_ime_input_event_get_segment_offset(PP_Resource ime_event, uint32_t index);

int32_t
ppb_ime_input_event_get_target_segment(PP_Resource ime_event);

void
ppb_ime_input_event_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end);
