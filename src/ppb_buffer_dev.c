#include <ppapi/c/dev/ppb_buffer_dev.h>
#include <stddef.h>


const PPB_Buffer_Dev ppb_buffer_dev = {
    .Create = NULL,
    .IsBuffer = NULL,
    .Describe = NULL,
    .Map = NULL,
    .Unmap = NULL
};
