#include <ppapi/c/ppb_instance.h>
#include <stddef.h>
#include "trace.h"

PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    return PP_TRUE;
}

PP_Bool
ppb_instance_is_full_frame(PP_Instance instance)
{
    return PP_TRUE;
}

// trace wrappers
static
PP_Bool
trace_ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    trace_info("[PPB] {zilch} %s instance=%d, device=%d\n", __func__+6, instance, device);
    return ppb_instance_bind_graphics(instance, device);
}

static
PP_Bool
trace_ppb_instance_is_full_frame(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_is_full_frame(instance);
}


const struct PPB_Instance_1_0 ppb_instance_interface_1_0 = {
    .BindGraphics = trace_ppb_instance_bind_graphics,
    .IsFullFrame =  trace_ppb_instance_is_full_frame,
};
