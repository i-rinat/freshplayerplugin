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

static
char *
extract_relevant_part_from_locale_name(const char *locale_name)
{
    char *lang = strdup(locale_name ? locale_name : "en");

    if (strncmp(lang, "zh", 2) == 0) {
        // starts with zh. For Chinese languages country is required too

        // replace "_" by "-". That way "zh_CN.utf8" becomes "zh-CN.utf8"
        char *ptr = strchr(lang, '_');
        if (ptr)
            *ptr = '-';

        // cut at ".". That way "zh-CN.utf8" becomes "zh-CN"
        ptr = strchr(lang, '.');
        if (ptr)
            *ptr = '\0';

        return lang;
    }

    // otherwise, leave only language. Cut at "_". That makes "ru" from "ru_RU.utf8"
    char *ptr = strchr(lang,  '_');
    if (ptr)
        *ptr = '\0';

    return lang;
}

struct PP_Var
ppb_char_set_get_default_char_set(PP_Instance instance)
{
    char *lang = extract_relevant_part_from_locale_name(getenv("LANG"));

    struct lang_encoding {
        const char *lang;
        const char *encoding;
    };

    static const struct lang_encoding pairs[] = {
        {"am", "windows-1252"},
        {"ar", "windows-1256"},
        {"bg", "windows-1251"},
        {"bn", "windows-1252"},
        {"ca", "windows-1252"},
        {"cs", "windows-1250"},
        {"da", "windows-1252"},
        {"de", "windows-1252"},
        {"el", "ISO-8859-7"},
        {"en", "windows-1252"},
        {"es", "windows-1252"},
        {"et", "windows-1257"},
        {"fa", "windows-1256"},
        {"fil", "windows-1252"},
        {"fi", "windows-1252"},
        {"fr", "windows-1252"},
        {"gu", "windows-1252"},
        {"he", "windows-1255"},
        {"hi", "windows-1252"},
        {"hr", "windows-1250"},
        {"hu", "ISO-8859-2"},
        {"id", "windows-1252"},
        {"it", "windows-1252"},
        {"ja", "Shift_JIS"},
        {"kn", "windows-1252"},
        {"ko", "windows-949"},
        {"lt", "windows-1257"},
        {"lv", "windows-1257"},
        {"ml", "windows-1252"},
        {"mr", "windows-1252"},
        {"nb", "windows-1252"},
        {"nl", "windows-1252"},
        {"pl", "ISO-8859-2"},
        {"pt", "windows-1252"},
        {"pt", "windows-1252"},
        {"ro", "ISO-8859-2"},
        {"ru", "windows-1251"},
        {"sk", "windows-1250"},
        {"sl", "ISO-8859-2"},
        {"sr", "windows-1251"},
        {"sv", "windows-1252"},
        {"sw", "windows-1252"},
        {"ta", "windows-1252"},
        {"te", "windows-1252"},
        {"th", "windows-874"},
        {"tr", "ISO-8859-9"},
        {"uk", "windows-1251"},
        {"vi", "windows-1258"},
        {"zh-CN", "GBK"},
        {"zh-TW", "Big5"},
    };

    const char *enc = "windows-1252";
    for (uint32_t k = 0; k < sizeof(pairs)/sizeof(pairs[0]); k ++) {
        if (strcasecmp(pairs[k].lang, lang) == 0) {
            enc = pairs[k].encoding;
            break;
        }
    }

    struct PP_Var ret = ppb_var_var_from_utf8_z(enc);
    free(lang);
    return ret;
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
