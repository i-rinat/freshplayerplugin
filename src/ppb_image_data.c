#include <ppapi/c/ppb_image_data.h>
#include <stddef.h>
#include "trace.h"
#include "pp_resource.h"
#include "reverse_constant.h"

PP_ImageDataFormat
ppb_image_data_get_native_image_data_format(void)
{
    return 0;
}

PP_Bool
ppb_image_data_is_image_data_format_supported(PP_ImageDataFormat format)
{
    return PP_TRUE;
}

PP_Resource
ppb_image_data_create(PP_Instance instance, PP_ImageDataFormat format,
                      const struct PP_Size* size, PP_Bool init_to_zero)
{
    PP_Resource image_data = pp_resource_allocate(PP_RESOURCE_IMAGE_DATA);

    // TODO: fill

    return image_data;
}

PP_Bool
ppb_image_data_is_image_data(PP_Resource image_data)
{
    return pp_resource_get_type(image_data) == PP_RESOURCE_IMAGE_DATA;
}

PP_Bool
ppb_image_data_describe(PP_Resource image_data, struct PP_ImageDataDesc* desc)
{
    return PP_TRUE;
}

void *
ppb_image_data_map(PP_Resource image_data)
{
    return (void*)0xdeadbeef;
}

void
ppb_image_data_unmap(PP_Resource image_data)
{
}

// trace wrappers
static
PP_ImageDataFormat
trace_ppb_image_data_get_native_image_data_format(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_image_data_get_native_image_data_format();
}

static
PP_Bool
trace_ppb_image_data_is_image_data_format_supported(PP_ImageDataFormat format)
{
    trace_info("[PPB] {zilch} %s format=%s\n", __func__+6, reverse_pp_image_data_format(format));
    return ppb_image_data_is_image_data_format_supported(format);
}

static
PP_Resource
trace_ppb_image_data_create(PP_Instance instance, PP_ImageDataFormat format,
                            const struct PP_Size* size, PP_Bool init_to_zero)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {part} %s instance=%d, format=%s, size=%s, init_to_zero=%d\n", __func__+6,
               instance, reverse_pp_image_data_format(format), s_size, init_to_zero);
    free(s_size);
    return ppb_image_data_create(instance, format, size, init_to_zero);
}

static
PP_Bool
trace_ppb_image_data_is_image_data(PP_Resource image_data)
{
    trace_info("[PPB] {full} %s image_data=%d\n", __func__+6, image_data);
    return ppb_image_data_is_image_data(image_data);
}

static
PP_Bool
trace_ppb_image_data_describe(PP_Resource image_data, struct PP_ImageDataDesc* desc)
{
    trace_info("[PPB] {zilch} %s image_data=%d, desc=%p\n", __func__+6, image_data, desc);
    return ppb_image_data_describe(image_data, desc);
}

static
void *
trace_ppb_image_data_map(PP_Resource image_data)
{
    trace_info("[PPB] {zilch} %s image_data=%d\n", __func__+6, image_data);
    return ppb_image_data_map(image_data);
}

static
void
trace_ppb_image_data_unmap(PP_Resource image_data)
{
    trace_info("[PPB] {zilch} %s image_data=%d\n", __func__+6, image_data);
    ppb_image_data_unmap(image_data);
}


const struct PPB_ImageData_1_0 ppb_image_data_interface_1_0 = {
    .GetNativeImageDataFormat = trace_ppb_image_data_get_native_image_data_format,
    .IsImageDataFormatSupported = trace_ppb_image_data_is_image_data_format_supported,
    .Create =                   trace_ppb_image_data_create,
    .IsImageData =              trace_ppb_image_data_is_image_data,
    .Describe =                 trace_ppb_image_data_describe,
    .Map =                      trace_ppb_image_data_map,
    .Unmap =                    trace_ppb_image_data_unmap,
};
