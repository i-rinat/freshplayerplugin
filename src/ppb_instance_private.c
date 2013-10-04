#include <ppapi/c/private/ppb_instance_private.h>
#include "trace.h"


static
struct PP_Var
ppb_instance_private_get_window_object(PP_Instance instance)
{
    trace_info("{zilch} %s\n", __func__);
    struct PP_Var var = {0};
    return var;
}

static
struct PP_Var
ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    trace_info("{zilch} %s\n", __func__);
    struct PP_Var var = {0};
    return var;
}

static
struct PP_Var
ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                    struct PP_Var *exception)
{
    trace_info("{zilch} %s\n", __func__);
    struct PP_Var var = {0};
    return var;
}


const struct PPB_Instance_Private_0_1 ppb_instance_private_interface_0_1 = {
    .GetWindowObject = ppb_instance_private_get_window_object,
    .GetOwnerElementObject = ppb_instance_private_get_owner_element_object,
    .ExecuteScript = ppb_instance_private_execute_script,
};
