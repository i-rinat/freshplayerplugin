#include <ppapi/c/private/ppb_instance_private.h>
#include "trace.h"


struct PP_Var
ppb_instance_private_get_window_object(PP_Instance instance)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                    struct PP_Var *exception)
{
    struct PP_Var var = {0};
    return var;
}

// trace wrappers
static
struct PP_Var
trace_ppb_instance_private_get_window_object(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_instance_private_get_window_object(instance);
}

static
struct PP_Var
trace_ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_instance_private_get_owner_element_object(instance);
}

static
struct PP_Var
trace_ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                          struct PP_Var *exception)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_instance_private_execute_script(instance, script, exception);
}


const struct PPB_Instance_Private_0_1 ppb_instance_private_interface_0_1 = {
    .GetWindowObject =          trace_ppb_instance_private_get_window_object,
    .GetOwnerElementObject =    trace_ppb_instance_private_get_owner_element_object,
    .ExecuteScript =            trace_ppb_instance_private_execute_script,
};
