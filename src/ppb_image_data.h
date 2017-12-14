/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#pragma once

#include "pp_resource.h"
#include <cairo.h>
#include <ppapi/c/ppb_image_data.h>

struct pp_image_data_s {
    COMMON_STRUCTURE_FIELDS
    int32_t             width;
    int32_t             height;
    int32_t             stride;
    char               *data;
    PP_ImageDataFormat  format;
    cairo_surface_t    *cairo_surf;
};

PP_ImageDataFormat
ppb_image_data_get_native_image_data_format(void);

PP_Bool
ppb_image_data_is_image_data_format_supported(PP_ImageDataFormat format);

PP_Resource
ppb_image_data_create(PP_Instance instance, PP_ImageDataFormat format,
                      const struct PP_Size *size, PP_Bool init_to_zero);

PP_Bool
ppb_image_data_is_image_data(PP_Resource image_data);

PP_Bool
ppb_image_data_describe(PP_Resource image_data, struct PP_ImageDataDesc *desc);

void *
ppb_image_data_map(PP_Resource image_data);

void
ppb_image_data_unmap(PP_Resource image_data);
