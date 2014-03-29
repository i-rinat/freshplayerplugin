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

#include "ppb_font_dev.h"
#include <stdlib.h>
#include "trace.h"


struct PP_Var
ppb_font_dev_get_font_families(PP_Instance instance)
{
    return PP_MakeUndefined();
}

PP_Resource
ppb_font_dev_create(PP_Instance instance, const struct PP_FontDescription_Dev *description)
{
    return 0;
}

PP_Bool
ppb_font_dev_is_font(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_font_dev_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                      struct PP_FontMetrics_Dev *metrics)
{
    return PP_FALSE;
}

PP_Bool
ppb_font_dev_draw_text_at(PP_Resource font, PP_Resource image_data,
                          const struct PP_TextRun_Dev *text, const struct PP_Point *position,
                          uint32_t color, const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    return PP_FALSE;
}

int32_t
ppb_font_dev_measure_text(PP_Resource font, const struct PP_TextRun_Dev *text)
{
    return -1;
}

uint32_t
ppb_font_dev_character_offset_for_pixel(PP_Resource font, const struct PP_TextRun_Dev *text,
                                        int32_t pixel_position)
{
    return 0;
}

int32_t
ppb_font_dev_pixel_offset_for_character(PP_Resource font, const struct PP_TextRun_Dev *text,
                                        uint32_t char_offset)
{
    return -1;
}


// trace wrappers
static
struct PP_Var
trace_ppb_font_dev_get_font_families(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_get_font_families(instance);
}

static
PP_Resource
trace_ppb_font_dev_create(PP_Instance instance, const struct PP_FontDescription_Dev *description)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_create(instance, description);
}

static
PP_Bool
trace_ppb_font_dev_is_font(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_is_font(resource);
}

static
PP_Bool
trace_ppb_font_dev_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                            struct PP_FontMetrics_Dev *metrics)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_describe(font, description, metrics);
}

static
PP_Bool
trace_ppb_font_dev_draw_text_at(PP_Resource font, PP_Resource image_data,
                                const struct PP_TextRun_Dev *text, const struct PP_Point *position,
                                uint32_t color, const struct PP_Rect *clip,
                                PP_Bool image_data_is_opaque)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_draw_text_at(font, image_data, text, position, color, clip,
                                     image_data_is_opaque);
}

static
int32_t
trace_ppb_font_dev_measure_text(PP_Resource font, const struct PP_TextRun_Dev *text)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_measure_text(font, text);
}

static
uint32_t
trace_ppb_font_dev_character_offset_for_pixel(PP_Resource font, const struct PP_TextRun_Dev *text,
                                              int32_t pixel_position)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_character_offset_for_pixel(font, text, pixel_position);
}

static
int32_t
trace_ppb_font_dev_pixel_offset_for_character(PP_Resource font, const struct PP_TextRun_Dev *text,
                                              uint32_t char_offset)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_font_dev_pixel_offset_for_character(font, text, char_offset);
}

const struct PPB_Font_Dev_0_6 ppb_font_dev_interface_0_6 = {
    .GetFontFamilies =          trace_ppb_font_dev_get_font_families,
    .Create =                   trace_ppb_font_dev_create,
    .IsFont =                   trace_ppb_font_dev_is_font,
    .Describe =                 trace_ppb_font_dev_describe,
    .DrawTextAt =               trace_ppb_font_dev_draw_text_at,
    .MeasureText =              trace_ppb_font_dev_measure_text,
    .CharacterOffsetForPixel =  trace_ppb_font_dev_character_offset_for_pixel,
    .PixelOffsetForCharacter =  trace_ppb_font_dev_pixel_offset_for_character,
};
