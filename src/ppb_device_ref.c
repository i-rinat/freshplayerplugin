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

#include "ppb_device_ref.h"
#include <stdlib.h>
#include "trace.h"


PP_Bool
ppb_device_ref_is_device_ref(PP_Resource resource)
{
    return PP_TRUE;
}

PP_DeviceType_Dev
ppb_device_ref_get_type(PP_Resource device_ref)
{
    return 0;
}

struct PP_Var
ppb_device_ref_get_name(PP_Resource device_ref)
{
    return PP_MakeUndefined();
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_device_ref_is_device_ref(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_device_ref_is_device_ref(resource);
}

TRACE_WRAPPER
PP_DeviceType_Dev
trace_ppb_device_ref_get_type(PP_Resource device_ref)
{
    trace_info("[PPB] {zilch} %s device_ref=%d\n", __func__+6, device_ref);
    return ppb_device_ref_get_type(device_ref);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_device_ref_get_name(PP_Resource device_ref)
{
    trace_info("[PPB] {zilch} %s device_ref=%d\n", __func__+6, device_ref);
    return ppb_device_ref_get_name(device_ref);
}

const struct PPB_DeviceRef_Dev_0_1 ppb_device_ref_dev_interface_0_1 = {
    .IsDeviceRef =  TWRAPZ(ppb_device_ref_is_device_ref),
    .GetType =      TWRAPZ(ppb_device_ref_get_type),
    .GetName =      TWRAPZ(ppb_device_ref_get_name),
};
