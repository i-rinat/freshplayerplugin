#include <ppapi/c/ppb_instance.h>
#include <stddef.h>
#include "trace.h"

static
PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    trace_info("[PPB] {zilch} %s instance=%d, device=%d\n", __func__, instance, device);
    return PP_TRUE;
}

static
PP_Bool
ppb_instance_is_full_frame(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
    return PP_TRUE;
}


const struct PPB_Instance_1_0 ppb_instance_interface_1_0 = {
    .BindGraphics = ppb_instance_bind_graphics,
    .IsFullFrame = ppb_instance_is_full_frame,
};
