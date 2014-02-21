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

#include <ppapi/c/private/ppb_flash.h>
#include <stddef.h>
#include <stdlib.h>
#include "trace.h"
#include "reverse_constant.h"


void
ppb_flash_set_instance_always_on_top(PP_Instance instance, PP_Bool on_top)
{
    return;
}

PP_Bool
ppb_flash_draw_glyphs(PP_Instance instance, PP_Resource pp_image_data,
                      const struct PP_BrowserFont_Trusted_Description *font_desc,
                      uint32_t color, const struct PP_Point *position, const struct PP_Rect *clip,
                      const float transformation[3][3], PP_Bool allow_subpixel_aa,
                      uint32_t glyph_count, const uint16_t glyph_indices[],
                      const struct PP_Point glyph_advances[])
{
    return PP_TRUE;
}

struct PP_Var
ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url)
{
    struct PP_Var var = {0};
    return var;
}

int32_t
ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action)
{
    return 0;
}

double
ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t)
{
    return 0.0;
}

struct PP_Var
ppb_flash_get_command_line_args(PP_Module module)
{
    struct PP_Var var = {0};
    return var;
}

void
ppb_flash_preload_font_win(const void *logfontw)
{
    return;
}

PP_Bool
ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect)
{
    return PP_TRUE;
}

void
ppb_flash_update_activity(PP_Instance instance)
{
    return;
}

struct PP_Var
ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting)
{
    struct PP_Var var = {0};

    switch (setting) {
    case PP_FLASHSETTING_3DENABLED:
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = PP_FALSE; // TODO: enable, when it's ready
        break;
    case PP_FLASHSETTING_INCOGNITO:
        // TODO: incognito?
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = PP_FALSE;
        break;
    case PP_FLASHSETTING_STAGE3DENABLED:
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = PP_FALSE;  // TODO: enable, when it's ready
        break;
    case PP_FLASHSETTING_LANGUAGE:
        // TODO: detect language
        var.type = PP_VARTYPE_STRING;
        var.value.as_id = (int64_t)(void*)"ru-RU";
        break;
    case PP_FLASHSETTING_NUMCORES:
        // TODO: check number of cores
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = 2;
        break;
    case PP_FLASHSETTING_LSORESTRICTIONS:
        // TODO: select restrictions
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = PP_FLASHLSORESTRICTIONS_IN_MEMORY;
        break;
    case PP_FLASHSETTING_STAGE3DBASELINEENABLED:
        // TODO: check if driver reliable enough
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = PP_FALSE;
        break;
    }

    return var;
}

PP_Bool
ppb_flash_set_crash_data(PP_Instance instance, PP_FlashCrashKey key, struct PP_Var value)
{
    return PP_TRUE;
}

int32_t
ppb_flash_enumerate_video_capture_devices(PP_Instance instance, PP_Resource video_capture,
                                          struct PP_ArrayOutput devices)
{
    return 0;
}

void
ppb_flash_run_message_loop(PP_Instance instance)
{
    return;
}

void
ppb_flash_quit_message_loop(PP_Instance instance)
{
    return;
}

int32_t
ppb_flash_invoke_printing(PP_Instance instance)
{
    return 0;
}

struct PP_Var
ppb_flash_get_device_id(PP_Instance instance)
{
    struct PP_Var var = {0};
    return var;
}

int32_t
ppb_flash_get_setting_int(PP_Instance instance, PP_FlashSetting setting)
{
    return 0;
}


// trace wrappers
static
void
trace_ppb_flash_set_instance_always_on_top(PP_Instance instance, PP_Bool on_top)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_flash_set_instance_always_on_top(instance, on_top);
}

static
PP_Bool
trace_ppb_flash_draw_glyphs(PP_Instance instance, PP_Resource pp_image_data,
                            const struct PP_BrowserFont_Trusted_Description *font_desc,
                            uint32_t color, const struct PP_Point *position,
                            const struct PP_Rect *clip, const float transformation[3][3],
                            PP_Bool allow_subpixel_aa, uint32_t glyph_count,
                            const uint16_t glyph_indices[], const struct PP_Point glyph_advances[])
{
    char *position_str = trace_point_as_string(position);
    char *clip_str = trace_rect_as_string(clip);
    trace_info("[PPB] {zilch} %s instance=%d, pp_image_data=%d, font_desc=%p, color=0x%x, "
               "position=%s, clip=%s, transformation={{%f,%f,%f},{%f,%f,%f},{%f,%f,%f}}, "
               "allow_subpixel_aa=%d, glyph_count=%u, glyph_indices=%p, glyph_advances=%p\n",
               __func__+6, instance, pp_image_data, font_desc, color, position_str, clip_str,
               transformation[0][0], transformation[0][1], transformation[0][2],
               transformation[1][0], transformation[1][1], transformation[1][2],
               transformation[2][0], transformation[2][1], transformation[2][2],
               allow_subpixel_aa, glyph_count, glyph_indices, glyph_advances);
    free(position_str);
    free(clip_str);
    return ppb_flash_draw_glyphs(instance, pp_image_data, font_desc, color, position, clip,
                                 transformation, allow_subpixel_aa, glyph_count, glyph_indices,
                                 glyph_advances);
}

static
struct PP_Var
trace_ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url)
{
    trace_info("[PPB] {zilch} %s instance=%d, url=%s\n", __func__+6, instance, url);
    return ppb_flash_get_proxy_for_url(instance, url);
}

static
int32_t
trace_ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action)
{
    trace_info("[PPB] {zilch} %s request_info=%d, target=%s, from_user_action=%d\n", __func__+6,
               request_info, target, from_user_action);
    return ppb_flash_navigate(request_info, target, from_user_action);
}

static
double
trace_ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t)
{
    trace_info("[PPB] {zilch} %s instance=%d, t=%f\n", __func__+6, instance, t);
    return ppb_flash_get_local_time_zone_offset(instance, t);
}

static
struct PP_Var
trace_ppb_flash_get_command_line_args(PP_Module module)
{
    trace_info("[PPB] {zilch} %s module=%d\n", __func__+6, module);
    return ppb_flash_get_command_line_args(module);
}

static
void
trace_ppb_flash_preload_font_win(const void *logfontw)
{
    trace_info("[PPB] {zilch} %s logfontw=%p\n", __func__+6, logfontw);
    ppb_flash_preload_font_win(logfontw);
}

static
PP_Bool
trace_ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect)
{
    char *rect_str = trace_rect_as_string(rect);
    trace_info("[PPB] {zilch} %s instance=%d, rect=%s\n", __func__+6, instance, rect_str);
    free(rect_str);
    return ppb_flash_is_rect_topmost(instance, rect);
}

static
void
trace_ppb_flash_update_activity(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_flash_update_activity(instance);
}

static
struct PP_Var
trace_ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting)
{
    trace_info("[PPB] {part} %s instance=%d, setting=%s\n", __func__+6, instance,
               reverse_pp_flash_setting(setting));
    return ppb_flash_get_setting(instance, setting);
}

static
PP_Bool
trace_ppb_flash_set_crash_data(PP_Instance instance, PP_FlashCrashKey key, struct PP_Var value)
{
    char *value_str = trace_var_as_string(value);
    trace_info("[PPB] {zilch} %s instance=%d, key=%d, value=%s\n", __func__+6, instance, key,
               value_str);
    free(value_str);
    return ppb_flash_set_crash_data(instance, key, value);
}

static
int32_t
trace_ppb_flash_enumerate_video_capture_devices(PP_Instance instance, PP_Resource video_capture,
                                                struct PP_ArrayOutput devices)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_enumerate_video_capture_devices(instance, video_capture, devices);
}

static
void
trace_ppb_flash_run_message_loop(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_flash_run_message_loop(instance);
}

static
void
trace_ppb_flash_quit_message_loop(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_flash_quit_message_loop(instance);
}

static
int32_t
trace_ppb_flash_invoke_printing(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_invoke_printing(instance);
}

static
struct PP_Var
trace_ppb_flash_get_device_id(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_get_device_id(instance);
}

static
int32_t
trace_ppb_flash_get_setting_int(PP_Instance instance, PP_FlashSetting setting)
{
    trace_info("[PPB] {zilch} %s instance=%d, setting=%s\n", __func__+6, instance,
               reverse_pp_flash_setting(setting));
    return ppb_flash_get_setting_int(instance, setting);
}


const struct PPB_Flash_13_0 ppb_flash_interface_13_0 = {
    .SetInstanceAlwaysOnTop = trace_ppb_flash_set_instance_always_on_top,
    .DrawGlyphs =           trace_ppb_flash_draw_glyphs,
    .GetProxyForURL =       trace_ppb_flash_get_proxy_for_url,
    .Navigate =             trace_ppb_flash_navigate,
    .GetLocalTimeZoneOffset = trace_ppb_flash_get_local_time_zone_offset,
    .GetCommandLineArgs =   trace_ppb_flash_get_command_line_args,
    .PreloadFontWin =       trace_ppb_flash_preload_font_win,
    .IsRectTopmost =        trace_ppb_flash_is_rect_topmost,
    .UpdateActivity =       trace_ppb_flash_update_activity,
    .GetSetting =           trace_ppb_flash_get_setting,
    .SetCrashData =         trace_ppb_flash_set_crash_data,
    .EnumerateVideoCaptureDevices = trace_ppb_flash_enumerate_video_capture_devices
};

const struct PPB_Flash_12_6 ppb_flash_interface_12_6 = {
    .SetInstanceAlwaysOnTop = trace_ppb_flash_set_instance_always_on_top,
    .DrawGlyphs =           trace_ppb_flash_draw_glyphs,
    .GetProxyForURL =       trace_ppb_flash_get_proxy_for_url,
    .Navigate =             trace_ppb_flash_navigate,
    .RunMessageLoop =       trace_ppb_flash_run_message_loop,
    .QuitMessageLoop =      trace_ppb_flash_quit_message_loop,
    .GetLocalTimeZoneOffset = trace_ppb_flash_get_local_time_zone_offset,
    .GetCommandLineArgs =   trace_ppb_flash_get_command_line_args,
    .PreloadFontWin =       trace_ppb_flash_preload_font_win,
    .IsRectTopmost =        trace_ppb_flash_is_rect_topmost,
    .InvokePrinting =       trace_ppb_flash_invoke_printing,
    .UpdateActivity =       trace_ppb_flash_update_activity,
    .GetDeviceID =          trace_ppb_flash_get_device_id,
    .GetSettingInt =        trace_ppb_flash_get_setting_int,
    .GetSetting =           trace_ppb_flash_get_setting,
    .SetCrashData =         trace_ppb_flash_set_crash_data,
    .EnumerateVideoCaptureDevices = trace_ppb_flash_enumerate_video_capture_devices,
};
