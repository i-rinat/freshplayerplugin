/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include "ppb_char_set.h"
#include "ppb_memory.h"
#include <stdlib.h>
#include <locale.h>
#include <langinfo.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"
#include "ppb_var.h"
#include "pp_interface.h"
#include <unicode/ucnv.h>
#include <unicode/ucnv_cb.h>
#include <unicode/ucnv_err.h>
#include <unicode/ustring.h>


char *
ppb_char_set_utf16_to_char_set(PP_Instance instance, const uint16_t *utf16, uint32_t utf16_len,
                               const char *output_char_set,
                               enum PP_CharSet_ConversionError on_error, uint32_t *output_length)
{
    // each character could take up to 4 bytes in UTF-8; with additional zero-terminator byte
    const uint32_t output_buffer_length = (utf16_len + 1) * 4 + 1;
    char *output = ppb_memory_mem_alloc(output_buffer_length);

    if (!output) {
        trace_error("%s, can't allocate memory, %u bytes\n", __func__, output_buffer_length);
        goto err;
    }

    const UChar subst = '?';
    UErrorCode st = U_ZERO_ERROR;
    UConverter *u = ucnv_open(output_char_set, &st);
    if (!U_SUCCESS(st)) {
        trace_error("%s, wrong charset %s\n", __func__, output_char_set);
        goto err;
    }

    switch (on_error) {
    default:
    case PP_CHARSET_CONVERSIONERROR_FAIL:
        st = U_ZERO_ERROR;
        ucnv_setFromUCallBack(u, UCNV_FROM_U_CALLBACK_STOP, NULL, NULL, NULL, &st);
        break;

    case PP_CHARSET_CONVERSIONERROR_SKIP:
        st = U_ZERO_ERROR;
        ucnv_setFromUCallBack(u, UCNV_FROM_U_CALLBACK_SKIP, NULL, NULL, NULL, &st);
        break;

    case PP_CHARSET_CONVERSIONERROR_SUBSTITUTE:
        st = U_ZERO_ERROR;
        ucnv_setFromUCallBack(u, UCNV_FROM_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &st);

        st = U_ZERO_ERROR;
        ucnv_setSubstString(u, &subst, 1, &st);
        break;
    }

    *output_length = ucnv_fromUChars(u, output, output_buffer_length, utf16, utf16_len, &st);

    if (st != U_BUFFER_OVERFLOW_ERROR && !U_SUCCESS(st))
        goto err;

    ucnv_close(u);
    return output;

err:
    *output_length = 0;
    ppb_memory_mem_free(output);
    if (u)
        ucnv_close(u);
    return NULL;
}

uint16_t *
ppb_char_set_char_set_to_utf16(PP_Instance instance, const char *input, uint32_t input_len,
                               const char *input_char_set, enum PP_CharSet_ConversionError on_error,
                               uint32_t *output_length)
{
    // each character could be converted into a surrogate pair
    const uint32_t output_buffer_length = (input_len + 2) * 2 * sizeof(uint16_t);
    uint16_t *output = ppb_memory_mem_alloc(output_buffer_length);

    if (!output) {
        trace_error("%s, can't allocate memory, %u bytes\n", __func__, output_buffer_length);
        goto err;
    }

    const UChar subst = '?';
    UErrorCode st = U_ZERO_ERROR;
    UConverter *u = ucnv_open(input_char_set, &st);
    if (!U_SUCCESS(st)) {
        trace_error("%s, wrong charset %s\n", __func__, input_char_set);
        goto err;
    }

    switch (on_error) {
    default:
    case PP_CHARSET_CONVERSIONERROR_FAIL:
        st = U_ZERO_ERROR;
        ucnv_setToUCallBack(u, UCNV_TO_U_CALLBACK_STOP, NULL, NULL, NULL, &st);
        break;

    case PP_CHARSET_CONVERSIONERROR_SKIP:
        st = U_ZERO_ERROR;
        ucnv_setToUCallBack(u, UCNV_TO_U_CALLBACK_SKIP, NULL, NULL, NULL, &st);
        break;

    case PP_CHARSET_CONVERSIONERROR_SUBSTITUTE:
        st = U_ZERO_ERROR;
        ucnv_setToUCallBack(u, UCNV_TO_U_CALLBACK_SUBSTITUTE, NULL, NULL, NULL, &st);

        st = U_ZERO_ERROR;
        ucnv_setSubstString(u, &subst, 1, &st);
        break;
    }

    st = U_ZERO_ERROR;
    *output_length = ucnv_toUChars(u, output, output_buffer_length / sizeof(uint16_t),
                                   input, input_len, &st);

    if (st != U_BUFFER_OVERFLOW_ERROR && !U_SUCCESS(st))
        goto err;

    ucnv_close(u);
    return output;

err:
    *output_length = 0;
    ppb_memory_mem_free(output);
    if (u)
        ucnv_close(u);
    return NULL;
}

struct PP_Var
ppb_char_set_get_default_char_set(PP_Instance instance)
{
    setlocale(LC_ALL, "");
    return ppb_var_var_from_utf8_z(nl_langinfo(CODESET));
}


// trace wrappers
TRACE_WRAPPER
char *
trace_ppb_char_set_utf16_to_char_set(PP_Instance instance, const uint16_t *utf16,
                                     uint32_t utf16_len, const char *output_char_set,
                                     enum PP_CharSet_ConversionError on_error,
                                     uint32_t *output_length)
{
    trace_info("[PPB] {full} %s instance=%d, utf16=%p, utf16_len=%u, output_char_set=%s, "
               "on_error=%s\n", __func__+6, instance, utf16, utf16_len, output_char_set,
               reverse_char_set_conversion_error(on_error));
    return ppb_char_set_utf16_to_char_set(instance, utf16, utf16_len, output_char_set, on_error,
                                          output_length);
}

TRACE_WRAPPER
uint16_t *
trace_ppb_char_set_char_set_to_utf16(PP_Instance instance, const char *input, uint32_t input_len,
                                     const char *input_char_set,
                                     enum PP_CharSet_ConversionError on_error,
                                     uint32_t *output_length)
{
    trace_info("[PPB] {full} %s instance=%d, input=%.*s, input_len=%u, input_char_set=%s, "
               "on_error=%s\n", __func__+6, instance, input_len, input, input_len, input_char_set,
               reverse_char_set_conversion_error(on_error));
    return ppb_char_set_char_set_to_utf16(instance, input, input_len, input_char_set, on_error,
                                          output_length);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_char_set_get_default_char_set(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_char_set_get_default_char_set(instance);
}


const struct PPB_CharSet_Dev_0_4 ppb_char_set_dev_interface_0_4 = {
    .UTF16ToCharSet =       TWRAPF(ppb_char_set_utf16_to_char_set),
    .CharSetToUTF16 =       TWRAPF(ppb_char_set_char_set_to_utf16),
    .GetDefaultCharSet =    TWRAPF(ppb_char_set_get_default_char_set),
};

static
void
__attribute__((constructor))
constructor_ppb_char_set(void)
{
    register_interface(PPB_CHAR_SET_DEV_INTERFACE_0_4, &ppb_char_set_dev_interface_0_4);
}
