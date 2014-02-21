/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#include <ppapi/c/ppb_graphics_2d.h>
#include <stdlib.h>
#include "trace.h"
#include "pp_resource.h"
#include "interface_list.h"


PP_Resource
ppb_graphics2d_create(PP_Instance instance, const struct PP_Size *size, PP_Bool is_always_opaque)
{
    PP_Resource graphics_2d = pp_resource_allocate(PP_RESOURCE_GRAPHICS2D);
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_warning("%s, can't create graphics2d resource\n", __func__);
        return 0;
    }

    g2d->is_always_opaque = is_always_opaque;
    g2d->width = size->width;
    g2d->height = size->height;
    g2d->stride = 4 * size->width;
    g2d->data = calloc(g2d->stride * g2d->height, 1);
    if (!g2d->data) {
        trace_warning("%s, can't allocate memory\n", __func__);
        pp_resource_release(graphics_2d);
        ppb_core_release_resource(graphics_2d);
        return 0;
    }

    pp_resource_release(graphics_2d);
    return graphics_2d;
}

PP_Bool
ppb_graphics2d_is_graphics2d(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_GRAPHICS2D;
}

PP_Bool
ppb_graphics2d_describe(PP_Resource graphics_2d, struct PP_Size *size, PP_Bool *is_always_opaque)
{
    return PP_TRUE;
}

void
ppb_graphics2d_paint_image_data(PP_Resource graphics_2d, PP_Resource image_data,
                                const struct PP_Point *top_left, const struct PP_Rect *src_rect)
{
}

void
ppb_graphics2d_scroll(PP_Resource graphics_2d, const struct PP_Rect *clip_rect,
                      const struct PP_Point *amount)
{
}

void
ppb_graphics2d_replace_contents(PP_Resource graphics_2d, PP_Resource image_data)
{
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);

    if (g2d->width == id->width && g2d->height == id->height) {
        void *tmp = g2d->data;
        g2d->data = id->data;
        id->data = tmp;
    } else {
        trace_warning("%s, slow path is not implemented\n", __func__);
    }

    pp_resource_release(graphics_2d);
    pp_resource_release(image_data);
}

int32_t
ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    return 0;
}

PP_Bool
ppb_graphics2d_set_scale(PP_Resource resource, float scale)
{
    return PP_TRUE;
}

float
ppb_graphics2d_get_scale(PP_Resource resource)
{
    return 1.0f;
}

// trace wrappers
static
PP_Resource
trace_ppb_graphics2d_create(PP_Instance instance, const struct PP_Size *size,
                            PP_Bool is_always_opaque)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {full} %s instance=%d, size=%s, is_always_opaque=%d\n", __func__+6,
               instance, s_size, is_always_opaque);
    free(s_size);
    return ppb_graphics2d_create(instance, size, is_always_opaque);
}

static
PP_Bool
trace_ppb_graphics2d_is_graphics2d(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_graphics2d_is_graphics2d(resource);
}

static
PP_Bool
trace_ppb_graphics2d_describe(PP_Resource graphics_2d, struct PP_Size *size,
                              PP_Bool *is_always_opaque)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {zilch} %s graphics_2d=%d, size=%s\n", __func__+6, graphics_2d, s_size);
    free(s_size);
    return ppb_graphics2d_describe(graphics_2d, size, is_always_opaque);
}

void
trace_ppb_graphics2d_paint_image_data(PP_Resource graphics_2d, PP_Resource image_data,
                                      const struct PP_Point *top_left,
                                      const struct PP_Rect *src_rect)
{
    char *s_top_left = trace_point_as_string(top_left);
    char *s_src_rect = trace_rect_as_string(src_rect);
    trace_info("[PPB] {zilch} %s graphics_2d=%d, image_data=%d, top_left=%s, src_rect=%s\n",
               __func__+6, graphics_2d, image_data, s_top_left, s_src_rect);
    free(s_top_left);
    free(s_src_rect);
    ppb_graphics2d_paint_image_data(graphics_2d, image_data, top_left, src_rect);
}

static
void
trace_ppb_graphics2d_scroll(PP_Resource graphics_2d, const struct PP_Rect *clip_rect,
                            const struct PP_Point *amount)
{
    char *s_clip_rect = trace_rect_as_string(clip_rect);
    char *s_amount = trace_point_as_string(amount);
    trace_info("[PPB] {zilch} %s graphics_2d=%d, clip_rect=%s, amount=%s\n", __func__+6,
               graphics_2d, s_clip_rect, s_amount);
    free(s_clip_rect);
    free(s_amount);
    ppb_graphics2d_scroll(graphics_2d, clip_rect, amount);
}

static
void
trace_ppb_graphics2d_replace_contents(PP_Resource graphics_2d, PP_Resource image_data)
{
    trace_info("[PPB] {part} %s graphics_2d=%d, image_data=%d\n", __func__+6, graphics_2d,
               image_data);
    ppb_graphics2d_replace_contents(graphics_2d, image_data);
}

static
int32_t
trace_ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s graphics_2d=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, graphics_2d, callback.func, callback.user_data, callback.flags);
    return ppb_graphics2d_flush(graphics_2d, callback);
}

static
PP_Bool
trace_ppb_graphics2d_set_scale(PP_Resource resource, float scale)
{
    trace_info("[PPB] {zilch} %s resource=%d, scale=%f\n", __func__+6, resource, scale);
    return ppb_graphics2d_set_scale(resource, scale);
}

static
float
trace_ppb_graphics2d_get_scale(PP_Resource resource)
{
    return ppb_graphics2d_get_scale(resource);
}


const struct PPB_Graphics2D_1_0 ppb_graphics2d_interface_1_0 = {
    .Create =           trace_ppb_graphics2d_create,
    .IsGraphics2D =     trace_ppb_graphics2d_is_graphics2d,
    .Describe =         trace_ppb_graphics2d_describe,
    .PaintImageData =   trace_ppb_graphics2d_paint_image_data,
    .Scroll =           trace_ppb_graphics2d_scroll,
    .ReplaceContents =  trace_ppb_graphics2d_replace_contents,
    .Flush =            trace_ppb_graphics2d_flush,
};

const struct PPB_Graphics2D_1_1 ppb_graphics2d_interface_1_1 = {
    .Create =           trace_ppb_graphics2d_create,
    .IsGraphics2D =     trace_ppb_graphics2d_is_graphics2d,
    .Describe =         trace_ppb_graphics2d_describe,
    .PaintImageData =   trace_ppb_graphics2d_paint_image_data,
    .Scroll =           trace_ppb_graphics2d_scroll,
    .ReplaceContents =  trace_ppb_graphics2d_replace_contents,
    .Flush =            trace_ppb_graphics2d_flush,
    .SetScale =         trace_ppb_graphics2d_set_scale,
    .GetScale =         trace_ppb_graphics2d_get_scale,
};
