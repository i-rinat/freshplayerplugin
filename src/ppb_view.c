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

#include "config.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_view.h"
#include "static_assert.h"
#include "trace_core.h"

STATIC_ASSERT(sizeof(struct pp_view_s) <= LARGEST_RESOURCE_SIZE);

PP_Bool
ppb_view_is_view(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_VIEW;
}

static
void
ppb_view_destroy(void *ptr)
{
}

PP_Bool
ppb_view_get_rect(PP_Resource resource, struct PP_Rect *rect)
{
    struct pp_view_s *v = pp_resource_acquire(resource, PP_RESOURCE_VIEW);
    if (!v) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    rect->point.x =     v->rect.point.x;
    rect->point.y =     v->rect.point.y;
    rect->size.width =  v->rect.size.width;
    rect->size.height = v->rect.size.height;
    pp_resource_release(resource);
    return PP_TRUE;
}

PP_Bool
ppb_view_is_fullscreen(PP_Resource resource)
{
    // TODO: detect when in fullscreen mode
    return PP_FALSE;
}

PP_Bool
ppb_view_is_visible(PP_Resource resource)
{
    // TODO: detect when not visible
    return PP_TRUE;
}

PP_Bool
ppb_view_is_page_visible(PP_Resource resource)
{
    // TODO: detect when not visible
    return PP_TRUE;
}

PP_Bool
ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect *clip)
{
    struct pp_view_s *v = pp_resource_acquire(resource, PP_RESOURCE_VIEW);
    if (!v) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }
    clip->point.x =     v->rect.point.x;
    clip->point.y =     v->rect.point.y;
    clip->size.width =  v->rect.size.width;
    clip->size.height = v->rect.size.height;
    pp_resource_release(resource);
    return PP_TRUE;
}

float
ppb_view_get_device_scale(PP_Resource resource)
{
    return config.device_scale;
}

float
ppb_view_get_css_scale(PP_Resource resource)
{
    return config.device_scale;
}

PP_Bool
ppb_view_get_scroll_offset(PP_Resource resource, struct PP_Point *offset)
{
    offset->x = 0;
    offset->y = 0;
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_view_is_view(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_view(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_get_rect(PP_Resource resource, struct PP_Rect *rect)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_rect(resource, rect);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_is_fullscreen(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_fullscreen(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_is_visible(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_visible(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_is_page_visible(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_page_visible(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect *clip)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_clip_rect(resource, clip);
}

TRACE_WRAPPER
float
trace_ppb_view_get_device_scale(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_device_scale(resource);
}

TRACE_WRAPPER
float
trace_ppb_view_get_css_scale(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_css_scale(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_view_get_scroll_offset(PP_Resource resource, struct PP_Point *offset)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_scroll_offset(resource, offset);
}


const struct PPB_View_Dev_0_1 ppb_view_dev_interface_0_1 = {
    .GetDeviceScale =   TWRAPF(ppb_view_get_device_scale),
    .GetCSSScale =      TWRAPF(ppb_view_get_css_scale),
};

const struct PPB_View_1_0 ppb_view_interface_1_0 = {
    .IsView =           TWRAPF(ppb_view_is_view),
    .GetRect =          TWRAPF(ppb_view_get_rect),
    .IsFullscreen =     TWRAPZ(ppb_view_is_fullscreen),
    .IsVisible =        TWRAPZ(ppb_view_is_visible),
    .IsPageVisible =    TWRAPZ(ppb_view_is_page_visible),
    .GetClipRect =      TWRAPF(ppb_view_get_clip_rect),
};

const struct PPB_View_1_1 ppb_view_interface_1_1 = {
    .IsView =           TWRAPF(ppb_view_is_view),
    .GetRect =          TWRAPF(ppb_view_get_rect),
    .IsFullscreen =     TWRAPZ(ppb_view_is_fullscreen),
    .IsVisible =        TWRAPZ(ppb_view_is_visible),
    .IsPageVisible =    TWRAPZ(ppb_view_is_page_visible),
    .GetClipRect =      TWRAPF(ppb_view_get_clip_rect),
    .GetDeviceScale =   TWRAPF(ppb_view_get_device_scale),
    .GetCSSScale =      TWRAPF(ppb_view_get_css_scale),
};

const struct PPB_View_1_2 ppb_view_interface_1_2 = {
    .IsView =           TWRAPF(ppb_view_is_view),
    .GetRect =          TWRAPF(ppb_view_get_rect),
    .IsFullscreen =     TWRAPZ(ppb_view_is_fullscreen),
    .IsVisible =        TWRAPZ(ppb_view_is_visible),
    .IsPageVisible =    TWRAPZ(ppb_view_is_page_visible),
    .GetClipRect =      TWRAPF(ppb_view_get_clip_rect),
    .GetDeviceScale =   TWRAPF(ppb_view_get_device_scale),
    .GetCSSScale =      TWRAPF(ppb_view_get_css_scale),
    .GetScrollOffset =  TWRAPZ(ppb_view_get_scroll_offset),
};

static
void
__attribute__((constructor))
constructor_ppb_view(void)
{
    register_interface(PPB_VIEW_DEV_INTERFACE_0_1, &ppb_view_dev_interface_0_1);
    register_interface(PPB_VIEW_INTERFACE_1_0, &ppb_view_interface_1_0);
    register_interface(PPB_VIEW_INTERFACE_1_1, &ppb_view_interface_1_1);
    register_interface(PPB_VIEW_INTERFACE_1_2, &ppb_view_interface_1_2);
    register_resource(PP_RESOURCE_VIEW, ppb_view_destroy);
}
