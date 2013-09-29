#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <stddef.h>


const PPB_Flash_Clipboard ppb_flash_clipboard = {
    .RegisterCustomFormat = NULL,
    .IsFormatAvailable = NULL,
    .ReadData = NULL,
    .WriteData = NULL
};
