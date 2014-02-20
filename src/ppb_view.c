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

#include <ppapi/c/ppb_view.h>
#include <stdlib.h>
#include "trace.h"
#include "pp_resource.h"


PP_Bool
ppb_view_is_view(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_VIEW;
}

PP_Bool
ppb_view_get_rect(PP_Resource resource, struct PP_Rect* rect)
{
    rect->point.x = 0;
    rect->point.y = 0;
    rect->size.width = 100;
    rect->size.height = 100;
    return PP_TRUE;
}

PP_Bool
ppb_view_is_fullscreen(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_view_is_visible(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_view_is_page_visible(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect* clip)
{
    clip->point.x = 0;
    clip->point.y = 0;
    clip->size.width = 100;
    clip->size.height = 100;
    return PP_TRUE;
}

float
ppb_view_get_device_scale(PP_Resource resource)
{
    return 1.0;
}

float
ppb_view_get_css_scale(PP_Resource resource)
{
    return 1.0;
}

// trace wrappers
PP_Bool
trace_ppb_view_is_view(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_view(resource);
}

PP_Bool
trace_ppb_view_get_rect(PP_Resource resource, struct PP_Rect* rect)
{
    trace_info("[PPB] {fake} %s resource=%d, rect=%p\n", __func__+6, resource, rect);
    return ppb_view_get_rect(resource, rect);
}

PP_Bool
trace_ppb_view_is_fullscreen(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_fullscreen(resource);
}

PP_Bool
trace_ppb_view_is_visible(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_visible(resource);
}

PP_Bool
trace_ppb_view_is_page_visible(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_page_visible(resource);
}

PP_Bool
trace_ppb_view_get_clip_rect(PP_Resource resource, struct PP_Rect* clip)
{
    trace_info("[PPB] {fake} %s resource=%d, clip=%p\n", __func__+6, resource, clip);
    return ppb_view_get_clip_rect(resource, clip);
}

float
trace_ppb_view_get_device_scale(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_device_scale(resource);
}

float
trace_ppb_view_get_css_scale(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_get_css_scale(resource);
}

const struct PPB_View_1_0 ppb_view_interface_1_0 = {
    .IsView =           trace_ppb_view_is_view,
    .GetRect =          trace_ppb_view_get_rect,
    .IsFullscreen =     trace_ppb_view_is_fullscreen,
    .IsVisible =        trace_ppb_view_is_visible,
    .IsPageVisible =    trace_ppb_view_is_page_visible,
    .GetClipRect =      trace_ppb_view_get_clip_rect,
};

const struct PPB_View_1_1 ppb_view_interface_1_1 = {
    .IsView =           trace_ppb_view_is_view,
    .GetRect =          trace_ppb_view_get_rect,
    .IsFullscreen =     trace_ppb_view_is_fullscreen,
    .IsVisible =        trace_ppb_view_is_visible,
    .IsPageVisible =    trace_ppb_view_is_page_visible,
    .GetClipRect =      trace_ppb_view_get_clip_rect,
    .GetDeviceScale =   trace_ppb_view_get_device_scale,
    .GetCSSScale =      trace_ppb_view_get_css_scale,
};
