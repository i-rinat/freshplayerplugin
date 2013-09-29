#include <ppapi/c/private/ppb_flash.h>
#include <stddef.h>


const PPB_Flash ppb_flash = {
    .SetInstanceAlwaysOnTop = NULL,
    .DrawGlyphs = NULL,
    .GetProxyForURL = NULL,
    .Navigate = NULL,
    .GetLocalTimeZoneOffset = NULL,
    .GetCommandLineArgs = NULL,
    .PreloadFontWin = NULL,
    .IsRectTopmost = NULL,
    .UpdateActivity = NULL,
    .GetSetting = NULL,
    .SetCrashData = NULL,
    .EnumerateVideoCaptureDevices = NULL
};
