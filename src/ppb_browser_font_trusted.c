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

#include "ppb_browser_font_trusted.h"
#include <ppapi/c/pp_var.h>
#include "trace.h"
#include "tables.h"
#include <stdlib.h>


struct PP_Var
ppb_browser_font_trusted_get_font_families(PP_Instance instance)
{
    return PP_MakeUndefined();
}

PP_Resource
ppb_browser_font_trusted_create(PP_Instance instance,
                                const struct PP_BrowserFont_Trusted_Description *description)
{
    PP_Resource font = pp_resource_allocate(PP_RESOURCE_BROWSER_FONT, instance);
    // TODO: store data provided in description
    return font;
}

void
ppb_browser_font_trusted_destroy(void *p)
{
    // nothing here yet to destroy
}

PP_Bool
ppb_browser_font_trusted_is_font(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_browser_font_trusted_describe(PP_Resource font,
                                  struct PP_BrowserFont_Trusted_Description *description,
                                  struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    return PP_FALSE;
}

PP_Bool
ppb_browser_font_trusted_draw_text_at(PP_Resource font, PP_Resource image_data,
                                      const struct PP_BrowserFont_Trusted_TextRun *text,
                                      const struct PP_Point *position, uint32_t color,
                                      const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    return PP_FALSE;
}

int32_t
ppb_browser_font_trusted_measure_text(PP_Resource font,
                                      const struct PP_BrowserFont_Trusted_TextRun *text)
{
    return 0;
}

uint32_t
ppb_browser_font_trusted_character_offset_for_pixel(PP_Resource font,
                    const struct PP_BrowserFont_Trusted_TextRun *text, int32_t pixel_position)
{
    return 0;
}

int32_t
ppb_browser_font_trusted_pixel_offset_for_character(PP_Resource font,
                    const struct PP_BrowserFont_Trusted_TextRun *text, uint32_t char_offset)
{
    return 0;
}


// trace wrappers
static
struct PP_Var
trace_ppb_browser_font_trusted_get_font_families(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_browser_font_trusted_get_font_families(instance);
}

static
PP_Resource
trace_ppb_browser_font_trusted_create(PP_Instance instance,
                                const struct PP_BrowserFont_Trusted_Description *description)
{
    char *s_face = trace_var_as_string(description->face);
    trace_info("[PPB] {part} %s instance=%d, description={.face=%s, .family=%d, .size=%u, "
               ".weight=%d, .italic=%u, .small_caps=%u, .letter_spacing=%d, .word_spacing=%d}\n",
               __func__+6, instance, s_face, description->family, description->size,
               description->weight, description->italic, description->small_caps,
               description->letter_spacing, description->word_spacing);
    free(s_face);
    return ppb_browser_font_trusted_create(instance, description);
}

static
PP_Bool
trace_ppb_browser_font_trusted_is_font(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_browser_font_trusted_is_font(resource);
}

static
PP_Bool
trace_ppb_browser_font_trusted_describe(PP_Resource font,
                                  struct PP_BrowserFont_Trusted_Description *description,
                                  struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    trace_info("[PPB] {zilch} %s font=%d\n", __func__+6, font);
    return ppb_browser_font_trusted_describe(font, description, metrics);
}

static
PP_Bool
trace_ppb_browser_font_trusted_draw_text_at(PP_Resource font, PP_Resource image_data,
                                      const struct PP_BrowserFont_Trusted_TextRun *text,
                                      const struct PP_Point *position, uint32_t color,
                                      const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    char *s_text_text = trace_var_as_string(text->text);
    char *s_position = trace_point_as_string(position);
    char *s_clip = trace_rect_as_string(clip);
    trace_info("[PPB] {zilch} %s font=%d, image_data=%d, text={.text=%s, .rtl=%u, "
               ".override_direction=%u}, position=%s, color=0x%06x, clip=%s, "
               "image_data_is_opaque=%u\n", __func__+6, font, image_data, s_text_text, text->rtl,
               text->override_direction, s_position, color, s_clip, image_data_is_opaque);
    free(s_text_text);
    free(s_position);
    free(s_clip);
    return ppb_browser_font_trusted_draw_text_at(font, image_data, text, position, color, clip,
                                                 image_data_is_opaque);
}

static
int32_t
trace_ppb_browser_font_trusted_measure_text(PP_Resource font,
                                      const struct PP_BrowserFont_Trusted_TextRun *text)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}\n",
               __func__+6, font, s_text_text, text->rtl, text->override_direction);
    free(s_text_text);
    return ppb_browser_font_trusted_measure_text(font, text);
}

static
uint32_t
trace_ppb_browser_font_trusted_character_offset_for_pixel(PP_Resource font,
                    const struct PP_BrowserFont_Trusted_TextRun *text, int32_t pixel_position)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "pixel_position=%d\n", __func__+6, font, s_text_text, text->rtl,
               text->override_direction, pixel_position);
    free(s_text_text);
    return ppb_browser_font_trusted_character_offset_for_pixel(font, text, pixel_position);
}

static
int32_t
trace_ppb_browser_font_trusted_pixel_offset_for_character(PP_Resource font,
                    const struct PP_BrowserFont_Trusted_TextRun *text, uint32_t char_offset)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "char_offset=%u\n", __func__+6, font, s_text_text, text->rtl,
               text->override_direction, char_offset);
    free(s_text_text);
    return ppb_browser_font_trusted_pixel_offset_for_character(font, text, char_offset);
}


const struct PPB_BrowserFont_Trusted_1_0 ppb_browser_font_trusted_interface_1_0 = {
    .GetFontFamilies =          trace_ppb_browser_font_trusted_get_font_families,
    .Create =                   trace_ppb_browser_font_trusted_create,
    .IsFont =                   trace_ppb_browser_font_trusted_is_font,
    .Describe =                 trace_ppb_browser_font_trusted_describe,
    .DrawTextAt =               trace_ppb_browser_font_trusted_draw_text_at,
    .MeasureText =              trace_ppb_browser_font_trusted_measure_text,
    .CharacterOffsetForPixel =  trace_ppb_browser_font_trusted_character_offset_for_pixel,
    .PixelOffsetForCharacter =  trace_ppb_browser_font_trusted_pixel_offset_for_character,
};
