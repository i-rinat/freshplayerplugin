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

#include "ppb_flash_font_file.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_flash_font_file_create(PP_Instance instance,
                           const struct PP_BrowserFont_Trusted_Description *description,
                           PP_PrivateFontCharset charset)
{
    return 0;
}

PP_Bool
ppb_flash_font_file_is_flash_font_file(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                   uint32_t *output_length)
{
    return PP_FALSE;
}


// trace wrappers
static
PP_Resource
trace_ppb_flash_font_file_create(PP_Instance instance,
                                 const struct PP_BrowserFont_Trusted_Description *description,
                                 PP_PrivateFontCharset charset)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_font_file_create(instance, description, charset);
}

static
PP_Bool
trace_ppb_flash_font_file_is_flash_font_file(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_font_file_is_flash_font_file(resource);
}

static
PP_Bool
trace_ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                         uint32_t *output_length)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_font_file_get_font_table(font_file, table, output, output_length);
}


const struct PPB_Flash_FontFile_0_1 ppb_flash_font_file_interface_0_1 = {
    .Create =           trace_ppb_flash_font_file_create,
    .IsFlashFontFile =  trace_ppb_flash_font_file_is_flash_font_file,
    .GetFontTable =     trace_ppb_flash_font_file_get_font_table,
};
