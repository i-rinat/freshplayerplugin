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

#ifndef FPP_PPB_VAR_ARRAY_BUFFER_H
#define FPP_PPB_VAR_ARRAY_BUFFER_H

#include <ppapi/c/ppb_var_array_buffer.h>


struct PP_Var
ppb_var_array_buffer_create(uint32_t size_in_bytes);

PP_Bool
ppb_var_array_buffer_byte_length(struct PP_Var array, uint32_t *byte_length);

void *
ppb_var_array_buffer_map(struct PP_Var array);

void
ppb_var_array_buffer_unmap(struct PP_Var array);

#endif // FPP_PPB_VAR_ARRAY_BUFFER_H
