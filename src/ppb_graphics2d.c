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

#include "ppb_graphics2d.h"
#include "ppb_core.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"


struct g2d_paint_task_s {
    enum g2d_paint_task_type_e {
        gpt_paint_id,
        gpt_replace_contents,
    } type;
    PP_Resource     image_data;
    struct PP_Point ofs;
    struct PP_Rect  src;
    int             src_is_set;
};

PP_Resource
ppb_graphics2d_create(PP_Instance instance, const struct PP_Size *size, PP_Bool is_always_opaque)
{
    PP_Resource graphics_2d = pp_resource_allocate(PP_RESOURCE_GRAPHICS2D, instance);
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
    g2d->second_buffer = calloc(g2d->stride * g2d->height, 1);
    if (!g2d->data || !g2d->second_buffer) {
        trace_warning("%s, can't allocate memory\n", __func__);
        free_and_nullify(g2d, data);
        free_and_nullify(g2d, second_buffer);
        pp_resource_release(graphics_2d);
        ppb_core_release_resource(graphics_2d);
        return 0;
    }
    g2d->cairo_surf = cairo_image_surface_create_for_data((unsigned char *)g2d->data,
                            CAIRO_FORMAT_ARGB32, g2d->width, g2d->height, g2d->stride);
    g2d->task_list = NULL;
    pthread_mutex_init(&g2d->lock, NULL);

    pp_resource_release(graphics_2d);
    return graphics_2d;
}

void
ppb_graphics2d_destroy(void *p)
{
    if (!p)
        return;
    struct pp_graphics2d_s *g2d = p;
    free_and_nullify(g2d, data);
    free_and_nullify(g2d, second_buffer);
    if (g2d->cairo_surf) {
        cairo_surface_destroy(g2d->cairo_surf);
        g2d->cairo_surf = NULL;
    }
    pthread_mutex_destroy(&g2d->lock);
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
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    if (!g2d)
        return;
    struct g2d_paint_task_s *pt = malloc(sizeof(*pt));

    pt->type = gpt_paint_id;
    pp_resource_ref(image_data);
    pt->image_data = image_data;
    pt->src_is_set = !!src_rect;

    if (top_left) {
        memcpy(&pt->ofs, top_left, sizeof(*top_left));
    } else {
        pt->ofs.x = pt->ofs.y = 0;
    }
    if (src_rect)
        memcpy(&pt->src, src_rect, sizeof(*src_rect));

    g2d->task_list = g_list_append(g2d->task_list, pt);
    pp_resource_release(graphics_2d);
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
    if (!g2d)
        return;

    struct g2d_paint_task_s *pt = malloc(sizeof(*pt));

    pt->type = gpt_replace_contents;
    pp_resource_ref(image_data);
    pt->image_data = image_data;
    g2d->task_list = g_list_append(g2d->task_list, pt);

    pp_resource_release(graphics_2d);
}

int32_t
ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_warning("%s, wrong graphics_2d\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct pp_instance_s *pp_i = tables_get_pp_instance(g2d->instance);

    while (g2d->task_list) {
        GList *link = g_list_first(g2d->task_list);
        struct g2d_paint_task_s *pt = link->data;
        struct pp_image_data_s  *id;

        cairo_t *cr;

        g2d->task_list = g_list_delete_link(g2d->task_list, link);
        switch (pt->type) {
        case gpt_paint_id:
            id = pp_resource_acquire(pt->image_data, PP_RESOURCE_IMAGE_DATA);
            if (!id)
                break;

            cairo_surface_mark_dirty(g2d->cairo_surf);
            cr = cairo_create(g2d->cairo_surf);
            cairo_set_source_surface(cr, id->cairo_surf, pt->ofs.x, pt->ofs.y);
            if (pt->src_is_set) {
                cairo_rectangle(cr, pt->src.point.x + pt->ofs.x, pt->src.point.y + pt->ofs.y,
                                pt->src.size.width, pt->src.size.height);
                cairo_fill(cr);
            } else {
                cairo_paint(cr);
            }
            cairo_surface_flush(g2d->cairo_surf);
            cairo_destroy(cr);
            pp_resource_release(pt->image_data);
            pp_resource_unref(pt->image_data);
            break;
        case gpt_replace_contents:
            id = pp_resource_acquire(pt->image_data, PP_RESOURCE_IMAGE_DATA);
            if (!id)
                break;
            if (id->width == g2d->width || id->height == g2d->height) {
                void            *tmp;
                cairo_surface_t *tmp_surf;

                cairo_surface_flush(id->cairo_surf);
                cairo_surface_flush(g2d->cairo_surf);

                tmp = g2d->data;
                g2d->data = id->data;
                id->data = tmp;

                tmp_surf = g2d->cairo_surf;
                g2d->cairo_surf = id->cairo_surf;
                id->cairo_surf = tmp_surf;
            }
            pp_resource_release(pt->image_data);
            pp_resource_unref(pt->image_data);
            break;
        }
        free(pt);
    }

    pthread_mutex_lock(&g2d->lock);
    memcpy(g2d->second_buffer, g2d->data, g2d->stride * g2d->height);
    pthread_mutex_unlock(&g2d->lock);

    NPRect npr = {.top = 0, .left = 0, .bottom = g2d->height, .right = g2d->width};
    pp_resource_release(graphics_2d);
    if (pp_i->is_fullscreen) {
        XGraphicsExposeEvent ev = {
            .type = GraphicsExpose,
            .drawable = pp_i->fs_wnd,
            .width = pp_i->width,
            .height = pp_i->height
        };

        XSendEvent(pp_i->dpy, pp_i->fs_wnd, True, ExposureMask, (void *)&ev);
        XFlush(pp_i->dpy);
    } else {
        npn.invalidaterect(pp_i->npp, &npr);
        npn.forceredraw(pp_i->npp);
    }

    if (callback.flags != PP_COMPLETIONCALLBACK_FLAG_OPTIONAL) {
        trace_warning("%s, non-optional callback was skipped\n", __func__);
    }
    return PP_OK;
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
    trace_info("[PPB] {full} %s graphics_2d=%d, image_data=%d, top_left=%s, src_rect=%s\n",
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
    trace_info("[PPB] {full} %s graphics_2d=%d, image_data=%d\n", __func__+6, graphics_2d,
               image_data);
    ppb_graphics2d_replace_contents(graphics_2d, image_data);
}

static
int32_t
trace_ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s graphics_2d=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
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
