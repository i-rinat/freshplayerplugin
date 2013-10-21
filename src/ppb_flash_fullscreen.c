#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <stddef.h>
#include "trace.h"


static
PP_Bool
ppb_flash_fullscreen_is_fullscreen(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
    return PP_TRUE;
}

static
PP_Bool
ppb_flash_fullscreen_set_fullscreen(PP_Instance instance, PP_Bool fullscreen)
{
    trace_info("[PPB] {zilch} %s instance=%d fullscreen=%d\n", __func__, instance, fullscreen);
    return PP_TRUE;
}

static
PP_Bool
ppb_flash_fullscreen_get_screen_size(PP_Instance instance, struct PP_Size* size)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
    return PP_TRUE;
}


const struct PPB_FlashFullscreen_1_0 ppb_flash_fullscreen_interface_1_0 = {
    .IsFullscreen =     ppb_flash_fullscreen_is_fullscreen,
    .SetFullscreen =    ppb_flash_fullscreen_set_fullscreen,
    .GetScreenSize =    ppb_flash_fullscreen_get_screen_size,
};
