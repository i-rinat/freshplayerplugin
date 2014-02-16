#include <ppapi/c/ppb_view.h>
#include <stdlib.h>
#include "trace.h"


PP_Bool
ppb_view_is_view(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_view_get_rect(PP_Resource resource, struct PP_Rect* rect)
{
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
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_view_is_view(resource);
}

PP_Bool
trace_ppb_view_get_rect(PP_Resource resource, struct PP_Rect* rect)
{
    char *s_rect = trace_rect_as_string(rect);
    trace_info("[PPB] {zilch} %s resource=%d, rect=%s\n", __func__+6, resource, s_rect);
    free(s_rect);
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
    char *s_clip = trace_rect_as_string(clip);
    trace_info("[PPB] {zilch} %s resource=%d, clip=%s\n", __func__+6, resource, s_clip);
    free(s_clip);
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
