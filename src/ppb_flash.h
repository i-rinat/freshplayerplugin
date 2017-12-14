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

#include <ppapi/c/private/ppb_flash.h>

void
ppb_flash_set_instance_always_on_top(PP_Instance instance, PP_Bool on_top);

PP_Bool
ppb_flash_draw_glyphs(PP_Instance instance, PP_Resource pp_image_data,
                      const struct PP_BrowserFont_Trusted_Description *font_desc,
                      uint32_t color, const struct PP_Point *position, const struct PP_Rect *clip,
                      const float transformation[3][3], PP_Bool allow_subpixel_aa,
                      uint32_t glyph_count, const uint16_t glyph_indices[],
                      const struct PP_Point glyph_advances[]);

struct PP_Var
ppb_flash_get_proxy_for_url(PP_Instance instance, const char *url);

int32_t
ppb_flash_navigate(PP_Resource request_info, const char *target, PP_Bool from_user_action);

double
ppb_flash_get_local_time_zone_offset(PP_Instance instance, PP_Time t);

struct PP_Var
ppb_flash_get_command_line_args(PP_Module module);

void
ppb_flash_preload_font_win(const void *logfontw);

PP_Bool
ppb_flash_is_rect_topmost(PP_Instance instance, const struct PP_Rect *rect);

void
ppb_flash_update_activity(PP_Instance instance);

struct PP_Var
ppb_flash_get_setting(PP_Instance instance, PP_FlashSetting setting);

PP_Bool
ppb_flash_set_crash_data(PP_Instance instance, PP_FlashCrashKey key, struct PP_Var value);

int32_t
ppb_flash_enumerate_video_capture_devices(PP_Instance instance, PP_Resource video_capture,
                                          struct PP_ArrayOutput devices);

void
ppb_flash_run_message_loop(PP_Instance instance);

void
ppb_flash_quit_message_loop(PP_Instance instance);

int32_t
ppb_flash_invoke_printing(PP_Instance instance);

struct PP_Var
ppb_flash_get_device_id(PP_Instance instance);

int32_t
ppb_flash_get_setting_int(PP_Instance instance, PP_FlashSetting setting);
