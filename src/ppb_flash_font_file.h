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

#ifndef FPP_PPB_FLASH_FONT_FILE_H
#define FPP_PPB_FLASH_FONT_FILE_H

#include <ppapi/c/private/ppb_flash_font_file.h>

PP_Resource
ppb_flash_font_file_create(PP_Instance instance,
                           const struct PP_BrowserFont_Trusted_Description *description,
                           PP_PrivateFontCharset charset);

PP_Bool
ppb_flash_font_file_is_flash_font_file(PP_Resource resource);

PP_Bool
ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                   uint32_t *output_length);

PP_Bool
ppb_flash_font_file_is_supported_for_windows(void);

#endif // FPP_PPB_FLASH_FONT_FILE_H
