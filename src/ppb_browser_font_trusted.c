#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <stddef.h>


const struct PPB_BrowserFont_Trusted_1_0 ppb_browser_font_trusted_interface_1_0 = {
    .GetFontFamilies = (void *)282,
    .Create = (void *)283,
    .IsFont = (void *)284,
    .Describe = (void *)285,
    .DrawTextAt = (void *)286,
    .MeasureText = (void *)287,
    .CharacterOffsetForPixel = (void *)288,
    .PixelOffsetForCharacter = (void *)289,
};
