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
#include <iconv.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"
#include "ppb_var.h"
#include "pp_interface.h"


static
const char *
get_supported_charset_alias(const char *charset)
{
    if (strcasecmp(charset, "gb2312-80") == 0) {
        return "gb2312";
    } else {
        return charset;
    }
}

char *
ppb_char_set_utf16_to_char_set(PP_Instance instance, const uint16_t *utf16, uint32_t utf16_len,
                               const char *output_char_set,
                               enum PP_CharSet_ConversionError on_error, uint32_t *output_length)
{
    const uint32_t output_buffer_length = (utf16_len + 1) * 4 + 1;
    char *output = ppb_memory_mem_alloc(output_buffer_length);
    char *inbuf = (char*)utf16;
    char *outbuf = (char*)output;
    size_t inbytesleft = utf16_len * 2;
    size_t outbytesleft = output_buffer_length - 1;
    iconv_t cd;
    char *tmp;

    output_char_set = get_supported_charset_alias(output_char_set);

    switch (on_error) {
    default:
    case PP_CHARSET_CONVERSIONERROR_FAIL:
        cd = iconv_open(output_char_set, "UTF16LE");
        break;
    case PP_CHARSET_CONVERSIONERROR_SKIP:
        tmp = g_strdup_printf("%s//IGNORE", output_char_set);
        cd = iconv_open(tmp, "UTF16LE");
        g_free(tmp);
        break;
    case PP_CHARSET_CONVERSIONERROR_SUBSTITUTE:
        tmp = g_strdup_printf("%s//TRANSLIT", output_char_set);
        cd = iconv_open(tmp, "UTF16LE");
        g_free(tmp);
        break;
    }

    if (cd == (iconv_t)-1) {
        trace_error("%s, wrong charset %s\n", __func__, output_char_set);
        memcpy(output, utf16, inbytesleft);
        *output_length = inbytesleft;
        return output;
    }

    size_t ret = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    if (ret == (size_t) -1) {
        if (errno == E2BIG) {
            trace_warning("%s, this should never happen\n", __func__);
        } else {
            if (on_error == PP_CHARSET_CONVERSIONERROR_FAIL) {
                ppb_memory_mem_free(output);
                *output_length = 0;
                iconv_close(cd);
                return NULL;
            }
        }
    }

    *output_length = output_buffer_length - 1 - outbytesleft;
    output[*output_length] = 0;
    iconv_close(cd);
    return output;
}

uint16_t *
ppb_char_set_char_set_to_utf16(PP_Instance instance, const char *input, uint32_t input_len,
                               const char *input_char_set, enum PP_CharSet_ConversionError on_error,
                               uint32_t *output_length)
{
    const uint32_t output_buffer_length = (input_len + 2) * sizeof(uint16_t);
    uint16_t *output = ppb_memory_mem_alloc(output_buffer_length);
    char *inbuf = (char *)input;
    char *outbuf = (char *)output;
    size_t inbytesleft = input_len;
    size_t outbytesleft = output_buffer_length - 2;
    iconv_t cd;

    input_char_set = get_supported_charset_alias(input_char_set);

    switch (on_error) {
    default:
    case PP_CHARSET_CONVERSIONERROR_FAIL:
        cd = iconv_open("UTF16LE", input_char_set);
        break;
    case PP_CHARSET_CONVERSIONERROR_SKIP:
        cd = iconv_open("UTF16LE//IGNORE", input_char_set);
        break;
    case PP_CHARSET_CONVERSIONERROR_SUBSTITUTE:
        cd = iconv_open("UTF16LE//TRANSLIT", input_char_set);
        break;
    }

    if (cd == (iconv_t)-1) {
        trace_error("%s, wrong charset %s\n", __func__, input_char_set);
        memcpy(output, input, inbytesleft);
        *output_length = inbytesleft / 2;
        return output;
    }

    size_t ret = iconv(cd, &inbuf, &inbytesleft, &outbuf, &outbytesleft);
    if (ret == (size_t) -1) {
        if (errno == E2BIG) {
            trace_warning("%s, this should never happen\n", __func__);
        } else {
            if (on_error == PP_CHARSET_CONVERSIONERROR_FAIL) {
                ppb_memory_mem_free(output);
                *output_length = 0;
                iconv_close(cd);
                return NULL;
            }
        }
    }

    *output_length = (output_buffer_length - 2 - outbytesleft) / sizeof(uint16_t);
    output[*output_length] = 0;
    iconv_close(cd);
    return output;
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
