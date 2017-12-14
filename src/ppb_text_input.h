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

#include <ppapi/c/dev/ppb_text_input_dev.h>

void
ppb_text_input_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type);

void
ppb_text_input_interface_update_caret_position(PP_Instance instance, const struct PP_Rect *caret,
                                               const struct PP_Rect *bounding_box);

void
ppb_text_input_interface_cancel_composition_text(PP_Instance instance);

void
ppb_text_input_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                 uint32_t caret, uint32_t anchor);

void
ppb_text_input_interface_selection_changed(PP_Instance instance);
