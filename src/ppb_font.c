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

#include "ppb_font.h"
#include <stdlib.h>
#include "trace.h"
#include <glib.h>
#include <pango/pangoft2.h>
#include "tables.h"
#include "ppb_var.h"


struct PP_Var
ppb_font_get_font_families(PP_Instance instance)
{
    PangoFontFamily **families;
    int n, k, total_len;

    pango_font_map_list_families(tables_get_pango_font_map(), &families, &n);
    total_len = 0;
    for (k = 0; k < n; k ++) {
        const char *name = pango_font_family_get_name(families[k]);
        total_len += strlen(name) + 1; // with '\0' at the end
    }

    char *s = malloc(total_len);
    char *ptr = s;
    for (k = 0; k < n; k ++) {
        const char *name = pango_font_family_get_name(families[k]);
        const int len = strlen(name);
        memcpy(ptr, name, len + 1); // with '\0' at the end
        ptr += len + 1;
    }

    struct PP_Var var = ppb_var_var_from_utf8(s, total_len);
    free(s);
    g_free(families);

    return var;
}

PP_Resource
ppb_font_create(PP_Instance instance, const struct PP_FontDescription_Dev *description)
{
    return 0;
}

PP_Bool
ppb_font_is_font(PP_Resource resource)
{
    return PP_TRUE;
}

PP_Bool
ppb_font_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                  struct PP_FontMetrics_Dev *metrics)
{
    return PP_FALSE;
}

PP_Bool
ppb_font_draw_text_at(PP_Resource font, PP_Resource image_data, const struct PP_TextRun_Dev *text,
                      const struct PP_Point *position, uint32_t color, const struct PP_Rect *clip,
                      PP_Bool image_data_is_opaque)
{
    return PP_FALSE;
}

int32_t
ppb_font_measure_text(PP_Resource font, const struct PP_TextRun_Dev *text)
{
    return -1;
}

uint32_t
ppb_font_character_offset_for_pixel(PP_Resource font, const struct PP_TextRun_Dev *text,
                                    int32_t pixel_position)
{
    return 0;
}

int32_t
ppb_font_pixel_offset_for_character(PP_Resource font, const struct PP_TextRun_Dev *text,
                                    uint32_t char_offset)
{
    return -1;
}


// trace wrappers
TRACE_WRAPPER
struct PP_Var
trace_ppb_font_get_font_families(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_font_get_font_families(instance);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_font_create(PP_Instance instance, const struct PP_FontDescription_Dev *description)
{
    gchar *s_face = trace_var_as_string(description->face);
    trace_info("[PPB] {zilch} %s instance=%d, description={.face=%s, .family=%u, .size=%u, "
               ".weight=%u, .italic=%u, .small_caps=%u, .letter_spacing=%d, .word_spacing=%d}\n",
               __func__+6, instance, s_face, description->family, description->size,
               description->weight, description->italic, description->small_caps,
               description->letter_spacing, description->word_spacing);
    g_free(s_face);
    return ppb_font_create(instance, description);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_font_is_font(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_font_is_font(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_font_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                        struct PP_FontMetrics_Dev *metrics)
{
    trace_info("[PPB] {zilch} %s font=%d\n", __func__+6, font);
    return ppb_font_describe(font, description, metrics);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_font_draw_text_at(PP_Resource font, PP_Resource image_data,
                            const struct PP_TextRun_Dev *text, const struct PP_Point *position,
                            uint32_t color, const struct PP_Rect *clip,
                            PP_Bool image_data_is_opaque)
{
    gchar *s_text = trace_var_as_string(text->text);
    gchar *s_position = trace_point_as_string(position);
    gchar *s_clip = trace_rect_as_string(clip);
    trace_info("[PPB] {zilch} %s font=%d, image_data=%d, text={.text=%s, .rtl=%u, "
               ".override_direction=%u}, position=%s, color=0x%08x, clip=%s, "
               "image_data_is_opaque=%u\n", __func__+6, font, image_data, s_text, text->rtl,
               text->override_direction, s_position, color, s_clip, image_data_is_opaque);
    g_free(s_text);
    g_free(s_position);
    g_free(s_clip);
    return ppb_font_draw_text_at(font, image_data, text, position, color, clip,
                                 image_data_is_opaque);
}

TRACE_WRAPPER
int32_t
trace_ppb_font_measure_text(PP_Resource font, const struct PP_TextRun_Dev *text)
{
    gchar *s_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}\n",
               __func__+6, font, s_text, text->rtl, text->override_direction);
    g_free(s_text);
    return ppb_font_measure_text(font, text);
}

TRACE_WRAPPER
uint32_t
trace_ppb_font_character_offset_for_pixel(PP_Resource font, const struct PP_TextRun_Dev *text,
                                          int32_t pixel_position)
{
    gchar *s_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "pixel_position=%d\n", __func__+6, font, s_text, text->rtl, text->override_direction,
               pixel_position);
    g_free(s_text);
    return ppb_font_character_offset_for_pixel(font, text, pixel_position);
}

TRACE_WRAPPER
int32_t
trace_ppb_font_pixel_offset_for_character(PP_Resource font, const struct PP_TextRun_Dev *text,
                                          uint32_t char_offset)
{
    gchar *s_text = trace_var_as_string(text->text);
    trace_info("[PPB] {zilch} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}, "
               "char_offset=%u\n", __func__+6, font, s_text, text->rtl, text->override_direction,
               char_offset);
    g_free(s_text);
    return ppb_font_pixel_offset_for_character(font, text, char_offset);
}


const struct PPB_Font_Dev_0_6 ppb_font_dev_interface_0_6 = {
    .GetFontFamilies =          TWRAPF(ppb_font_get_font_families),
    .Create =                   TWRAPZ(ppb_font_create),
    .IsFont =                   TWRAPZ(ppb_font_is_font),
    .Describe =                 TWRAPZ(ppb_font_describe),
    .DrawTextAt =               TWRAPZ(ppb_font_draw_text_at),
    .MeasureText =              TWRAPZ(ppb_font_measure_text),
    .CharacterOffsetForPixel =  TWRAPZ(ppb_font_character_offset_for_pixel),
    .PixelOffsetForCharacter =  TWRAPZ(ppb_font_pixel_offset_for_character),
};
