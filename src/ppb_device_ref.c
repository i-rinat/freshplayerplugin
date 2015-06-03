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
#include "ppb_var.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_interface.h"


PP_Resource
ppb_device_ref_create(PP_Instance instance, struct PP_Var name, struct PP_Var longname,
                      PP_DeviceType_Dev type)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource device_ref = pp_resource_allocate(PP_RESOURCE_DEVICE_REF, pp_i);
    struct pp_device_ref_s *dr = pp_resource_acquire(device_ref, PP_RESOURCE_DEVICE_REF);
    if (!dr) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    // no type checking is perfomed as it's an internal function
    dr->name = ppb_var_add_ref2(name);
    dr->longname = ppb_var_add_ref2(longname);
    dr->type = type;

    pp_resource_release(device_ref);
    return device_ref;
}

static
void
ppb_device_ref_destroy(void *ptr)
{
    struct pp_device_ref_s *dr = ptr;

    ppb_var_release(dr->name);
    ppb_var_release(dr->longname);
}

PP_Bool
ppb_device_ref_is_device_ref(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_DEVICE_REF;
}

PP_DeviceType_Dev
ppb_device_ref_get_type(PP_Resource device_ref)
{
    struct pp_device_ref_s *dr = pp_resource_acquire(device_ref, PP_RESOURCE_DEVICE_REF);
    if (!dr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_DEVICETYPE_DEV_INVALID;
    }

    PP_DeviceType_Dev type = dr->type;

    pp_resource_release(device_ref);
    return type;
}

struct PP_Var
ppb_device_ref_get_name(PP_Resource device_ref)
{
    struct pp_device_ref_s *dr = pp_resource_acquire(device_ref, PP_RESOURCE_DEVICE_REF);
    if (!dr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var name = ppb_var_add_ref2(dr->name);

    pp_resource_release(device_ref);
    return name;
}

struct PP_Var
ppb_device_ref_get_longname(PP_Resource device_ref)
{
    struct pp_device_ref_s *dr = pp_resource_acquire(device_ref, PP_RESOURCE_DEVICE_REF);
    if (!dr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var name = ppb_var_add_ref2(dr->longname);

    pp_resource_release(device_ref);
    return name;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_device_ref_is_device_ref(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_device_ref_is_device_ref(resource);
}

TRACE_WRAPPER
PP_DeviceType_Dev
trace_ppb_device_ref_get_type(PP_Resource device_ref)
{
    trace_info("[PPB] {full} %s device_ref=%d\n", __func__+6, device_ref);
    return ppb_device_ref_get_type(device_ref);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_device_ref_get_name(PP_Resource device_ref)
{
    trace_info("[PPB] {full} %s device_ref=%d\n", __func__+6, device_ref);
    return ppb_device_ref_get_name(device_ref);
}

const struct PPB_DeviceRef_Dev_0_1 ppb_device_ref_dev_interface_0_1 = {
    .IsDeviceRef =  TWRAPF(ppb_device_ref_is_device_ref),
    .GetType =      TWRAPF(ppb_device_ref_get_type),
    .GetName =      TWRAPF(ppb_device_ref_get_name),
};

static
void
__attribute__((constructor))
constructor_ppb_device_ref(void)
{
    register_interface(PPB_DEVICEREF_DEV_INTERFACE_0_1, &ppb_device_ref_dev_interface_0_1);
    register_resource(PP_RESOURCE_DEVICE_REF, ppb_device_ref_destroy);
}
