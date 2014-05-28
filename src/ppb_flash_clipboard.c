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

#include "ppb_flash_clipboard.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "reverse_constant.h"


uint32_t
ppb_flash_clipboard_register_custom_format(PP_Instance instance_id, const char *format_name)
{
    return 0;
}

PP_Bool
ppb_flash_clipboard_is_format_available(PP_Instance instance_id,
                                        PP_Flash_Clipboard_Type clipboard_type, uint32_t format)
{
    return PP_FALSE;
}

struct PP_Var
ppb_flash_clipboard_read_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                              uint32_t format)
{
    return PP_MakeUndefined();
}

int32_t
ppb_flash_clipboard_write_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                               uint32_t data_item_count, const uint32_t formats[],
                               const struct PP_Var data_items[])
{
    return -1;
}


#ifndef NDEBUG
// trace wrappers
static
uint32_t
trace_ppb_flash_clipboard_register_custom_format(PP_Instance instance_id, const char *format_name)
{
    trace_info("[PPB] {zilch} %s instance_id=%d, format_name=%s\n", __func__+6, instance_id,
               format_name);
    return ppb_flash_clipboard_register_custom_format(instance_id, format_name);
}

static
PP_Bool
trace_ppb_flash_clipboard_is_format_available(PP_Instance instance_id,
                                              PP_Flash_Clipboard_Type clipboard_type,
                                              uint32_t format)
{
    trace_info("[PPB] {zilch} %s instance_id=%d, clipboard_type=%s, format=%s(%u)\n", __func__+6,
               instance_id, reverse_clipboard_type(clipboard_type),
               reverse_clipboard_format(format), format);
    return ppb_flash_clipboard_is_format_available(instance_id, clipboard_type, format);
}

static
struct PP_Var
trace_ppb_flash_clipboard_read_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                                    uint32_t format)
{
    trace_info("[PPB] {zilch} %s instance_id=%d, clipboard_type=%s, format=%s(%u)\n", __func__+6,
               instance_id, reverse_clipboard_type(clipboard_type),
               reverse_clipboard_format(format), format);
    return ppb_flash_clipboard_read_data(instance_id, clipboard_type, format);
}

static
int32_t
trace_ppb_flash_clipboard_write_data(PP_Instance instance_id,
                                     PP_Flash_Clipboard_Type clipboard_type,
                                     uint32_t data_item_count, const uint32_t formats[],
                                     const struct PP_Var data_items[])
{
    trace_info("[PPB] {zilch} %s instance_id=%d, clipboard_type=%s, data_item_count=%u, "
               "formats[]=%p, data_items[]=%p\n", __func__+6, instance_id,
               reverse_clipboard_type(clipboard_type), data_item_count, formats, data_items);
    return ppb_flash_clipboard_write_data(instance_id, clipboard_type, data_item_count, formats,
                                          data_items);
}
#endif // NDEBUG


const struct PPB_Flash_Clipboard_5_0 ppb_flash_clipboard_interface_5_0 = {
    .RegisterCustomFormat = TWRAP(ppb_flash_clipboard_register_custom_format),
    .IsFormatAvailable =    TWRAP(ppb_flash_clipboard_is_format_available),
    .ReadData =             TWRAP(ppb_flash_clipboard_read_data),
    .WriteData =            TWRAP(ppb_flash_clipboard_write_data),
};
