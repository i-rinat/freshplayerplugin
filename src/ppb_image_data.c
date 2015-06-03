/*
 * Copyright © 2013-2015  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ppb_image_data.h"
#include "ppb_core.h"
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include "pp_interface.h"


PP_ImageDataFormat
ppb_image_data_get_native_image_data_format(void)
{
    return PP_IMAGEDATAFORMAT_BGRA_PREMUL;
}

PP_Bool
ppb_image_data_is_image_data_format_supported(PP_ImageDataFormat format)
{
    switch (format) {
    case PP_IMAGEDATAFORMAT_BGRA_PREMUL:
        return PP_TRUE;
    case PP_IMAGEDATAFORMAT_RGBA_PREMUL:
        return PP_TRUE;
    default:
        return PP_FALSE;
    }
}

PP_Resource
ppb_image_data_create(PP_Instance instance, PP_ImageDataFormat format,
                      const struct PP_Size *size, PP_Bool init_to_zero)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource image_data = pp_resource_allocate(PP_RESOURCE_IMAGE_DATA, pp_i);
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, failed to create image data resource\n", __func__);
        return 0;
    }

    id->format = format;
    id->width = size->width;
    id->height = size->height;
    id->stride = id->width * 4;

    (void)init_to_zero; // ignore flag, always clear memory
    id->data = calloc(id->stride * id->height, 1);
    if (!id->data) {
        pp_resource_release(image_data);
        ppb_core_release_resource(image_data);
        trace_error("%s, can't allocate memory for image\n", __func__);
        return 0;
    }

    id->cairo_surf = cairo_image_surface_create_for_data((void *)id->data, CAIRO_FORMAT_ARGB32,
                                                         id->width, id->height, id->stride);
    pp_resource_release(image_data);
    return image_data;
}

static
void
ppb_image_data_destroy(void *p)
{
    if (!p)
        return;
    struct pp_image_data_s *id = p;

    if (id->cairo_surf) {
        cairo_surface_destroy(id->cairo_surf);
        id->cairo_surf = NULL;
    }
    free_and_nullify(id->data);
}

PP_Bool
ppb_image_data_is_image_data(PP_Resource image_data)
{
    return pp_resource_get_type(image_data) == PP_RESOURCE_IMAGE_DATA;
}

PP_Bool
ppb_image_data_describe(PP_Resource image_data, struct PP_ImageDataDesc *desc)
{
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    desc->format = id->format;
    desc->size.width = id->width;
    desc->size.height = id->height;
    desc->stride = id->stride;

    pp_resource_release(image_data);
    return PP_TRUE;
}

void *
ppb_image_data_map(PP_Resource image_data)
{
    void *data_ptr;
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, bad resource\n", __func__);
        return NULL;
    }

    data_ptr = id->data;
    pp_resource_release(image_data);
    return data_ptr;
}

void
ppb_image_data_unmap(PP_Resource image_data)
{
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }
    pp_resource_release(image_data);
}


// trace wrappers
TRACE_WRAPPER
PP_ImageDataFormat
trace_ppb_image_data_get_native_image_data_format(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_image_data_get_native_image_data_format();
}

TRACE_WRAPPER
PP_Bool
trace_ppb_image_data_is_image_data_format_supported(PP_ImageDataFormat format)
{
    trace_info("[PPB] {full} %s format=%s\n", __func__+6, reverse_pp_image_data_format(format));
    return ppb_image_data_is_image_data_format_supported(format);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_image_data_create(PP_Instance instance, PP_ImageDataFormat format,
                            const struct PP_Size *size, PP_Bool init_to_zero)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {full} %s instance=%d, format=%s, size=%s, init_to_zero=%d\n", __func__+6,
               instance, reverse_pp_image_data_format(format), s_size, init_to_zero);
    g_free(s_size);
    return ppb_image_data_create(instance, format, size, init_to_zero);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_image_data_is_image_data(PP_Resource image_data)
{
    trace_info("[PPB] {full} %s image_data=%d\n", __func__+6, image_data);
    return ppb_image_data_is_image_data(image_data);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_image_data_describe(PP_Resource image_data, struct PP_ImageDataDesc *desc)
{
    trace_info("[PPB] {full} %s image_data=%d, desc=%p\n", __func__+6, image_data, desc);
    return ppb_image_data_describe(image_data, desc);
}

TRACE_WRAPPER
void *
trace_ppb_image_data_map(PP_Resource image_data)
{
    trace_info("[PPB] {full} %s image_data=%d\n", __func__+6, image_data);
    return ppb_image_data_map(image_data);
}

TRACE_WRAPPER
void
trace_ppb_image_data_unmap(PP_Resource image_data)
{
    trace_info("[PPB] {full} %s image_data=%d\n", __func__+6, image_data);
    ppb_image_data_unmap(image_data);
}


const struct PPB_ImageData_1_0 ppb_image_data_interface_1_0 = {
    .GetNativeImageDataFormat =     TWRAPF(ppb_image_data_get_native_image_data_format),
    .IsImageDataFormatSupported =   TWRAPF(ppb_image_data_is_image_data_format_supported),
    .Create =                       TWRAPF(ppb_image_data_create),
    .IsImageData =                  TWRAPF(ppb_image_data_is_image_data),
    .Describe =                     TWRAPF(ppb_image_data_describe),
    .Map =                          TWRAPF(ppb_image_data_map),
    .Unmap =                        TWRAPF(ppb_image_data_unmap),
};

static
void
__attribute__((constructor))
constructor_ppb_image_data(void)
{
    register_interface(PPB_IMAGEDATA_INTERFACE_1_0, &ppb_image_data_interface_1_0);
    register_resource(PP_RESOURCE_IMAGE_DATA, ppb_image_data_destroy);
}
