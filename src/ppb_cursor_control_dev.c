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

#include "ppb_cursor_control_dev.h"
#include <stdlib.h>
#include "trace.h"


PP_Bool
ppb_cursor_control_dev_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                                  PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_unlock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_has_cursor_lock(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_can_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}


// trace wrappers
static
PP_Bool
trace_ppb_cursor_control_dev_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                                        PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    char *s_hot_spot = trace_point_as_string(hot_spot);
    trace_info("[PPB] {zilch} %s instance=%d, type=%d, custom_image=%d, hot_spot=%s\n", __func__+6,
               instance, type, custom_image, s_hot_spot);
    free(s_hot_spot);
    return ppb_cursor_control_dev_set_cursor(instance, type, custom_image, hot_spot);
}

static
PP_Bool
trace_ppb_cursor_control_dev_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_lock_cursor(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_unlock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_unlock_cursor(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_has_cursor_lock(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_has_cursor_lock(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_can_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_can_lock_cursor(instance);
}

const struct PPB_CursorControl_Dev_0_4 ppb_cursor_control_dev_interface_0_4 = {
    .SetCursor =        trace_ppb_cursor_control_dev_set_cursor,
    .LockCursor =       trace_ppb_cursor_control_dev_lock_cursor,
    .UnlockCursor =     trace_ppb_cursor_control_dev_unlock_cursor,
    .HasCursorLock =    trace_ppb_cursor_control_dev_has_cursor_lock,
    .CanLockCursor =    trace_ppb_cursor_control_dev_can_lock_cursor,
};
