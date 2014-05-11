#include "ppb_flash_message_loop.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "trace.h"
#include "pp_resource.h"



PP_Resource
ppb_flash_message_loop_create(PP_Instance instance)
{
    return 0;
}

PP_Bool
ppb_flash_message_loop_is_flash_message_loop(PP_Resource resource)
{
    return PP_TRUE;
}

int32_t
ppb_flash_message_loop_run(PP_Resource flash_message_loop)
{
    return 0;
}

void
ppb_flash_message_loop_quit(PP_Resource flash_message_loop)
{
}


// trace wrappers
static
PP_Resource
trace_ppb_flash_message_loop_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_message_loop_create(instance);
}

static
PP_Bool
trace_ppb_flash_message_loop_is_flash_message_loop(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_flash_message_loop_is_flash_message_loop(resource);
}

static
int32_t
trace_ppb_flash_message_loop_run(PP_Resource flash_message_loop)
{
    trace_info("[PPB] {zilch} %s flash_message_loop=%d\n", __func__+6, flash_message_loop);
    return ppb_flash_message_loop_run(flash_message_loop);
}

static
void
trace_ppb_flash_message_loop_quit(PP_Resource flash_message_loop)
{
    trace_info("[PPB] {zilch} %s flash_message_loop=%d\n", __func__+6, flash_message_loop);
    return ppb_flash_message_loop_quit(flash_message_loop);
}

const struct PPB_Flash_MessageLoop_0_1 ppb_flash_message_loop_interface_0_1 = {
    .Create =               trace_ppb_flash_message_loop_create,
    .IsFlashMessageLoop =   trace_ppb_flash_message_loop_is_flash_message_loop,
    .Run =                  trace_ppb_flash_message_loop_run,
    .Quit =                 trace_ppb_flash_message_loop_quit,
};
