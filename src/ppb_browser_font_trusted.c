#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <stddef.h>


const PPB_BrowserFont_Trusted ppb_browser_font_trusted = {
    .GetFontFamilies = NULL,
    .Create = NULL,
    .IsFont = NULL,
    .Describe = NULL,
    .DrawTextAt = NULL,
    .MeasureText = NULL,
    .CharacterOffsetForPixel = NULL,
    .PixelOffsetForCharacter = NULL
};
