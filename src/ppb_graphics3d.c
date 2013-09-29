#include <ppapi/c/ppb_graphics_3d.h>
#include <stddef.h>


const PPB_Graphics3D ppb_graphics3d = {
    .GetAttribMaxValue = NULL,
    .Create = NULL,
    .IsGraphics3D = NULL,
    .GetAttribs = NULL,
    .SetAttribs = NULL,
    .GetError = NULL,
    .ResizeBuffers = NULL,
    .SwapBuffers = NULL
};
