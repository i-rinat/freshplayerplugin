#include <ppapi/c/ppb_image_data.h>
#include <stddef.h>


const struct PPB_ImageData_1_0 ppb_image_data_interface_1_0 = {
    .GetNativeImageDataFormat = (void *)219,
    .IsImageDataFormatSupported = (void *)220,
    .Create = (void *)221,
    .IsImageData = (void *)222,
    .Describe = (void *)223,
    .Map = (void *)224,
    .Unmap = (void *)225,
};
