#include <ppapi/c/ppb_graphics_3d.h>
#include <stddef.h>


const struct PPB_Graphics3D_1_0 ppb_graphics3d_interface_1_0 = {
    .GetAttribMaxValue = (void *)226,
    .Create = (void *)227,
    .IsGraphics3D = (void *)228,
    .GetAttribs = (void *)229,
    .SetAttribs = (void *)230,
    .GetError = (void *)231,
    .ResizeBuffers = (void *)232,
    .SwapBuffers = (void *)233,
};
