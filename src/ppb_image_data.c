#include <ppapi/c/ppb_image_data.h>
#include <stddef.h>


const PPB_ImageData ppb_image_data = {
    .GetNativeImageDataFormat = NULL,
    .IsImageDataFormatSupported = NULL,
    .Create = NULL,
    .IsImageData = NULL,
    .Describe = NULL,
    .Map = NULL,
    .Unmap = NULL
};
