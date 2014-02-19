#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <stddef.h>
#include "trace.h"


PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    return PP_FALSE;
}

PP_Bool
ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    return PP_TRUE;
}

PP_Bool
ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size* size)
{
    return PP_TRUE;
}

// trace wrappers
static
PP_Bool
trace_ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    trace_info("[PPB] {fake} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_is_fullscreen(instance);
}

static
PP_Bool
trace_ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    trace_info("[PPB] {zilch} %s instance=%d fullscreen=%d\n", __func__+6, instance, fullscreen);
    return ppb_flash_fullscreen_set_fullscreen(instance, fullscreen);
}

static
PP_Bool
trace_ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size* size)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_fullscreen_get_screen_size(instance, size);
}


const struct PPB_FlashFullscreen_1_0 ppb_flash_fullscreen_interface_1_0 = {
    .IsFullscreen =     trace_ppb_flash_fullscreen_is_fullscreen,
    .SetFullscreen =    trace_ppb_flash_fullscreen_set_fullscreen,
    .GetScreenSize =    trace_ppb_flash_fullscreen_get_screen_size,
};
