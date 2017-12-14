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

#include <ppapi/c/private/ppb_flash_clipboard.h>

uint32_t
ppb_flash_clipboard_register_custom_format(PP_Instance instance_id, const char *format_name);

PP_Bool
ppb_flash_clipboard_is_format_available(PP_Instance instance_id,
                                        PP_Flash_Clipboard_Type clipboard_type, uint32_t format);

struct PP_Var
ppb_flash_clipboard_read_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                              uint32_t format);

int32_t
ppb_flash_clipboard_write_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                               uint32_t data_item_count, const uint32_t formats[],
                               const struct PP_Var data_items[]);

PP_Bool
ppb_flash_clipboard_get_sequence_number(PP_Instance instance_id,
                                        PP_Flash_Clipboard_Type clipboard_type,
                                        uint64_t *sequence_number);
