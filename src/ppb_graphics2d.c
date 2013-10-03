#include <ppapi/c/ppb_graphics_2d.h>
#include <stddef.h>


const struct PPB_Graphics2D_1_0 ppb_graphics2d_interface_1_0 = {
    .Create = (void *)234,
    .IsGraphics2D = (void *)235,
    .Describe = (void *)236,
    .PaintImageData = (void *)237,
    .Scroll = (void *)238,
    .ReplaceContents = (void *)239,
    .Flush = (void *)240,
};
