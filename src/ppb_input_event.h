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

#ifndef FPP__PPB_INPUT_EVENT_H
#define FPP__PPB_INPUT_EVENT_H

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

#endif // FPP__PPB_INPUT_EVENT_H
