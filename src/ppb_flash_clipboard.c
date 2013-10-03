#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <stddef.h>


const struct PPB_Flash_Clipboard_5_0 ppb_flash_clipboard_interface_5_0 = {
    .RegisterCustomFormat = (void *)252,
    .IsFormatAvailable = (void *)253,
    .ReadData = (void *)254,
    .WriteData = (void *)255,
};
