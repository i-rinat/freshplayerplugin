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

#include "ppb_flash_menu.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_flash_menu_create(PP_Instance instance_id, const struct PP_Flash_Menu *menu_data)
{
    return 0;
}

PP_Bool
ppb_flash_menu_is_flash_menu(PP_Resource resource_id)
{
    return PP_TRUE;
}

int32_t
ppb_flash_menu_show(PP_Resource menu_id, const struct PP_Point *location, int32_t *selected_id,
                    struct PP_CompletionCallback callback)
{
    return -1;
}


// trace wrappers
static
PP_Resource
trace_ppb_flash_menu_create(PP_Instance instance_id, const struct PP_Flash_Menu *menu_data)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_menu_create(instance_id, menu_data);
}

static
PP_Bool
trace_ppb_flash_menu_is_flash_menu(PP_Resource resource_id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_menu_is_flash_menu(resource_id);
}

static
int32_t
trace_ppb_flash_menu_show(PP_Resource menu_id, const struct PP_Point *location,
                          int32_t *selected_id, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_menu_show(menu_id, location, selected_id, callback);
}

const struct PPB_Flash_Menu_0_2 ppb_flash_menu_interface_0_2 = {
    .Create =       trace_ppb_flash_menu_create,
    .IsFlashMenu =  trace_ppb_flash_menu_is_flash_menu,
    .Show =         trace_ppb_flash_menu_show,
};
