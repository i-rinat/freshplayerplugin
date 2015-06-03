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

#include "ppb_browser_font.h"
#include "ppb_var.h"
#include "trace.h"
#include "tables.h"
#include <stdlib.h>
#include <string.h>
#include <ppapi/c/pp_errors.h>
#include "pp_resource.h"
#include "pp_interface.h"
#include "font.h"


struct PP_Var
ppb_browser_font_get_font_families(PP_Instance instance)
{
    return PP_MakeUndefined();
}

PP_Resource
ppb_browser_font_create(PP_Instance instance,
                        const struct PP_BrowserFont_Trusted_Description *description)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource font = pp_resource_allocate(PP_RESOURCE_BROWSER_FONT, pp_i);
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    struct PP_FontDescription_Dev font_descr = pp_browser_font_desc_to_pp_font_desc(description);
    fpp_font_init(&bf->ff, &font_descr);

    pp_resource_release(font);
    return font;
}

static
void
ppb_browser_font_destroy(void *p)
{
    struct pp_browser_font_s *bf = p;
    if (!bf)
        return;
    fpp_font_destroy(&bf->ff);
}

PP_Bool
ppb_browser_font_is_font(PP_Resource resource)
{
    return PP_RESOURCE_BROWSER_FONT == pp_resource_get_type(resource);
}

PP_Bool
ppb_browser_font_describe(PP_Resource font,
                          struct PP_BrowserFont_Trusted_Description *description,
                          struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    struct PP_FontDescription_Dev font_descr = {};
    struct PP_FontMetrics_Dev font_metrics = {};
    PP_Bool ret = fpp_font_describe(&bf->ff, &font_descr, &font_metrics);

    description->face =             font_descr.face;
    description->family =           (PP_BrowserFont_Trusted_Family)font_descr.family;
    description->size =             font_descr.size;
    description->weight =           (PP_BrowserFont_Trusted_Weight)font_descr.weight;
    description->italic =           font_descr.italic;
    description->small_caps =       font_descr.small_caps;
    description->letter_spacing =   font_descr.letter_spacing;
    description->word_spacing =     font_descr.word_spacing;
    description->padding =          font_descr.padding;

    metrics->height =       font_metrics.height;
    metrics->ascent =       font_metrics.ascent;
    metrics->descent =      font_metrics.descent;
    metrics->line_spacing = font_metrics.line_spacing;
    metrics->x_height =     font_metrics.x_height;

    pp_resource_release(font);
    return ret;
}

PP_Bool
ppb_browser_font_draw_text_at(PP_Resource font, PP_Resource image_data,
                              const struct PP_BrowserFont_Trusted_TextRun *text,
                              const struct PP_Point *position, uint32_t color,
                              const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    struct PP_TextRun_Dev text_run = {
        .text =                 text->text,
        .rtl =                  text->rtl,
        .override_direction =   text->override_direction
    };

    PP_Bool ret = fpp_font_draw_text_at(&bf->ff, image_data, &text_run, position, color, clip,
                                        image_data_is_opaque);

    pp_resource_release(font);
    return ret;
}

int32_t
ppb_browser_font_measure_text(PP_Resource font,
                              const struct PP_BrowserFont_Trusted_TextRun *text)
{
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_FAILED;
    }

    struct PP_TextRun_Dev text_run = {
        .text =                 text->text,
        .rtl =                  text->rtl,
        .override_direction =   text->override_direction
    };

    int32_t width = fpp_font_measure_text(&bf->ff, &text_run);
    pp_resource_release(font);
    return width;
}

uint32_t
ppb_browser_font_character_offset_for_pixel(PP_Resource font,
                                            const struct PP_BrowserFont_Trusted_TextRun *text,
                                            int32_t pixel_position)
{
    return 0;
}

int32_t
ppb_browser_font_pixel_offset_for_character(PP_Resource font,
                                            const struct PP_BrowserFont_Trusted_TextRun *text,
                                            uint32_t char_offset)
{
    return 0;
}


// trace wrappers
TRACE_WRAPPER
struct PP_Var
trace_ppb_browser_font_get_font_families(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_browser_font_get_font_families(instance);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_browser_font_create(PP_Instance instance,
                              const struct PP_BrowserFont_Trusted_Description *description)
{
    char *s_face = trace_var_as_string(description->face);
    trace_info("[PPB] {full} %s instance=%d, description={.face=%s, .family=%d, .size=%u, "
               ".weight=%d, .italic=%u, .small_caps=%u, .letter_spacing=%d, .word_spacing=%d}\n",
               __func__+6, instance, s_face, description->family, description->size,
               description->weight, description->italic, description->small_caps,
               description->letter_spacing, description->word_spacing);
    g_free(s_face);
    return ppb_browser_font_create(instance, description);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_browser_font_is_font(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_browser_font_is_font(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_browser_font_describe(PP_Resource font,
                                struct PP_BrowserFont_Trusted_Description *description,
                                struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    trace_info("[PPB] {full} %s font=%d\n", __func__+6, font);
    return ppb_browser_font_describe(font, description, metrics);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_browser_font_draw_text_at(PP_Resource font, PP_Resource image_data,
                                    const struct PP_BrowserFont_Trusted_TextRun *text,
                                    const struct PP_Point *position, uint32_t color,
                                    const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    char *s_text_text = trace_var_as_string(text->text);
    char *s_position = trace_point_as_string(position);
    char *s_clip = trace_rect_as_string(clip);
    trace_info("[PPB] {full} %s font=%d, image_data=%d, text={.text=%s, .rtl=%u, "
               ".override_direction=%u}, position=%s, color=0x%06x, clip=%s, "
               "image_data_is_opaque=%u\n", __func__+6, font, image_data, s_text_text, text->rtl,
               text->override_direction, s_position, color, s_clip, image_data_is_opaque);
    g_free(s_text_text);
    g_free(s_position);
    g_free(s_clip);
    return ppb_browser_font_draw_text_at(font, image_data, text, position, color, clip,
                                         image_data_is_opaque);
}

TRACE_WRAPPER
int32_t
trace_ppb_browser_font_measure_text(PP_Resource font,
                                    const struct PP_BrowserFont_Trusted_TextRun *text)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {full} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}\n",
               __func__+6, font, s_text_text, text->rtl, text->override_direction);
    g_free(s_text_text);
    return ppb_browser_font_measure_text(font, text);
}

TRACE_WRAPPER
uint32_t
trace_ppb_browser_font_character_offset_for_pixel(PP_Resource font,
                                                  const struct PP_BrowserFont_Trusted_TextRun *text,
                                                  int32_t pixel_position)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "pixel_position=%d\n", __func__+6, font, s_text_text, text->rtl,
               text->override_direction, pixel_position);
    g_free(s_text_text);
    return ppb_browser_font_character_offset_for_pixel(font, text, pixel_position);
}

TRACE_WRAPPER
int32_t
trace_ppb_browser_font_pixel_offset_for_character(PP_Resource font,
                                                  const struct PP_BrowserFont_Trusted_TextRun *text,
                                                  uint32_t char_offset)
{
    char *s_text_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "char_offset=%u\n", __func__+6, font, s_text_text, text->rtl,
               text->override_direction, char_offset);
    g_free(s_text_text);
    return ppb_browser_font_pixel_offset_for_character(font, text, char_offset);
}


const struct PPB_BrowserFont_Trusted_1_0 ppb_browser_font_trusted_interface_1_0 = {
    .GetFontFamilies =          TWRAPZ(ppb_browser_font_get_font_families),
    .Create =                   TWRAPF(ppb_browser_font_create),
    .IsFont =                   TWRAPF(ppb_browser_font_is_font),
    .Describe =                 TWRAPF(ppb_browser_font_describe),
    .DrawTextAt =               TWRAPF(ppb_browser_font_draw_text_at),
    .MeasureText =              TWRAPF(ppb_browser_font_measure_text),
    .CharacterOffsetForPixel =  TWRAPZ(ppb_browser_font_character_offset_for_pixel),
    .PixelOffsetForCharacter =  TWRAPZ(ppb_browser_font_pixel_offset_for_character),
};

static
void
__attribute__((constructor))
constructor_ppb_browser_font(void)
{
    register_interface(PPB_BROWSERFONT_TRUSTED_INTERFACE_1_0,
                       &ppb_browser_font_trusted_interface_1_0);
    register_resource(PP_RESOURCE_BROWSER_FONT, ppb_browser_font_destroy);
}
