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

#include "ppb_graphics2d.h"
#include "ppb_core.h"
#include <pthread.h>
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "pp_resource.h"
#include "pp_interface.h"


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
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource graphics_2d = pp_resource_allocate(PP_RESOURCE_GRAPHICS2D, pp_i);
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_error("%s, can't create graphics2d resource\n", __func__);
        return 0;
    }

    g2d->is_always_opaque = is_always_opaque;
    g2d->scale = config.device_scale;
    g2d->width =  size->width;
    g2d->height = size->height;
    g2d->stride = 4 * size->width;

    g2d->scaled_width = g2d->width * g2d->scale + 0.5;
    g2d->scaled_height = g2d->height * g2d->scale + 0.5;
    g2d->scaled_stride = 4 * g2d->scaled_width;

    g2d->data = calloc(g2d->stride * g2d->height, 1);
    g2d->second_buffer = calloc(g2d->scaled_stride * g2d->scaled_height, 1);
    if (!g2d->data || !g2d->second_buffer) {
        trace_warning("%s, can't allocate memory\n", __func__);
        free_and_nullify(g2d->data);
        free_and_nullify(g2d->second_buffer);
        pp_resource_release(graphics_2d);
        ppb_core_release_resource(graphics_2d);
        return 0;
    }
    g2d->cairo_surf = cairo_image_surface_create_for_data((unsigned char *)g2d->data,
                            CAIRO_FORMAT_ARGB32, g2d->width, g2d->height, g2d->stride);
    g2d->task_list = NULL;

    if (pp_i->is_transparent) {
        // we need XRender picture (which in turn requires X Pixmap) to alpha blend
        // our images with existing pixmap provided by the browser. This is only needed
        // is instance is transparent, therefore depth is always 32-bit.
        pthread_mutex_lock(&display.lock);
        g2d->pixmap = XCreatePixmap(display.x, DefaultRootWindow(display.x), g2d->scaled_width,
                                    g2d->scaled_height, 32);
        XFlush(display.x);
        g2d->xr_pict = XRenderCreatePicture(display.x, g2d->pixmap, display.pictfmt_argb32, 0, 0);
        g2d->gc = XCreateGC(display.x, g2d->pixmap, 0, 0);
        XFlush(display.x);
        pthread_mutex_unlock(&display.lock);
    }

    pp_resource_release(graphics_2d);
    return graphics_2d;
}

static
void
ppb_graphics2d_destroy(void *p)
{
    if (!p)
        return;
    struct pp_graphics2d_s *g2d = p;
    free_and_nullify(g2d->data);
    free_and_nullify(g2d->second_buffer);
    if (g2d->cairo_surf) {
        cairo_surface_destroy(g2d->cairo_surf);
        g2d->cairo_surf = NULL;
    }

    if (g2d->instance->is_transparent) {
        pthread_mutex_lock(&display.lock);
        XRenderFreePicture(display.x, g2d->xr_pict);
        XFreePixmap(display.x, g2d->pixmap);
        XFreeGC(display.x, g2d->gc);
        pthread_mutex_unlock(&display.lock);
    }
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
    if (!g2d) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    struct g2d_paint_task_s *pt = g_slice_alloc(sizeof(*pt));
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
    if (!g2d) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    struct g2d_paint_task_s *pt = g_slice_alloc(sizeof(*pt));
    pt->type = gpt_replace_contents;
    pp_resource_ref(image_data);
    pt->image_data = image_data;
    g2d->task_list = g_list_append(g2d->task_list, pt);

    pp_resource_release(graphics_2d);
}

static
void
call_forceredraw_ptac(void *param)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(GPOINTER_TO_SIZE(param));
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    if (pp_i->is_fullscreen || pp_i->windowed_mode) {
        XEvent ev = {
            .xgraphicsexpose = {
                .type =     GraphicsExpose,
                .drawable = pp_i->is_fullscreen ? pp_i->fs_wnd : pp_i->wnd,
                .width =    pp_i->is_fullscreen ? pp_i->fs_width : pp_i->width,
                .height =   pp_i->is_fullscreen ? pp_i->fs_height : pp_i->height,
            }
        };

        pthread_mutex_lock(&display.lock);
        XSendEvent(display.x, ev.xgraphicsexpose.drawable, True, ExposureMask, &ev);
        XFlush(display.x);
        pthread_mutex_unlock(&display.lock);
    } else {
        NPRect npr = {.top = 0, .left = 0, .bottom = pp_i->height, .right = pp_i->width};
        npn.invalidaterect(pp_i->npp, &npr);
        npn.forceredraw(pp_i->npp);
    }
}

int32_t
ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    struct pp_graphics2d_s *g2d = pp_resource_acquire(graphics_2d, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct pp_instance_s *pp_i = g2d->instance;

    pthread_mutex_lock(&display.lock);

    if (pp_i->graphics_in_progress) {
        pp_resource_release(graphics_2d);
        pthread_mutex_unlock(&display.lock);
        return PP_ERROR_INPROGRESS;
    }

    if (pp_i->graphics == graphics_2d) {
        pp_i->graphics_ccb = callback;
        pp_i->graphics_in_progress = 1;
    }
    pthread_mutex_unlock(&display.lock);

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
            cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
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
        g_slice_free(struct g2d_paint_task_s, pt);
    }

    // scale image
    if (g2d->scaled_width == g2d->width && g2d->scaled_height == g2d->height) {
        // fast path: exact copy
        memcpy(g2d->second_buffer, g2d->data, g2d->stride * g2d->height);
    } else {
        // slow path: scaling required
        cairo_surface_t *surf;
        surf = cairo_image_surface_create_for_data((unsigned char *)g2d->second_buffer,
                CAIRO_FORMAT_ARGB32, g2d->scaled_width, g2d->scaled_height, g2d->scaled_stride);
        cairo_t *cr = cairo_create(surf);
        cairo_scale(cr, g2d->scale, g2d->scale);
        cairo_set_source_surface(cr, g2d->cairo_surf, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
        cairo_surface_destroy(surf);
    }

    pp_resource_release(graphics_2d);

    ppb_core_call_on_browser_thread(pp_i->id, call_forceredraw_ptac, GSIZE_TO_POINTER(pp_i->id));

    if (callback.func) {
        // invoke callback as soon as possible if graphics device is not bound to an instance
        if (pp_i->graphics != graphics_2d)
            ppb_core_call_on_main_thread2(0, callback, PP_OK, __func__);
        return PP_OK_COMPLETIONPENDING;
    }

    trace_error("%s, callback.func==NULL branch not implemented\n", __func__);
    return PP_OK;
}

PP_Bool
ppb_graphics2d_set_scale(PP_Resource resource, float scale)
{
    struct pp_graphics2d_s *g2d = pp_resource_acquire(resource, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    g2d->scale = scale * config.device_scale;
    g2d->scaled_width = g2d->width * g2d->scale + 0.5;
    g2d->scaled_height = g2d->height * g2d->scale + 0.5;
    g2d->scaled_stride = 4 * g2d->scaled_width;

    free(g2d->second_buffer);
    g2d->second_buffer = calloc(g2d->scaled_stride * g2d->scaled_height, 1);
    PP_Bool ret = !!g2d->second_buffer;

    pp_resource_release(resource);
    return ret;
}

float
ppb_graphics2d_get_scale(PP_Resource resource)
{
    struct pp_graphics2d_s *g2d = pp_resource_acquire(resource, PP_RESOURCE_GRAPHICS2D);
    if (!g2d) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    float scale = g2d->scale / config.device_scale;
    pp_resource_release(resource);
    return scale;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_graphics2d_create(PP_Instance instance, const struct PP_Size *size,
                            PP_Bool is_always_opaque)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {full} %s instance=%d, size=%s, is_always_opaque=%d\n", __func__+6,
               instance, s_size, is_always_opaque);
    g_free(s_size);
    return ppb_graphics2d_create(instance, size, is_always_opaque);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_graphics2d_is_graphics2d(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_graphics2d_is_graphics2d(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_graphics2d_describe(PP_Resource graphics_2d, struct PP_Size *size,
                              PP_Bool *is_always_opaque)
{
    char *s_size = trace_size_as_string(size);
    trace_info("[PPB] {zilch} %s graphics_2d=%d, size=%s\n", __func__+6, graphics_2d, s_size);
    g_free(s_size);
    return ppb_graphics2d_describe(graphics_2d, size, is_always_opaque);
}

TRACE_WRAPPER
void
trace_ppb_graphics2d_paint_image_data(PP_Resource graphics_2d, PP_Resource image_data,
                                      const struct PP_Point *top_left,
                                      const struct PP_Rect *src_rect)
{
    char *s_top_left = trace_point_as_string(top_left);
    char *s_src_rect = trace_rect_as_string(src_rect);
    trace_info("[PPB] {full} %s graphics_2d=%d, image_data=%d, top_left=%s, src_rect=%s\n",
               __func__+6, graphics_2d, image_data, s_top_left, s_src_rect);
    g_free(s_top_left);
    g_free(s_src_rect);
    ppb_graphics2d_paint_image_data(graphics_2d, image_data, top_left, src_rect);
}

TRACE_WRAPPER
void
trace_ppb_graphics2d_scroll(PP_Resource graphics_2d, const struct PP_Rect *clip_rect,
                            const struct PP_Point *amount)
{
    char *s_clip_rect = trace_rect_as_string(clip_rect);
    char *s_amount = trace_point_as_string(amount);
    trace_info("[PPB] {zilch} %s graphics_2d=%d, clip_rect=%s, amount=%s\n", __func__+6,
               graphics_2d, s_clip_rect, s_amount);
    g_free(s_clip_rect);
    g_free(s_amount);
    ppb_graphics2d_scroll(graphics_2d, clip_rect, amount);
}

TRACE_WRAPPER
void
trace_ppb_graphics2d_replace_contents(PP_Resource graphics_2d, PP_Resource image_data)
{
    trace_info("[PPB] {full} %s graphics_2d=%d, image_data=%d\n", __func__+6, graphics_2d,
               image_data);
    ppb_graphics2d_replace_contents(graphics_2d, image_data);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics2d_flush(PP_Resource graphics_2d, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s graphics_2d=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, graphics_2d, callback.func, callback.user_data, callback.flags);
    return ppb_graphics2d_flush(graphics_2d, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_graphics2d_set_scale(PP_Resource resource, float scale)
{
    trace_info("[PPB] {full} %s resource=%d, scale=%f\n", __func__+6, resource, scale);
    return ppb_graphics2d_set_scale(resource, scale);
}

TRACE_WRAPPER
float
trace_ppb_graphics2d_get_scale(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_graphics2d_get_scale(resource);
}


const struct PPB_Graphics2D_1_0 ppb_graphics2d_interface_1_0 = {
    .Create =           TWRAPF(ppb_graphics2d_create),
    .IsGraphics2D =     TWRAPF(ppb_graphics2d_is_graphics2d),
    .Describe =         TWRAPZ(ppb_graphics2d_describe),
    .PaintImageData =   TWRAPF(ppb_graphics2d_paint_image_data),
    .Scroll =           TWRAPZ(ppb_graphics2d_scroll),
    .ReplaceContents =  TWRAPF(ppb_graphics2d_replace_contents),
    .Flush =            TWRAPF(ppb_graphics2d_flush),
};

const struct PPB_Graphics2D_1_1 ppb_graphics2d_interface_1_1 = {
    .Create =           TWRAPF(ppb_graphics2d_create),
    .IsGraphics2D =     TWRAPF(ppb_graphics2d_is_graphics2d),
    .Describe =         TWRAPZ(ppb_graphics2d_describe),
    .PaintImageData =   TWRAPF(ppb_graphics2d_paint_image_data),
    .Scroll =           TWRAPZ(ppb_graphics2d_scroll),
    .ReplaceContents =  TWRAPF(ppb_graphics2d_replace_contents),
    .Flush =            TWRAPF(ppb_graphics2d_flush),
    .SetScale =         TWRAPF(ppb_graphics2d_set_scale),
    .GetScale =         TWRAPF(ppb_graphics2d_get_scale),
};

static
void
__attribute__((constructor))
constructor_ppb_graphics2d(void)
{
    register_interface(PPB_GRAPHICS_2D_INTERFACE_1_0, &ppb_graphics2d_interface_1_0);
    register_interface(PPB_GRAPHICS_2D_INTERFACE_1_1, &ppb_graphics2d_interface_1_1);
    register_resource(PP_RESOURCE_GRAPHICS2D, ppb_graphics2d_destroy);
}
