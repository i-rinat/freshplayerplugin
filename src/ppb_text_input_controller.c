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
#include "ppb_text_input_controller.h"
#include "trace_core.h"

void
ppb_text_input_controller_set_text_input_type(PP_Instance instance, PP_TextInput_Type type)
{
}

void
ppb_text_input_controller_update_caret_position(PP_Instance instance, const struct PP_Rect *caret)
{
}

void
ppb_text_input_controller_cancel_composition_text(PP_Instance instance)
{
}

void
ppb_text_input_controller_update_surrounding_text(PP_Instance instance, struct PP_Var text,
                                                  uint32_t caret, uint32_t anchor)
{
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_text_input_controller_set_text_input_type(PP_Instance instance, PP_TextInput_Type type)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_text_input_controller_set_text_input_type(instance, type);
}

TRACE_WRAPPER
void
trace_ppb_text_input_controller_update_caret_position(PP_Instance instance,
                                                      const struct PP_Rect *caret)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_text_input_controller_update_caret_position(instance, caret);
}

TRACE_WRAPPER
void
trace_ppb_text_input_controller_cancel_composition_text(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_text_input_controller_cancel_composition_text(instance);
}

TRACE_WRAPPER
void
trace_ppb_text_input_controller_update_surrounding_text(PP_Instance instance, struct PP_Var text,
                                                        uint32_t caret, uint32_t anchor)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_text_input_controller_update_surrounding_text(instance, text, caret, anchor);
}

const struct PPB_TextInputController_1_0 ppb_text_input_controller_interface_1_0 = {
    .SetTextInputType =         TWRAPZ(ppb_text_input_controller_set_text_input_type),
    .UpdateCaretPosition =      TWRAPZ(ppb_text_input_controller_update_caret_position),
    .CancelCompositionText =    TWRAPZ(ppb_text_input_controller_cancel_composition_text),
    .UpdateSurroundingText =    TWRAPZ(ppb_text_input_controller_update_surrounding_text),
};

static
void
__attribute__((constructor))
constructor_ppb_text_input_controller(void)
{
    register_interface(PPB_TEXTINPUTCONTROLLER_INTERFACE_1_0,
                       &ppb_text_input_controller_interface_1_0);
}
