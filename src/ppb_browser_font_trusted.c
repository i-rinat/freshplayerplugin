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
#include "ppb_var.h"
#include "trace.h"
#include "tables.h"
#include <stdlib.h>
#include <string.h>
#include "pp_resource.h"


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
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    PangoFontDescription *font_desc = pp_font_desc_to_pango_font_desc(description);

    bf->family = description->face.type == PP_VARTYPE_STRING ? -1 : description->family;
    bf->letter_spacing = description->letter_spacing;
    bf->word_spacing = description->word_spacing;
    bf->font = pango_context_load_font(tables_get_pango_ctx(), font_desc);
    bf->font_desc = pango_font_describe_with_absolute_size(bf->font);
    pango_font_description_free(font_desc);

    pp_resource_release(font);
    return font;
}

void
ppb_browser_font_trusted_destroy(void *p)
{
    struct pp_browser_font_s *bf = p;
    if (!bf)
        return;
}

PP_Bool
ppb_browser_font_trusted_is_font(PP_Resource resource)
{
    return PP_RESOURCE_BROWSER_FONT == pp_resource_get_type(resource);
}

PP_Bool
ppb_browser_font_trusted_describe(PP_Resource font,
                                  struct PP_BrowserFont_Trusted_Description *description,
                                  struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf)
        return PP_FALSE;

    memset(description, 0, sizeof(*description));
    memset(metrics, 0, sizeof(*metrics));

    const char *s_family = pango_font_description_get_family(bf->font_desc);
    description->face = PP_MakeString(s_family);

    description->family = bf->family >= 0 ? bf->family : 0;

    description->size = pango_font_description_get_size(bf->font_desc) / PANGO_SCALE;
    description->weight = pango_font_description_get_weight(bf->font_desc)/100 - 1;
    description->italic = (pango_font_description_get_style(bf->font_desc) != PANGO_STYLE_NORMAL);
    description->small_caps =
            (pango_font_description_get_variant(bf->font_desc) == PANGO_VARIANT_SMALL_CAPS);
    description->letter_spacing = bf->letter_spacing;
    description->word_spacing = bf->word_spacing;

    PangoFontMetrics *m = pango_font_get_metrics(bf->font, NULL);
    // TODO: use fontconfig-specific structures in pango to determine height and x-height
    metrics->ascent = pango_font_metrics_get_ascent(m) / PANGO_SCALE;
    metrics->descent = pango_font_metrics_get_descent(m) / PANGO_SCALE;
    metrics->height = (pango_font_metrics_get_ascent(m) +
                       pango_font_metrics_get_descent(m)) / PANGO_SCALE;
    metrics->line_spacing = 1; // TODO: get actual line spacing
    metrics->x_height = metrics->height;    // TODO: find out actual x-height

    pango_font_metrics_unref(m);
    pp_resource_release(font);
    return PP_TRUE;
}

PP_Bool
ppb_browser_font_trusted_draw_text_at(PP_Resource font, PP_Resource image_data,
                                      const struct PP_BrowserFont_Trusted_TextRun *text,
                                      const struct PP_Point *position, uint32_t color,
                                      const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    (void)image_data_is_opaque; // TODO: is it worth implementing?
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf)
        return PP_FALSE;
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        pp_resource_release(font);
        return PP_FALSE;
    }

    cairo_t *cr = cairo_create(id->cairo_surf);
    if (clip) {
        cairo_rectangle(cr, clip->point.x, clip->point.y, clip->size.width, clip->size.height);
        cairo_clip(cr);
    }

    PangoFontMetrics *m = pango_font_get_metrics(bf->font, NULL);
    int32_t ascent = pango_font_metrics_get_ascent(m) / PANGO_SCALE;
    cairo_surface_mark_dirty(id->cairo_surf);
    if (position)
        cairo_move_to(cr, position->x, position->y - ascent);
    else
        cairo_move_to(cr, 0, 0);
    pango_font_metrics_unref(m);

    cairo_set_source_rgba(cr, (color & 0xffu) / 255.0, ((color >> 8) & 0xffu) / 255.0,
                          ((color >> 16) & 0xffu) / 255.0, ((color >> 24) & 0xffu) / 255.0);

    PangoLayout *layout = pango_cairo_create_layout(cr);
    uint32_t len = 0;
    const char *s = "";
    if (text->text.type == PP_VARTYPE_STRING)
        s = ppb_var_var_to_utf8(text->text, &len);

    // TODO: factor into rtl direction
    pango_layout_set_font_description(layout, bf->font_desc);
    pango_layout_set_text(layout, s, len);
    pango_cairo_layout_path(cr, layout);
    cairo_fill(cr);
    g_object_unref(layout);
    cairo_surface_flush(id->cairo_surf);
    cairo_destroy(cr);

    pp_resource_release(font);
    pp_resource_release(image_data);
    return PP_FALSE;
}

int32_t
ppb_browser_font_trusted_measure_text(PP_Resource font,
                                      const struct PP_BrowserFont_Trusted_TextRun *text)
{
    struct pp_browser_font_s *bf = pp_resource_acquire(font, PP_RESOURCE_BROWSER_FONT);
    if (!bf)
        return -1;

    PangoLayout *layout = pango_layout_new(tables_get_pango_ctx());
    uint32_t len = 0;
    const char *s = "";
    if (text->text.type == PP_VARTYPE_STRING)
        s = ppb_var_var_to_utf8(text->text, &len);

    // TODO: factor into rtl direction
    pango_layout_set_text(layout, s, len);
    int width, height;
    pango_layout_get_pixel_size(layout, &width, &height);
    g_object_unref(layout);

    pp_resource_release(font);
    return width;
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
    trace_info("[PPB] {full} %s instance=%d, description={.face=%s, .family=%d, .size=%u, "
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
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_browser_font_trusted_is_font(resource);
}

static
PP_Bool
trace_ppb_browser_font_trusted_describe(PP_Resource font,
                                  struct PP_BrowserFont_Trusted_Description *description,
                                  struct PP_BrowserFont_Trusted_Metrics *metrics)
{
    trace_info("[PPB] {full} %s font=%d\n", __func__+6, font);
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
    trace_info("[PPB] {full} %s font=%d, image_data=%d, text={.text=%s, .rtl=%u, "
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
    trace_info("[PPB] {full} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}\n",
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
