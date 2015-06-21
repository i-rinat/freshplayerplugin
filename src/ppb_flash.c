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

#include "ppb_flash.h"
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "reverse_constant.h"
#include "ppb_var.h"
#include "ppb_url_loader.h"
#include "ppb_core.h"
#include "ppb_message_loop.h"
#include "pp_resource.h"
#include <ppapi/c/dev/ppb_font_dev.h>
#include <ppapi/c/pp_errors.h>
#include "screensaver_control.h"
#include "pp_interface.h"
#include "ppb_video_capture.h"


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
    struct pp_image_data_s *id = pp_resource_acquire(pp_image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    cairo_t *cr = cairo_create(id->cairo_surf);

    const char *font_family;
    if (font_desc->face.type == PP_VARTYPE_STRING) {
        font_family = ppb_var_var_to_utf8(font_desc->face, NULL);
    } else {
        switch (font_desc->family) {
        case PP_BROWSERFONT_TRUSTED_FAMILY_SERIF:
            font_family = "serif";
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_SANSSERIF:
            font_family = "sans-serif";
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_MONOSPACE:
            font_family = "monospace";
            break;
        default:
            font_family = "";
            break;
        }
    }

    cairo_select_font_face(cr, font_family,
                           font_desc->italic ? CAIRO_FONT_SLANT_ITALIC
                                             : CAIRO_FONT_SLANT_NORMAL,
                           font_desc->weight >= (int)PP_FONTWEIGHT_700 ? CAIRO_FONT_WEIGHT_BOLD
                                                                       : CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, font_desc->size);

    if (allow_subpixel_aa) {
        cairo_font_options_t *options = cairo_font_options_create();
        cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_SUBPIXEL);
        cairo_set_font_options(cr, options);
        cairo_font_options_destroy(options);
    }

    if (clip) {
        cairo_rectangle(cr, clip->point.x, clip->point.y, clip->size.width, clip->size.height);
        cairo_clip(cr);
    }

    cairo_set_source_rgba(cr, ((color >> 16) & 0xffu) / 255.0,
                              ((color >> 8) & 0xffu) / 255.0,
                              ((color >> 0) & 0xffu) / 255.0,
                              ((color >> 24) & 0xffu) / 255.0);

    cairo_matrix_t matrix;
    cairo_matrix_init(&matrix, transformation[0][0], transformation[0][1],
                               transformation[1][0], transformation[1][1],
                               transformation[0][2], transformation[1][2]);
    cairo_set_matrix(cr, &matrix);

    cairo_glyph_t *c_glyphs = malloc(glyph_count * sizeof(cairo_glyph_t));
    struct PP_Point current = {.x = 0, .y = 0};
    for (uint32_t k = 0; k < glyph_count; k ++) {
        c_glyphs[k].index = glyph_indices[k];
        c_glyphs[k].x = current.x;
        c_glyphs[k].y = current.y;
        current.x += glyph_advances[k].x;
        current.y += glyph_advances[k].y;
    }
    cairo_show_glyphs(cr, c_glyphs, glyph_count);
    free(c_glyphs);

    cairo_surface_flush(id->cairo_surf);
    cairo_destroy(cr);

    pp_resource_release(pp_image_data);
    return PP_TRUE;
}

struct get_proxy_for_url_param_s {
    PP_Instance         instance_id;
    const char         *url;
    struct PP_Var       result;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
get_proxy_for_url_ptac(void *user_data)
{
    struct get_proxy_for_url_param_s *p = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance_id);

    p->result = PP_MakeUndefined();
    if (pp_i && pp_i->npp && npn.getvalueforurl) {
        char *value = NULL;
        uint32_t len = 0;
        NPError err;

        err = npn.getvalueforurl(pp_i->npp, NPNURLVProxy, p->url, &value, &len);
        if (err == NPERR_NO_ERROR) {
            p->result = ppb_var_var_from_utf8(value, len);
        }
    }

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
get_proxy_for_url_comt(void *user_data, int32_t result)
{
    ppb_core_call_on_browser_thread(0, get_proxy_for_url_ptac, user_data);
}

struct PP_Var
ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url)
{
    struct get_proxy_for_url_param_s *p = g_slice_alloc(sizeof(*p));
    p->instance_id =    instance;
    p->url =            url;
    p->m_loop =         ppb_message_loop_get_current();
    p->depth =          ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(get_proxy_for_url_comt, p), 0,
                                           PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}

static
void
nop_callback(void *user_data, int32_t result)
{
    (void)user_data;
    (void)result;
    // do nothing
}

int32_t
ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action)
{

    struct pp_url_request_info_s *ri =
        pp_resource_acquire(request_info, PP_RESOURCE_URL_REQUEST_INFO);
    if (!ri) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }
    pp_resource_release(request_info);

    PP_Resource url_loader = ppb_url_loader_create(ri->instance->id);
    int32_t result = ppb_url_loader_open_target(url_loader, request_info,
                                                PP_MakeCCB(nop_callback, NULL), target);
    ppb_core_release_resource(url_loader);
    if (result != PP_OK && result != PP_OK_COMPLETIONPENDING)
        return result;

    return PP_OK;
}

double
ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t)
{
    time_t timep = t;
    struct tm lt = { };
    localtime_r(&timep, &lt);
    return lt.tm_gmtoff;
}

struct PP_Var
ppb_flash_get_command_line_args(PP_Module module)
{
    return ppb_var_var_from_utf8_z(config.flash_command_line);
}

void
ppb_flash_preload_font_win(const void *logfontw)
{
    return;
}

struct topmost_rect_param_s {
    PP_Instance     instance;
    struct PP_Rect  rect;
    PP_Bool         result;
    PP_Resource     m_loop;
    int             depth;
};

static
void
topmost_rect_ptac(void *param)
{
    struct topmost_rect_param_s *p = param;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i) {
        trace_error("%s, no instance\n", __func__);
        goto err_1;
    }

    p->result = PP_FALSE;

    NPString topmost_func_src = NPString_literal(
        "(function(elem, x, y) {"
            "var r = elem.getBoundingClientRect();"
            "return document.elementFromPoint(x + r.left, y + r.top) == elem;"
        "})");
    NPVariant topmost_func;

    if (!npn.evaluate(pp_i->npp, pp_i->np_window_obj, &topmost_func_src, &topmost_func))
        goto err_1;

    if (!NPVARIANT_IS_OBJECT(topmost_func))
        goto err_1;

    NPObject *topmost_func_obj = NPVARIANT_TO_OBJECT(topmost_func);

    NPVariant is_topmost;
    NPVariant args[3];

    OBJECT_TO_NPVARIANT(pp_i->np_plugin_element_obj, args[0]);
    INT32_TO_NPVARIANT(p->rect.point.x + p->rect.size.width / 2, args[1]);
    INT32_TO_NPVARIANT(p->rect.point.y + p->rect.size.height / 2, args[2]);

    if (!npn.invokeDefault(pp_i->npp, topmost_func_obj, args, 3, &is_topmost))
        goto err_2;

    if (!NPVARIANT_IS_BOOLEAN(is_topmost))
        goto err_3;

    p->result = NPVARIANT_TO_BOOLEAN(is_topmost);

err_3:
    npn.releasevariantvalue(&is_topmost);
err_2:
    npn.releasevariantvalue(&topmost_func);
err_1:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
topmost_rect_comt(void *user_data, int32_t result)
{
    ppb_core_call_on_browser_thread(0, topmost_rect_ptac, user_data);
}

PP_Bool
ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect)
{
    if (!rect)
        return PP_FALSE;

    struct topmost_rect_param_s *p = g_slice_alloc(sizeof(*p));
    p->instance =       instance;
    p->rect =          *rect;
    p->m_loop =         ppb_message_loop_get_current();
    p->depth =          ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(topmost_rect_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    PP_Bool result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}

void
ppb_flash_update_activity(PP_Instance instance)
{
    pthread_mutex_lock(&display.lock);
    screensaver_deactivate(display.x, display.screensaver_types);
    pthread_mutex_unlock(&display.lock);
}

struct PP_Var
get_flashsetting_language(void)
{
    char *lang = getenv("LANG");
    if (!lang)
        return ppb_var_var_from_utf8_z("en-US");

    // make a working copy
    lang = strdup(lang);

    // cut character encoding if exists
    char *ptr = strchr(lang, '.');
    if (ptr)
        *ptr = 0;

    // replace _ by
    ptr = strchr(lang, '_');
    if (ptr)
        *ptr = '-';

    struct PP_Var res = ppb_var_var_from_utf8_z(lang);
    free(lang);
    return res;
}


struct PP_Var
ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting)
{
    long cpu_count;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_MakeUndefined();
    }
    struct PP_Var var = PP_MakeUndefined();

    switch (setting) {
    case PP_FLASHSETTING_3DENABLED:
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = config.enable_3d ? PP_TRUE : PP_FALSE;
        break;
    case PP_FLASHSETTING_INCOGNITO:
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = pp_i->incognito_mode ? PP_TRUE : PP_FALSE;
        break;
    case PP_FLASHSETTING_STAGE3DENABLED:
        var.type = PP_VARTYPE_BOOL;
        var.value.as_bool = config.enable_3d ? PP_TRUE : PP_FALSE;
        break;
    case PP_FLASHSETTING_LANGUAGE:
        var = get_flashsetting_language();
        break;
    case PP_FLASHSETTING_NUMCORES:
        cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = cpu_count > 0 ? cpu_count : 1;
        break;
    case PP_FLASHSETTING_LSORESTRICTIONS:
        var.type = PP_VARTYPE_INT32;
        var.value.as_int = PP_FLASHLSORESTRICTIONS_NONE;
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
    int32_t ret = ppb_video_capture_enumerate_devices(video_capture, devices,
                                                      PP_MakeCCB(nop_callback, NULL));
    if (ret == PP_OK || ret == PP_OK_COMPLETIONPENDING)
        return PP_OK;

    return ret;
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
    return PP_MakeUndefined();
}

int32_t
ppb_flash_get_setting_int(PP_Instance instance, PP_FlashSetting setting)
{
    return 0;
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_flash_set_instance_always_on_top(PP_Instance instance, PP_Bool on_top)
{
    trace_info("[PPB] {zilch} %s instance=%d, on_top=%d\n", __func__+6, instance, on_top);
    ppb_flash_set_instance_always_on_top(instance, on_top);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_draw_glyphs(PP_Instance instance, PP_Resource pp_image_data,
                            const struct PP_BrowserFont_Trusted_Description *font_desc,
                            uint32_t color, const struct PP_Point *position,
                            const struct PP_Rect *clip, const float transformation[3][3],
                            PP_Bool allow_subpixel_aa, uint32_t glyph_count,
                            const uint16_t glyph_indices[], const struct PP_Point glyph_advances[])
{
    char *s_position = trace_point_as_string(position);
    char *s_clip = trace_rect_as_string(clip);
    char *s_face = trace_var_as_string(font_desc->face);
    trace_info("[PPB] {full} %s instance=%d, pp_image_data=%d, font_desc={.face=%s, .family=%d, "
               ".size=%u, .weight=%d, .italic=%u, .small_caps=%u, .letter_spacing=%d, "
               ".word_spacing=%d}, color=0x%x, position=%s, clip=%s, transformation={{%f,%f,%f},"
               "{%f,%f,%f},{%f,%f,%f}}, allow_subpixel_aa=%d, glyph_count=%u, glyph_indices=%p, "
               "glyph_advances=%p\n", __func__+6, instance, pp_image_data, s_face,
               font_desc->family, font_desc->size, font_desc->weight, font_desc->italic,
               font_desc->small_caps, font_desc->letter_spacing, font_desc->word_spacing,
               color, s_position, s_clip,
               transformation[0][0], transformation[0][1], transformation[0][2],
               transformation[1][0], transformation[1][1], transformation[1][2],
               transformation[2][0], transformation[2][1], transformation[2][2],
               allow_subpixel_aa, glyph_count, glyph_indices, glyph_advances);
    g_free(s_position);
    g_free(s_clip);
    g_free(s_face);
    return ppb_flash_draw_glyphs(instance, pp_image_data, font_desc, color, position, clip,
                                 transformation, allow_subpixel_aa, glyph_count, glyph_indices,
                                 glyph_advances);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url)
{
    trace_info("[PPB] {full} %s instance=%d, url=%s\n", __func__+6, instance, url);
    return ppb_flash_get_proxy_for_url(instance, url);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action)
{
    trace_info("[PPB] {full} %s request_info=%d, target=%s, from_user_action=%d\n", __func__+6,
               request_info, target, from_user_action);
    return ppb_flash_navigate(request_info, target, from_user_action);
}

TRACE_WRAPPER
double
trace_ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t)
{
    trace_info("[PPB] {full} %s instance=%d, t=%f\n", __func__+6, instance, t);
    return ppb_flash_get_local_time_zone_offset(instance, t);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_flash_get_command_line_args(PP_Module module)
{
    trace_info("[PPB] {full} %s module=%d\n", __func__+6, module);
    return ppb_flash_get_command_line_args(module);
}

TRACE_WRAPPER
void
trace_ppb_flash_preload_font_win(const void *logfontw)
{
    trace_info("[PPB] {zilch} %s logfontw=%p\n", __func__+6, logfontw);
    ppb_flash_preload_font_win(logfontw);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect)
{
    char *rect_str = trace_rect_as_string(rect);
    trace_info("[PPB] {full} %s instance=%d, rect=%s\n", __func__+6, instance, rect_str);
    g_free(rect_str);
    return ppb_flash_is_rect_topmost(instance, rect);
}

TRACE_WRAPPER
void
trace_ppb_flash_update_activity(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_flash_update_activity(instance);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting)
{
    trace_info("[PPB] {full} %s instance=%d, setting=%s\n", __func__+6, instance,
               reverse_pp_flash_setting(setting));
    return ppb_flash_get_setting(instance, setting);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_set_crash_data(PP_Instance instance, PP_FlashCrashKey key, struct PP_Var value)
{
    char *value_str = trace_var_as_string(value);
    trace_info("[PPB] {full} %s instance=%d, key=%d, value=%s\n", __func__+6, instance, key,
               value_str);
    g_free(value_str);
    return ppb_flash_set_crash_data(instance, key, value);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_enumerate_video_capture_devices(PP_Instance instance, PP_Resource video_capture,
                                                struct PP_ArrayOutput devices)
{
    trace_info("[PPB] {full} %s instance=%d, video_capture=%d, "
               "devices={.GetDataBuffer=%p, .user_data=%p}\n", __func__+6, instance, video_capture,
               devices.GetDataBuffer, devices.user_data);
    return ppb_flash_enumerate_video_capture_devices(instance, video_capture, devices);
}

TRACE_WRAPPER
void
trace_ppb_flash_run_message_loop(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_flash_run_message_loop(instance);
}

TRACE_WRAPPER
void
trace_ppb_flash_quit_message_loop(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_flash_quit_message_loop(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_invoke_printing(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_invoke_printing(instance);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_flash_get_device_id(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_get_device_id(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_get_setting_int(PP_Instance instance, PP_FlashSetting setting)
{
    trace_info("[PPB] {zilch} %s instance=%d, setting=%s\n", __func__+6, instance,
               reverse_pp_flash_setting(setting));
    return ppb_flash_get_setting_int(instance, setting);
}


const struct PPB_Flash_13_0 ppb_flash_interface_13_0 = {
    .SetInstanceAlwaysOnTop =       TWRAPZ(ppb_flash_set_instance_always_on_top),
    .DrawGlyphs =                   TWRAPF(ppb_flash_draw_glyphs),
    .GetProxyForURL =               TWRAPF(ppb_flash_get_proxy_for_url),
    .Navigate =                     TWRAPF(ppb_flash_navigate),
    .GetLocalTimeZoneOffset =       TWRAPF(ppb_flash_get_local_time_zone_offset),
    .GetCommandLineArgs =           TWRAPF(ppb_flash_get_command_line_args),
    .PreloadFontWin =               TWRAPZ(ppb_flash_preload_font_win),
    .IsRectTopmost =                TWRAPF(ppb_flash_is_rect_topmost),
    .UpdateActivity =               TWRAPF(ppb_flash_update_activity),
    .GetSetting =                   TWRAPF(ppb_flash_get_setting),
    .SetCrashData =                 TWRAPF(ppb_flash_set_crash_data),
    .EnumerateVideoCaptureDevices = TWRAPF(ppb_flash_enumerate_video_capture_devices),
};

const struct PPB_Flash_12_6 ppb_flash_interface_12_6 = {
    .SetInstanceAlwaysOnTop =       TWRAPZ(ppb_flash_set_instance_always_on_top),
    .DrawGlyphs =                   TWRAPF(ppb_flash_draw_glyphs),
    .GetProxyForURL =               TWRAPF(ppb_flash_get_proxy_for_url),
    .Navigate =                     TWRAPF(ppb_flash_navigate),
    .RunMessageLoop =               TWRAPZ(ppb_flash_run_message_loop),
    .QuitMessageLoop =              TWRAPZ(ppb_flash_quit_message_loop),
    .GetLocalTimeZoneOffset =       TWRAPF(ppb_flash_get_local_time_zone_offset),
    .GetCommandLineArgs =           TWRAPF(ppb_flash_get_command_line_args),
    .PreloadFontWin =               TWRAPZ(ppb_flash_preload_font_win),
    .IsRectTopmost =                TWRAPF(ppb_flash_is_rect_topmost),
    .InvokePrinting =               TWRAPZ(ppb_flash_invoke_printing),
    .UpdateActivity =               TWRAPF(ppb_flash_update_activity),
    .GetDeviceID =                  TWRAPZ(ppb_flash_get_device_id),
    .GetSettingInt =                TWRAPZ(ppb_flash_get_setting_int),
    .GetSetting =                   TWRAPF(ppb_flash_get_setting),
    .SetCrashData =                 TWRAPF(ppb_flash_set_crash_data),
    .EnumerateVideoCaptureDevices = TWRAPF(ppb_flash_enumerate_video_capture_devices),
};

static
void
__attribute__((constructor))
constructor_ppb_flash(void)
{
    register_interface(PPB_FLASH_INTERFACE_12_6, &ppb_flash_interface_12_6);
    register_interface(PPB_FLASH_INTERFACE_13_0, &ppb_flash_interface_13_0);
}
