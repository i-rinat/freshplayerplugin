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

#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_input_event.h"
#include "ppb_instance.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include "utils.h"
#include <glib.h>
#include <ppapi/c/pp_errors.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

struct pp_input_event_s {
    COMMON_STRUCTURE_FIELDS
    uint32_t                    event_class;
    PP_InputEvent_Type          type;
    PP_TimeTicks                time_stamp;
    uint32_t                    modifiers;
    PP_InputEvent_MouseButton   mouse_button;
    struct PP_Point             mouse_position;
    int32_t                     click_count;
    struct PP_Point             mouse_movement;
    struct PP_FloatPoint        wheel_delta;
    struct PP_FloatPoint        wheel_ticks;
    PP_Bool                     scroll_by_page;
    uint32_t                    key_code;
    struct PP_Var               code;
    struct PP_Var               text;
    uint32_t                    segment_number;
    uint32_t                   *segment_offsets;
    int32_t                     target_segment;
    uint32_t                    selection_start;
    uint32_t                    selection_end;
};

STATIC_ASSERT(sizeof(struct pp_input_event_s) <= LARGEST_RESOURCE_SIZE);

static
void
ppb_input_event_destroy(void *p)
{
    struct pp_input_event_s *ie = p;

    ppb_var_release(ie->code);
    ppb_var_release(ie->text);
    free_and_nullify(ie->segment_offsets);
}

int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_ERROR_BADARGUMENT;
    }

    pthread_mutex_lock(&display.lock);
    pp_i->event_mask |= event_classes;
    pthread_mutex_unlock(&display.lock);
    return PP_OK;
}

int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_ERROR_BADARGUMENT;
    }

    pthread_mutex_lock(&display.lock);
    pp_i->filtered_event_mask |= event_classes;
    pthread_mutex_unlock(&display.lock);
    return PP_OK;
}

void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    pthread_mutex_lock(&display.lock);
    pp_i->event_mask &= ~event_classes;
    pp_i->filtered_event_mask &= ~event_classes;
    pthread_mutex_unlock(&display.lock);
    return;
}

PP_Bool
ppb_input_event_is_input_event(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_INPUT_EVENT;
}

PP_InputEvent_Type
ppb_input_event_get_type(PP_Resource event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_INPUTEVENT_TYPE_UNDEFINED;
    }
    PP_InputEvent_Type t = ie->type;
    pp_resource_release(event);
    return t;
}

PP_TimeTicks
ppb_input_event_get_time_stamp(PP_Resource event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_INPUTEVENT_TYPE_UNDEFINED;
    }
    PP_TimeTicks time_stamp = ie->time_stamp;
    pp_resource_release(event);
    return time_stamp;
}

uint32_t
ppb_input_event_get_modifiers(PP_Resource event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    uint32_t modifiers = ie->modifiers;
    pp_resource_release(event);
    return modifiers;
}

PP_Resource
ppb_mouse_input_event_create(PP_Instance instance, PP_InputEvent_Type type, PP_TimeTicks time_stamp,
                             uint32_t modifiers, PP_InputEvent_MouseButton mouse_button,
                             const struct PP_Point *mouse_position, int32_t click_count,
                             const struct PP_Point *mouse_movement)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource input_event = pp_resource_allocate(PP_RESOURCE_INPUT_EVENT, pp_i);
    struct pp_input_event_s *ie = pp_resource_acquire(input_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, can't allocate memory\n", __func__);
        return 0;
    }
    ie->event_class = PP_INPUTEVENT_CLASS_MOUSE;
    ie->type = type;
    ie->time_stamp = time_stamp;
    ie->modifiers = modifiers;
    ie->mouse_button = mouse_button;
    ie->mouse_position.x = mouse_position ? mouse_position->x : 0;
    ie->mouse_position.y = mouse_position ? mouse_position->y : 0;
    ie->click_count = click_count;
    ie->mouse_movement.x = mouse_movement ? mouse_movement->x : 0;
    ie->mouse_movement.y = mouse_movement ? mouse_movement->y : 0;

    pp_resource_release(input_event);
    return input_event;
}

PP_Bool
ppb_mouse_input_event_is_mouse_input_event(PP_Resource resource)
{
    struct pp_input_event_s *ie = pp_resource_acquire(resource, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    PP_Bool res = ie->event_class == PP_INPUTEVENT_CLASS_MOUSE;
    pp_resource_release(resource);
    return res;
}

PP_InputEvent_MouseButton
ppb_mouse_input_event_get_button(PP_Resource mouse_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(mouse_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_INPUTEVENT_MOUSEBUTTON_NONE;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_MOUSE) {
        trace_error("%s, not a mouse event\n", __func__);
        pp_resource_release(mouse_event);
        return PP_INPUTEVENT_MOUSEBUTTON_NONE;
    }

    PP_InputEvent_MouseButton mb = ie->mouse_button;
    pp_resource_release(mouse_event);
    return mb;
}

struct PP_Point
ppb_mouse_input_event_get_position(PP_Resource mouse_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(mouse_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakePoint(0, 0);
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_MOUSE) {
        trace_error("%s, not a mouse event\n", __func__);
        pp_resource_release(mouse_event);
        return PP_MakePoint(0, 0);
    }

    struct PP_Point res = PP_MakePoint(ie->mouse_position.x, ie->mouse_position.y);
    pp_resource_release(mouse_event);
    return res;
}

int32_t
ppb_mouse_input_event_get_click_count(PP_Resource mouse_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(mouse_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_MOUSE) {
        trace_error("%s, not a mouse event\n", __func__);
        pp_resource_release(mouse_event);
        return 0;
    }

    uint32_t click_count = ie->click_count;
    pp_resource_release(mouse_event);
    return click_count;
}

struct PP_Point
ppb_mouse_input_event_get_movement(PP_Resource mouse_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(mouse_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakePoint(0, 0);
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_MOUSE) {
        trace_error("%s, not a mouse event\n", __func__);
        pp_resource_release(mouse_event);
        return PP_MakePoint(0, 0);
    }

    struct PP_Point mouse_movement = ie->mouse_movement;
    pp_resource_release(mouse_event);
    return mouse_movement;
}

PP_Resource
ppb_wheel_input_event_create(PP_Instance instance, PP_TimeTicks time_stamp, uint32_t modifiers,
                             const struct PP_FloatPoint *wheel_delta,
                             const struct PP_FloatPoint *wheel_ticks, PP_Bool scroll_by_page)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource input_event = pp_resource_allocate(PP_RESOURCE_INPUT_EVENT, pp_i);
    struct pp_input_event_s *ie = pp_resource_acquire(input_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, can't allocate memory\n", __func__);
        return 0;
    }
    ie->event_class = PP_INPUTEVENT_CLASS_WHEEL;
    ie->type = PP_INPUTEVENT_TYPE_WHEEL;
    ie->time_stamp = time_stamp;
    ie->modifiers = modifiers;
    ie->wheel_delta.x = wheel_delta ? wheel_delta->x : 0;
    ie->wheel_delta.y = wheel_delta ? wheel_delta->y : 0;
    ie->wheel_ticks.x = wheel_ticks ? wheel_ticks->x : 0;
    ie->wheel_ticks.y = wheel_ticks ? wheel_ticks->y : 0;
    ie->scroll_by_page = scroll_by_page;

    pp_resource_release(input_event);
    return input_event;
}

PP_Bool
ppb_wheel_input_event_is_wheel_input_event(PP_Resource resource)
{
    struct pp_input_event_s *ie = pp_resource_acquire(resource, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    PP_Bool res = ie->event_class == PP_INPUTEVENT_CLASS_WHEEL;
    pp_resource_release(resource);
    return res;
}

struct PP_FloatPoint
ppb_wheel_input_event_get_delta(PP_Resource wheel_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(wheel_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeFloatPoint(0, 0);
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_WHEEL) {
        trace_error("%s, not a wheel event\n", __func__);
        pp_resource_release(wheel_event);
        return PP_MakeFloatPoint(0, 0);
    }

    struct PP_FloatPoint ret = ie->wheel_delta;
    pp_resource_release(wheel_event);
    return ret;
}

struct PP_FloatPoint
ppb_wheel_input_event_get_ticks(PP_Resource wheel_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(wheel_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeFloatPoint(0, 0);
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_WHEEL) {
        trace_error("%s, not a wheel event\n", __func__);
        pp_resource_release(wheel_event);
        return PP_MakeFloatPoint(0, 0);
    }

    struct PP_FloatPoint ret = ie->wheel_ticks;
    pp_resource_release(wheel_event);
    return ret;
}

PP_Bool
ppb_wheel_input_event_get_scroll_by_page(PP_Resource wheel_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(wheel_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_WHEEL) {
        trace_error("%s, not a wheel event\n", __func__);
        pp_resource_release(wheel_event);
        return PP_FALSE;
    }

    PP_Bool ret = ie->scroll_by_page;
    pp_resource_release(wheel_event);
    return ret;
}

PP_Resource
ppb_keyboard_input_event_create_1_0(PP_Instance instance, PP_InputEvent_Type type,
                                    PP_TimeTicks time_stamp, uint32_t modifiers, uint32_t key_code,
                                    struct PP_Var character_text)
{
    return ppb_keyboard_input_event_create_1_2(instance, type, time_stamp, modifiers, key_code,
                                               character_text, PP_MakeUndefined());
}

PP_Resource
ppb_keyboard_input_event_create_1_2(PP_Instance instance, PP_InputEvent_Type type,
                                    PP_TimeTicks time_stamp, uint32_t modifiers, uint32_t key_code,
                                    struct PP_Var character_text, struct PP_Var code)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource input_event = pp_resource_allocate(PP_RESOURCE_INPUT_EVENT, pp_i);
    struct pp_input_event_s *ie = pp_resource_acquire(input_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, can't allocate memory\n", __func__);
        return 0;
    }
    ie->event_class = PP_INPUTEVENT_CLASS_KEYBOARD;
    ie->type = type;
    ie->time_stamp = time_stamp;
    ie->modifiers = modifiers;
    ie->key_code = key_code;
    ie->text = ppb_var_add_ref2(character_text);
    ie->code = ppb_var_add_ref2(code);

    pp_resource_release(input_event);
    return input_event;
}

PP_Bool
ppb_keyboard_input_event_is_keyboard_input_event(PP_Resource resource)
{
    struct pp_input_event_s *ie = pp_resource_acquire(resource, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    PP_Bool res = ie->event_class == PP_INPUTEVENT_CLASS_KEYBOARD;
    pp_resource_release(resource);
    return res;
}

uint32_t
ppb_keyboard_input_event_get_key_code(PP_Resource key_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(key_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_KEYBOARD) {
        trace_error("%s, not a keyboard event\n", __func__);
        pp_resource_release(key_event);
        return 0;
    }

    uint32_t key_code = ie->key_code;
    pp_resource_release(key_event);
    return key_code;
}

struct PP_Var
ppb_keyboard_input_event_get_character_text(PP_Resource character_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(character_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_KEYBOARD) {
        trace_error("%s, not a keyboard event\n", __func__);
        pp_resource_release(character_event);
        return PP_MakeUndefined();
    }

    struct PP_Var character_text = ppb_var_add_ref2(ie->text);
    pp_resource_release(character_event);
    return character_text;
}

struct PP_Var
ppb_keyboard_input_event_get_code(PP_Resource key_event)
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
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    if (type != PP_INPUTEVENT_TYPE_IME_COMPOSITION_START &&
        type != PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE &&
        type != PP_INPUTEVENT_TYPE_IME_COMPOSITION_END &&
        type != PP_INPUTEVENT_TYPE_IME_TEXT)
    {
        trace_warning("%s, wrong type=%d\n", __func__, type);
        return 0;
    }

    PP_Resource input_event = pp_resource_allocate(PP_RESOURCE_INPUT_EVENT, pp_i);
    struct pp_input_event_s *ie = pp_resource_acquire(input_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, can't allocate memory\n", __func__);
        return 0;
    }
    ie->event_class =     PP_INPUTEVENT_CLASS_IME;
    ie->type =            type;
    ie->time_stamp =      time_stamp;
    ie->text =            ppb_var_add_ref2(text);
    ie->segment_number =  segment_number;
    ie->segment_offsets = NULL;
    if (segment_number > 0) {
        ie->segment_offsets = malloc((segment_number + 1) * sizeof(uint32_t));
        memcpy(ie->segment_offsets, segment_offsets, (segment_number + 1) * sizeof(uint32_t));
    }
    ie->target_segment =  target_segment;
    ie->selection_start = selection_start;
    ie->selection_end =   selection_end;

    pp_resource_release(input_event);
    return input_event;
}

PP_Bool
ppb_ime_input_event_is_ime_input_event(PP_Resource resource)
{
    struct pp_input_event_s *ie = pp_resource_acquire(resource, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    PP_Bool res = ie->event_class == PP_INPUTEVENT_CLASS_IME;
    pp_resource_release(resource);
    return res;
}

struct PP_Var
ppb_ime_input_event_get_text(PP_Resource ime_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(ime_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_IME) {
        trace_error("%s, not an IME event\n", __func__);
        pp_resource_release(ime_event);
        return PP_MakeUndefined();
    }

    struct PP_Var text = ppb_var_add_ref2(ie->text);
    pp_resource_release(ime_event);
    return text;
}

uint32_t
ppb_ime_input_event_get_segment_number(PP_Resource ime_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(ime_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_IME) {
        trace_error("%s, not an IME event\n", __func__);
        pp_resource_release(ime_event);
        return 0;
    }

    uint32_t segment_number = ie->segment_number;
    pp_resource_release(ime_event);
    return segment_number;
}

uint32_t
ppb_ime_input_event_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    struct pp_input_event_s *ie = pp_resource_acquire(ime_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_IME) {
        trace_error("%s, not an IME event\n", __func__);
        pp_resource_release(ime_event);
        return 0;
    }

    uint32_t segment_offset = (index <= ie->segment_number) ? ie->segment_offsets[index] : 0;
    pp_resource_release(ime_event);
    return segment_offset;
}

int32_t
ppb_ime_input_event_get_target_segment(PP_Resource ime_event)
{
    struct pp_input_event_s *ie = pp_resource_acquire(ime_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_IME) {
        trace_error("%s, not an IME event\n", __func__);
        pp_resource_release(ime_event);
        return 0;
    }

    int32_t target_segment = ie->target_segment;
    pp_resource_release(ime_event);
    return target_segment;
}

void
ppb_ime_input_event_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
    struct pp_input_event_s *ie = pp_resource_acquire(ime_event, PP_RESOURCE_INPUT_EVENT);
    if (!ie) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }
    if (ie->event_class != PP_INPUTEVENT_CLASS_IME) {
        trace_error("%s, not an IME event\n", __func__);
        pp_resource_release(ime_event);
        return;
    }

    if (start)
        *start = ie->selection_start;
    if (end)
        *end = ie->selection_end;
    pp_resource_release(ime_event);
}


// trace wrappers
TRACE_WRAPPER
int32_t
trace_ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_input_events(instance, event_classes);
}

TRACE_WRAPPER
int32_t
trace_ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    return ppb_input_event_request_filtering_input_events(instance, event_classes);
}

TRACE_WRAPPER
void
trace_ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    char *s_event_classes = trace_event_classes_as_string(event_classes);
    trace_info("[PPB] {full} %s instance=%d, event_classes=%s\n", __func__+6, instance,
               s_event_classes);
    free(s_event_classes);
    ppb_input_event_clear_input_event_request(instance, event_classes);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_input_event_is_input_event(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_input_event_is_input_event(resource);
}

TRACE_WRAPPER
PP_InputEvent_Type
trace_ppb_input_event_get_type(PP_Resource event)
{
    trace_info("[PPB] {full} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_type(event);
}

TRACE_WRAPPER
PP_TimeTicks
trace_ppb_input_event_get_time_stamp(PP_Resource event)
{
    trace_info("[PPB] {full} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_time_stamp(event);
}

TRACE_WRAPPER
uint32_t
trace_ppb_input_event_get_modifiers(PP_Resource event)
{
    trace_info("[PPB] {full} %s event=%d\n", __func__+6, event);
    return ppb_input_event_get_modifiers(event);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_mouse_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                   PP_TimeTicks time_stamp, uint32_t modifiers,
                                   PP_InputEvent_MouseButton mouse_button,
                                   const struct PP_Point *mouse_position, int32_t click_count,
                                   const struct PP_Point *mouse_movement)
{
    char *s_mouse_position = trace_point_as_string(mouse_position);
    char *s_mouse_movement = trace_point_as_string(mouse_movement);
    trace_info("[PPB] {full} %s instance=%d, type=%d, time_stamp=%f, modifiers=0x%x, "
               "mouse_button=%d, mouse_position=%s, click_count=%d, mouse_movement=%s\n",
               __func__+6, instance, type, time_stamp, modifiers, mouse_button, s_mouse_position,
               click_count, s_mouse_movement);
    g_free(s_mouse_position);
    g_free(s_mouse_movement);
    return ppb_mouse_input_event_create(instance, type, time_stamp, modifiers, mouse_button,
                                        mouse_position, click_count, mouse_movement);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_mouse_input_event_is_mouse_input_event(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_mouse_input_event_is_mouse_input_event(resource);
}

TRACE_WRAPPER
PP_InputEvent_MouseButton
trace_ppb_mouse_input_event_get_button(PP_Resource mouse_event)
{
    trace_info("[PPB] {full} %s mouse_event=%d\n", __func__+6, mouse_event);
    return ppb_mouse_input_event_get_button(mouse_event);
}

TRACE_WRAPPER
struct PP_Point
trace_ppb_mouse_input_event_get_position(PP_Resource mouse_event)
{
    trace_info("[PPB] {full} %s mouse_event=%d\n", __func__+6, mouse_event);
    return ppb_mouse_input_event_get_position(mouse_event);
}

TRACE_WRAPPER
int32_t
trace_ppb_mouse_input_event_get_click_count(PP_Resource mouse_event)
{
    trace_info("[PPB] {full} %s mouse_event=%d\n", __func__+6, mouse_event);
    return ppb_mouse_input_event_get_click_count(mouse_event);
}

TRACE_WRAPPER
struct PP_Point
trace_ppb_mouse_input_event_get_movement(PP_Resource mouse_event)
{
    trace_info("[PPB] {full} %s mouse_event=%d\n", __func__+6, mouse_event);
    return ppb_mouse_input_event_get_movement(mouse_event);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_wheel_input_event_create(PP_Instance instance, PP_TimeTicks time_stamp,
                                   uint32_t modifiers, const struct PP_FloatPoint *wheel_delta,
                                   const struct PP_FloatPoint *wheel_ticks, PP_Bool scroll_by_page)
{
    char *s_wheel_delta = trace_float_point_as_string(wheel_delta);
    char *s_wheel_ticks = trace_float_point_as_string(wheel_ticks);
    trace_info("[PPB] {full} %s instance=%d, time_stamp=%f, modifiers=0x%x, wheel_delta=%s, "
               "wheel_ticks=%s, scrool_by_page=%d\n", __func__+6, instance, time_stamp, modifiers,
               s_wheel_delta, s_wheel_ticks, scroll_by_page);
    g_free(s_wheel_delta);
    g_free(s_wheel_ticks);
    return ppb_wheel_input_event_create(instance, time_stamp, modifiers, wheel_delta, wheel_ticks,
                                        scroll_by_page);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_wheel_input_event_is_wheel_input_event(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_wheel_input_event_is_wheel_input_event(resource);
}

TRACE_WRAPPER
struct PP_FloatPoint
trace_ppb_wheel_input_event_get_delta(PP_Resource wheel_event)
{
    trace_info("[PPB] {full} %s wheel_event=%d\n", __func__+6, wheel_event);
    return ppb_wheel_input_event_get_delta(wheel_event);
}

TRACE_WRAPPER
struct PP_FloatPoint
trace_ppb_wheel_input_event_get_ticks(PP_Resource wheel_event)
{
    trace_info("[PPB] {full} %s wheel_event=%d\n", __func__+6, wheel_event);
    return ppb_wheel_input_event_get_ticks(wheel_event);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_wheel_input_event_get_scroll_by_page(PP_Resource wheel_event)
{
    trace_info("[PPB] {full} %s wheel_event=%d\n", __func__+6, wheel_event);
    return ppb_wheel_input_event_get_scroll_by_page(wheel_event);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_keyboard_input_event_create_1_0(PP_Instance instance, PP_InputEvent_Type type,
                                          PP_TimeTicks time_stamp, uint32_t modifiers,
                                          uint32_t key_code, struct PP_Var character_text)
{
    char *s_character_text = trace_var_as_string(character_text);
    trace_info("[PPB] {full} %s instance=%d, type=%d, time_stamp=%f, modifiers=0x%x, "
               "key_code=%u, character_text=%s\n", __func__+6, instance, type, time_stamp,
               modifiers, key_code, s_character_text);
    g_free(s_character_text);
    return ppb_keyboard_input_event_create_1_0(instance, type, time_stamp, modifiers, key_code,
                                               character_text);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_keyboard_input_event_create_1_2(PP_Instance instance, PP_InputEvent_Type type,
                                          PP_TimeTicks time_stamp, uint32_t modifiers,
                                          uint32_t key_code, struct PP_Var character_text,
                                          struct PP_Var code)
{
    char *s_character_text = trace_var_as_string(character_text);
    char *s_code = trace_var_as_string(code);
    trace_info("[PPB] {full} %s instance=%d, type=%d, time_stamp=%f, modifiers=0x%x, "
               "key_code=%u, character_text=%s, code=%s\n", __func__+6, instance, type, time_stamp,
               modifiers, key_code, s_character_text, s_code);
    g_free(s_character_text);
    g_free(s_code);
    return ppb_keyboard_input_event_create_1_2(instance, type, time_stamp, modifiers, key_code,
                                               character_text, code);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_keyboard_input_event_is_keyboard_input_event(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_keyboard_input_event_is_keyboard_input_event(resource);
}

TRACE_WRAPPER
uint32_t
trace_ppb_keyboard_input_event_get_key_code(PP_Resource key_event)
{
    trace_info("[PPB] {full} %s key_event=%d\n", __func__+6, key_event);
    return ppb_keyboard_input_event_get_key_code(key_event);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_keyboard_input_event_get_character_text(PP_Resource character_event)
{
    trace_info("[PPB] {full} %s character_event=%d\n", __func__+6, character_event);
    return ppb_keyboard_input_event_get_character_text(character_event);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_keyboard_input_event_get_code(PP_Resource key_event)
{
    trace_info("[PPB] {zilch} %s key_event=%d\n", __func__+6, key_event);
    return ppb_keyboard_input_event_get_code(key_event);
}


TRACE_WRAPPER
PP_Resource
trace_ppb_touch_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                   PP_TimeTicks time_stamp, uint32_t modifiers)
{
    trace_info("[PPB] {zilch} %s instance=%d, type=%d, time_stamp=%f, modifiers=0x%x\n",
               __func__+6, instance, type, time_stamp, modifiers);
    return ppb_touch_input_event_create(instance, type, time_stamp, modifiers);
}

TRACE_WRAPPER
void
trace_ppb_touch_input_event_add_touch_point(PP_Resource touch_event, PP_TouchListType list,
                                            const struct PP_TouchPoint *point)
{
    char *s_point = trace_touch_point_as_string(point);
    trace_info("[PPB] {zilch} %s touch_event=%d, list=%d, point=%s\n", __func__+6, touch_event,
               list, s_point);
    g_free(s_point);
    return ppb_touch_input_event_add_touch_point(touch_event, list, point);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_touch_input_event_is_touch_input_event(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_touch_input_event_is_touch_input_event(resource);
}

TRACE_WRAPPER
uint32_t
trace_ppb_touch_input_event_get_touch_count(PP_Resource resource, PP_TouchListType list)
{
    trace_info("[PPB] {zilch} %s resource=%d, list=%d\n", __func__+6, resource, list);
    return ppb_touch_input_event_get_touch_count(resource, list);
}

TRACE_WRAPPER
struct PP_TouchPoint
trace_ppb_touch_input_event_get_touch_by_index(PP_Resource resource, PP_TouchListType list,
                                               uint32_t index)
{
    trace_info("[PPB] {zilch} %s resource=%d, list=%d, index=%u\n", __func__+6, resource, list,
               index);
    return ppb_touch_input_event_get_touch_by_index(resource, list, index);
}

TRACE_WRAPPER
struct PP_TouchPoint
trace_ppb_touch_input_event_get_touch_by_id(PP_Resource resource, PP_TouchListType list,
                                            uint32_t touch_id)
{
    trace_info("[PPB] {zilch} %s resource=%d, list=%d, touch_id=%u\n", __func__+6, resource, list,
               touch_id);
    return ppb_touch_input_event_get_touch_by_id(resource, list, touch_id);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_ime_input_event_create(PP_Instance instance, PP_InputEvent_Type type,
                                 PP_TimeTicks time_stamp, struct PP_Var text,
                                 uint32_t segment_number, const uint32_t segment_offsets[],
                                 int32_t target_segment, uint32_t selection_start,
                                 uint32_t selection_end)
{
    char *s_text = trace_var_as_string(text);
    trace_info("[PPB] {full} %s instance=%d, type=%d, time_stamp=%f, text=%s, segment_number=%u, "
               "segment_offsets=%p, target_segment=%d, selection_start=%u, selection_end=%u\n",
               __func__+6, instance, type, time_stamp, s_text, segment_number, segment_offsets,
               target_segment, selection_start, selection_end);
    g_free(s_text);
    return ppb_ime_input_event_create(instance, type, time_stamp, text, segment_number,
                                      segment_offsets, target_segment, selection_start,
                                      selection_end);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_ime_input_event_is_ime_input_event(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_ime_input_event_is_ime_input_event(resource);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_ime_input_event_get_text(PP_Resource ime_event)
{
    trace_info("[PPB] {full} %s ime_event=%d\n", __func__+6, ime_event);
    return ppb_ime_input_event_get_text(ime_event);
}

TRACE_WRAPPER
uint32_t
trace_ppb_ime_input_event_get_segment_number(PP_Resource ime_event)
{
    trace_info("[PPB] {full} %s ime_event=%d\n", __func__+6, ime_event);
    return ppb_ime_input_event_get_segment_number(ime_event);
}

TRACE_WRAPPER
uint32_t
trace_ppb_ime_input_event_get_segment_offset(PP_Resource ime_event, uint32_t index)
{
    trace_info("[PPB] {full} %s ime_event=%d, index=%u\n", __func__+6, ime_event, index);
    return ppb_ime_input_event_get_segment_offset(ime_event, index);
}

TRACE_WRAPPER
int32_t
trace_ppb_ime_input_event_get_target_segment(PP_Resource ime_event)
{
    trace_info("[PPB] {full} %s ime_event=%d\n", __func__+6, ime_event);
    return ppb_ime_input_event_get_target_segment(ime_event);
}

TRACE_WRAPPER
void
trace_ppb_ime_input_event_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end)
{
    trace_info("[PPB] {full} %s ime_event=%d\n", __func__+6, ime_event);
    return ppb_ime_input_event_get_selection(ime_event, start, end);
}


const struct PPB_InputEvent_1_0 ppb_input_event_interface_1_0 = {
    .RequestInputEvents =           TWRAPF(ppb_input_event_request_input_events),
    .RequestFilteringInputEvents =  TWRAPF(ppb_input_event_request_filtering_input_events),
    .ClearInputEventRequest =       TWRAPF(ppb_input_event_clear_input_event_request),
    .IsInputEvent =                 TWRAPF(ppb_input_event_is_input_event),
    .GetType =                      TWRAPF(ppb_input_event_get_type),
    .GetTimeStamp =                 TWRAPF(ppb_input_event_get_time_stamp),
    .GetModifiers =                 TWRAPF(ppb_input_event_get_modifiers),
};

const struct PPB_MouseInputEvent_1_1 ppb_mouse_input_event_interface_1_1 = {
    .Create =               TWRAPF(ppb_mouse_input_event_create),
    .IsMouseInputEvent =    TWRAPF(ppb_mouse_input_event_is_mouse_input_event),
    .GetButton =            TWRAPF(ppb_mouse_input_event_get_button),
    .GetPosition =          TWRAPF(ppb_mouse_input_event_get_position),
    .GetClickCount =        TWRAPF(ppb_mouse_input_event_get_click_count),
    .GetMovement =          TWRAPF(ppb_mouse_input_event_get_movement),
};

const struct PPB_WheelInputEvent_1_0 ppb_wheel_input_event_interface_1_0 = {
    .Create =               TWRAPF(ppb_wheel_input_event_create),
    .IsWheelInputEvent =    TWRAPF(ppb_wheel_input_event_is_wheel_input_event),
    .GetDelta =             TWRAPF(ppb_wheel_input_event_get_delta),
    .GetTicks =             TWRAPF(ppb_wheel_input_event_get_ticks),
    .GetScrollByPage =      TWRAPF(ppb_wheel_input_event_get_scroll_by_page),
};

const struct PPB_KeyboardInputEvent_1_0 ppb_keyboard_input_event_interface_1_0 = {
    .Create =               TWRAPF(ppb_keyboard_input_event_create_1_0),
    .IsKeyboardInputEvent = TWRAPF(ppb_keyboard_input_event_is_keyboard_input_event),
    .GetKeyCode =           TWRAPF(ppb_keyboard_input_event_get_key_code),
    .GetCharacterText =     TWRAPF(ppb_keyboard_input_event_get_character_text),
};

const struct PPB_KeyboardInputEvent_1_2 ppb_keyboard_input_event_interface_1_2 = {
    .Create =               TWRAPF(ppb_keyboard_input_event_create_1_2),
    .IsKeyboardInputEvent = TWRAPF(ppb_keyboard_input_event_is_keyboard_input_event),
    .GetKeyCode =           TWRAPF(ppb_keyboard_input_event_get_key_code),
    .GetCharacterText =     TWRAPF(ppb_keyboard_input_event_get_character_text),
    .GetCode =              TWRAPZ(ppb_keyboard_input_event_get_code),
};

const struct PPB_TouchInputEvent_1_0 ppb_touch_input_event_interface_1_0 = {
    .Create =               TWRAPZ(ppb_touch_input_event_create),
    .AddTouchPoint =        TWRAPZ(ppb_touch_input_event_add_touch_point),
    .IsTouchInputEvent =    TWRAPZ(ppb_touch_input_event_is_touch_input_event),
    .GetTouchCount =        TWRAPZ(ppb_touch_input_event_get_touch_count),
    .GetTouchByIndex =      TWRAPZ(ppb_touch_input_event_get_touch_by_index),
    .GetTouchById =         TWRAPZ(ppb_touch_input_event_get_touch_by_id),
};

const struct PPB_IMEInputEvent_Dev_0_2 ppb_ime_input_event_dev_interface_0_2 = {
    .Create =           TWRAPF(ppb_ime_input_event_create),
    .IsIMEInputEvent =  TWRAPF(ppb_ime_input_event_is_ime_input_event),
    .GetText =          TWRAPF(ppb_ime_input_event_get_text),
    .GetSegmentNumber = TWRAPF(ppb_ime_input_event_get_segment_number),
    .GetSegmentOffset = TWRAPF(ppb_ime_input_event_get_segment_offset),
    .GetTargetSegment = TWRAPF(ppb_ime_input_event_get_target_segment),
    .GetSelection =     TWRAPF(ppb_ime_input_event_get_selection),
};

const struct PPB_IMEInputEvent_1_0 ppb_ime_input_event_interface_1_0 = {
    .Create =           TWRAPF(ppb_ime_input_event_create),
    .IsIMEInputEvent =  TWRAPF(ppb_ime_input_event_is_ime_input_event),
    .GetText =          TWRAPF(ppb_ime_input_event_get_text),
    .GetSegmentNumber = TWRAPF(ppb_ime_input_event_get_segment_number),
    .GetSegmentOffset = TWRAPF(ppb_ime_input_event_get_segment_offset),
    .GetTargetSegment = TWRAPF(ppb_ime_input_event_get_target_segment),
    .GetSelection =     TWRAPF(ppb_ime_input_event_get_selection),
};

static
void
__attribute__((constructor))
constructor_ppb_input_event(void)
{
    register_interface(PPB_IME_INPUT_EVENT_DEV_INTERFACE_0_2,
                       &ppb_ime_input_event_dev_interface_0_2);
    register_interface(PPB_INPUT_EVENT_INTERFACE_1_0, &ppb_input_event_interface_1_0);
    register_interface(PPB_MOUSE_INPUT_EVENT_INTERFACE_1_1, &ppb_mouse_input_event_interface_1_1);
    register_interface(PPB_WHEEL_INPUT_EVENT_INTERFACE_1_0, &ppb_wheel_input_event_interface_1_0);
    register_interface(PPB_KEYBOARD_INPUT_EVENT_INTERFACE_1_0,
                       &ppb_keyboard_input_event_interface_1_0);
    register_interface(PPB_KEYBOARD_INPUT_EVENT_INTERFACE_1_2,
                       &ppb_keyboard_input_event_interface_1_2);
    register_interface(PPB_TOUCH_INPUT_EVENT_INTERFACE_1_0, &ppb_touch_input_event_interface_1_0);
    register_interface(PPB_IME_INPUT_EVENT_INTERFACE_1_0, &ppb_ime_input_event_interface_1_0);
    register_resource(PP_RESOURCE_INPUT_EVENT, ppb_input_event_destroy);
}
