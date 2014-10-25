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

#include "ppb_var_array_buffer.h"
#include <stdlib.h>
#include "trace.h"


struct PP_Var
ppb_var_array_buffer_create(uint32_t size_in_bytes)
{
    return PP_MakeUndefined();
}

PP_Bool
ppb_var_array_buffer_byte_length(struct PP_Var array, uint32_t *byte_length)
{
    return PP_FALSE;
}

void *
ppb_var_array_buffer_map(struct PP_Var array)
{
    return NULL;
}

void
ppb_var_array_buffer_unmap(struct PP_Var array)
{
}


// trace wrappers
TRACE_WRAPPER
struct PP_Var
trace_ppb_var_array_buffer_create(uint32_t size_in_bytes)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_var_array_buffer_create(size_in_bytes);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_var_array_buffer_byte_length(struct PP_Var array, uint32_t *byte_length)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_var_array_buffer_byte_length(array, byte_length);
}

TRACE_WRAPPER
void *
trace_ppb_var_array_buffer_map(struct PP_Var array)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_var_array_buffer_map(array);
}

TRACE_WRAPPER
void
trace_ppb_var_array_buffer_unmap(struct PP_Var array)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_var_array_buffer_unmap(array);
}

const struct PPB_VarArrayBuffer_1_0 ppb_var_array_buffer_interface_1_0 = {
    .Create =       TWRAPZ(ppb_var_array_buffer_create),
    .ByteLength =   TWRAPZ(ppb_var_array_buffer_byte_length),
    .Map =          TWRAPZ(ppb_var_array_buffer_map),
    .Unmap =        TWRAPZ(ppb_var_array_buffer_unmap),
};
