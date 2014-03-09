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

#ifndef FPP__PPB_IME_INPUT_EVENT_DEV_H
#define FPP__PPB_IME_INPUT_EVENT_DEV_H

#include <ppapi/c/dev/ppb_ime_input_event_dev.h>


PP_Resource
ppb_ime_input_event_dev_create(PP_Instance instance, PP_InputEvent_Type type,
                               PP_TimeTicks time_stamp, struct PP_Var text, uint32_t segment_number,
                               const uint32_t segment_offsets[], int32_t target_segment,
                               uint32_t selection_start, uint32_t selection_end);

PP_Bool
ppb_ime_input_event_dev_is_ime_input_event(PP_Resource resource);

struct PP_Var
ppb_ime_input_event_dev_get_text(PP_Resource ime_event);

uint32_t
ppb_ime_input_event_dev_get_segment_number(PP_Resource ime_event);

uint32_t
ppb_ime_input_event_dev_get_segment_offset(PP_Resource ime_event, uint32_t index);

int32_t
ppb_ime_input_event_dev_get_target_segment(PP_Resource ime_event);

void
ppb_ime_input_event_dev_get_selection(PP_Resource ime_event, uint32_t *start, uint32_t *end);

#endif // FPP__PPB_IME_INPUT_EVENT_DEV_H
