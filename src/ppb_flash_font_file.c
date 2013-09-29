#include <ppapi/c/private/ppb_flash_font_file.h>
#include <stddef.h>


const PPB_Flash_FontFile ppb_flash_font_file = {
    .Create = NULL,
    .IsFlashFontFile = NULL,
    .GetFontTable = NULL
};
