#include <ppapi/c/ppb_instance.h>
#include <stddef.h>


const PPB_Instance ppb_instance = {
    .BindGraphics = NULL,
    .IsFullFrame = NULL
};
