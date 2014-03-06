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

#include "ppb_text_input_dev.h"
#include <stdlib.h>
#include "trace.h"
#include "reverse_constant.h"


void
ppb_text_input_dev_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type)
{
    return;
}

void
ppb_text_input_dev_interface_update_caret_position(PP_Instance instance,
                                                   const struct PP_Rect *caret,
                                                   const struct PP_Rect *bounding_box)
{
    return;
}

void
ppb_text_input_dev_interface_cancel_composition_text(PP_Instance instance)
{
    return;
}

void
ppb_text_input_dev_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                     uint32_t caret, uint32_t anchor)
{
    return;
}

void
ppb_text_input_dev_interface_selection_changed(PP_Instance instance)
{
    return;
}

// trace wrappers
static
void
trace_ppb_text_input_dev_interface_set_text_input_type(PP_Instance instance,
                                                       PP_TextInput_Type_Dev type)
{
    trace_info("[PPB] {zilch} %s instance=%d, type=%s\n", __func__+6, instance,
               reverse_pp_text_input_type(type));
    ppb_text_input_dev_interface_set_text_input_type(instance, type);
}

static
void
trace_ppb_text_input_dev_interface_update_caret_position(PP_Instance instance,
                                                         const struct PP_Rect *caret,
                                                         const struct PP_Rect *bounding_box)
{
    char *caret_str = trace_rect_as_string(caret);
    char *bounding_box_str = trace_rect_as_string(bounding_box);
    trace_info("[PPB] {zilch} %s instance=%d, caret=%s, bounding_box=%s\n", __func__+6, instance,
               caret_str, bounding_box_str);
    free(caret_str);
    free(bounding_box_str);
    ppb_text_input_dev_interface_update_caret_position(instance, caret, bounding_box);
}

static
void
trace_ppb_text_input_dev_interface_cancel_composition_text(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_text_input_dev_interface_cancel_composition_text(instance);
}

static
void
trace_ppb_text_input_dev_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                           uint32_t caret, uint32_t anchor)
{
    trace_info("[PPB] {zilch} %s instance=%d, text=%s, caret=%u, anchor=%u\n", __func__+6, instance,
               text, caret, anchor);
    ppb_text_input_dev_interface_update_surrounding_text(instance, text, caret, anchor);
}

static
void
trace_ppb_text_input_dev_interface_selection_changed(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_text_input_dev_interface_selection_changed(instance);
}


const struct PPB_TextInput_Dev_0_2 ppb_text_input_dev_interface_0_2 = {
    .SetTextInputType =         trace_ppb_text_input_dev_interface_set_text_input_type,
    .UpdateCaretPosition =      trace_ppb_text_input_dev_interface_update_caret_position,
    .CancelCompositionText =    trace_ppb_text_input_dev_interface_cancel_composition_text,
    .UpdateSurroundingText =    trace_ppb_text_input_dev_interface_update_surrounding_text,
    .SelectionChanged =         trace_ppb_text_input_dev_interface_selection_changed,
};
