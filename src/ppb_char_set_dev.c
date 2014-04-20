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

#include "ppb_char_set_dev.h"
#include <stdlib.h>
#include <locale.h>
#include <langinfo.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"


char *
ppb_char_set_dev_utf16_to_char_set(PP_Instance instance, const uint16_t *utf16, uint32_t utf16_len,
                                   const char *output_char_set,
                                   enum PP_CharSet_ConversionError on_error,
                                   uint32_t *output_length)
{
    return NULL;
}

uint16_t *
ppb_char_set_dev_char_set_to_utf16(PP_Instance instance, const char *input, uint32_t input_len,
                                   const char *input_char_set,
                                   enum PP_CharSet_ConversionError on_error,
                                   uint32_t *output_length)
{
    return NULL;
}

struct PP_Var
ppb_char_set_dev_get_default_char_set(PP_Instance instance)
{
    setlocale(LC_ALL, NULL);
    return PP_MakeString(nl_langinfo(CODESET));
}


// trace wrappers
static
char *
trace_ppb_char_set_dev_utf16_to_char_set(PP_Instance instance, const uint16_t *utf16,
                                         uint32_t utf16_len, const char *output_char_set,
                                         enum PP_CharSet_ConversionError on_error,
                                         uint32_t *output_length)
{
    trace_info("[PPB] {zilch} %s instance=%d, utf16=%p, utf16_len=%u, output_char_set=%s, "
               "on_error=%s\n", __func__+6, instance, utf16, utf16_len, output_char_set,
               reverse_char_set_conversion_error(on_error));
    return ppb_char_set_dev_utf16_to_char_set(instance, utf16, utf16_len, output_char_set,
                                              on_error, output_length);
}

static
uint16_t *
trace_ppb_char_set_dev_char_set_to_utf16(PP_Instance instance, const char *input,
                                         uint32_t input_len, const char *input_char_set,
                                         enum PP_CharSet_ConversionError on_error,
                                         uint32_t *output_length)
{
    trace_info("[PPB] {zilch} %s instance=%d, input=%.*s, input_len=%u, input_char_set=%s, "
               "on_error=%s\n", __func__+6, instance, input_len, input, input_len, input_char_set,
               reverse_char_set_conversion_error(on_error));
    return ppb_char_set_dev_char_set_to_utf16(instance, input, input_len, input_char_set,
                                              on_error, output_length);
}

static
struct PP_Var
trace_ppb_char_set_dev_get_default_char_set(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_char_set_dev_get_default_char_set(instance);
}

const struct PPB_CharSet_Dev_0_4 ppb_char_set_dev_interface_0_4 = {
    .UTF16ToCharSet =       trace_ppb_char_set_dev_utf16_to_char_set,
    .CharSetToUTF16 =       trace_ppb_char_set_dev_char_set_to_utf16,
    .GetDefaultCharSet =    trace_ppb_char_set_dev_get_default_char_set,
};
