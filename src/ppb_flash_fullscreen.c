#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <stddef.h>


const PPB_FlashFullscreen ppb_flash_fullscreen = {
    .IsFullscreen = NULL,
    .SetFullscreen = NULL,
    .GetScreenSize = NULL
};
