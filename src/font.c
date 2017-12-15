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

#include "font.h"
#include "pp_resource.h"
#include "ppb_image_data.h"
#include "ppb_var.h"
#include "tables.h"
#include "trace_core.h"
#include <cairo.h>
#include <glib.h>
#include <pango/pangocairo.h>
#include <string.h>

struct PP_Var
fpp_font_get_font_families(void)
{
    GString *builder;
    PangoFontFamily **families;
    int n;

    pango_font_map_list_families(tables_get_pango_font_map(), &families, &n);

    builder = g_string_new(NULL);
    for (int k = 0; k < n; k ++) {
        g_string_append(builder, pango_font_family_get_name(families[k]));
        g_string_append_c(builder, 0);
    }

    struct PP_Var var = ppb_var_var_from_utf8(builder->str, builder->len);
    g_string_free(builder, TRUE);
    g_free(families);

    return var;
}

void
fpp_font_init(struct fpp_font *ff, const struct PP_FontDescription_Dev *description)
{
    PangoFontDescription *font_desc = pp_font_desc_to_pango_font_desc(description);

    ff->family = description->face.type == PP_VARTYPE_STRING ? 0 : description->family;
    ff->letter_spacing = description->letter_spacing;
    ff->word_spacing = description->word_spacing;
    ff->font = pango_context_load_font(tables_get_pango_ctx(), font_desc);
    ff->font_desc = pango_font_describe_with_absolute_size(ff->font);
    pango_font_description_free(font_desc);

    // font description won't change during lifetime of browser_font resource,
    // so we can cache it here
    ff->measure_layout = pango_layout_new(tables_get_pango_ctx());
    pango_layout_set_font_description(ff->measure_layout, ff->font_desc);
}

void
fpp_font_destroy(struct fpp_font *ff)
{
    pango_font_description_free(ff->font_desc);
    g_object_unref(ff->font);
    g_object_unref(ff->measure_layout);
}

PP_Bool
fpp_font_describe(struct fpp_font *ff, struct PP_FontDescription_Dev *description,
                  struct PP_FontMetrics_Dev *metrics)
{
    memset(description, 0, sizeof(*description));
    memset(metrics, 0, sizeof(*metrics));

    const char *s_family = pango_font_description_get_family(ff->font_desc);
    description->face = ppb_var_var_from_utf8_z(s_family);

    description->family = ff->family >= 0 ? ff->family : 0;

    description->size = pango_font_description_get_size(ff->font_desc) / PANGO_SCALE;
    description->weight = pango_font_description_get_weight(ff->font_desc)/100 - 1;
    description->italic = (pango_font_description_get_style(ff->font_desc) != PANGO_STYLE_NORMAL);
    description->small_caps =
            (pango_font_description_get_variant(ff->font_desc) == PANGO_VARIANT_SMALL_CAPS);
    description->letter_spacing = ff->letter_spacing;
    description->word_spacing = ff->word_spacing;

    PangoFontMetrics *m = pango_font_get_metrics(ff->font, NULL);
    // TODO: use fontconfig-specific structures in pango to determine height and x-height
    metrics->ascent = pango_font_metrics_get_ascent(m) / PANGO_SCALE;
    metrics->descent = pango_font_metrics_get_descent(m) / PANGO_SCALE;
    metrics->height = (pango_font_metrics_get_ascent(m) +
                       pango_font_metrics_get_descent(m)) / PANGO_SCALE;
    metrics->line_spacing = 1; // TODO: get actual line spacing
    metrics->x_height = metrics->height;    // TODO: find out actual x-height

    pango_font_metrics_unref(m);
    return PP_TRUE;
}

PP_Bool
fpp_font_draw_text_at(struct fpp_font *ff, PP_Resource image_data,
                      const struct PP_TextRun_Dev *text, const struct PP_Point *position,
                      uint32_t color, const struct PP_Rect *clip, PP_Bool image_data_is_opaque)
{
    (void)image_data_is_opaque; // TODO: is it worth implementing?
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    cairo_t *cr = cairo_create(id->cairo_surf);
    if (clip) {
        cairo_rectangle(cr, clip->point.x, clip->point.y, clip->size.width, clip->size.height);
        cairo_clip(cr);
    }

    PangoFontMetrics *m = pango_font_get_metrics(ff->font, NULL);
    int32_t ascent = pango_font_metrics_get_ascent(m) / PANGO_SCALE;
    cairo_surface_mark_dirty(id->cairo_surf);
    if (position)
        cairo_move_to(cr, position->x, position->y - ascent);
    else
        cairo_move_to(cr, 0, 0);
    pango_font_metrics_unref(m);

    cairo_set_source_rgba(cr, ((color >> 16) & 0xffu) / 255.0,
                              ((color >> 8) & 0xffu) / 255.0,
                              ((color >> 0) & 0xffu) / 255.0,
                              ((color >> 24) & 0xffu) / 255.0);

    PangoLayout *layout = pango_cairo_create_layout(cr);
    uint32_t len = 0;
    const char *s = "";
    if (text->text.type == PP_VARTYPE_STRING)
        s = ppb_var_var_to_utf8(text->text, &len);

    // TODO: factor into rtl direction
    pango_layout_set_font_description(layout, ff->font_desc);
    pango_layout_set_text(layout, s, len);
    pango_cairo_layout_path(cr, layout);
    cairo_fill(cr);
    g_object_unref(layout);
    cairo_surface_flush(id->cairo_surf);
    cairo_destroy(cr);

    pp_resource_release(image_data);
    return PP_TRUE;
}

int32_t
fpp_font_measure_text(struct fpp_font *ff, const struct PP_TextRun_Dev *text)
{
    uint32_t len = 0;
    const char *s = "";
    if (text->text.type == PP_VARTYPE_STRING)
        s = ppb_var_var_to_utf8(text->text, &len);

    // TODO: factor into rtl direction
    pango_layout_set_text(ff->measure_layout, s, len);
    int width, height;
    pango_layout_get_pixel_size(ff->measure_layout, &width, &height);
    return width;
}

uint32_t
fpp_font_character_offset_for_pixel(struct fpp_font *ff, const struct PP_TextRun_Dev *text,
                                    int32_t pixel_position)
{
    return 0;
}

int32_t
fpp_font_pixel_offset_for_character(struct fpp_font *ff, const struct PP_TextRun_Dev *text,
                                    uint32_t char_offset)
{
    return 0;
}

PangoFontDescription *
pp_font_desc_to_pango_font_desc(const struct PP_FontDescription_Dev *description)
{
    PangoFontDescription *font_desc;

    if (description->face.type == PP_VARTYPE_STRING) {
        const char *s = ppb_var_var_to_utf8(description->face, NULL);
        font_desc = pango_font_description_from_string(s);
    } else {
        font_desc = pango_font_description_new();
        switch (description->family) {
        case PP_BROWSERFONT_TRUSTED_FAMILY_SERIF:
            pango_font_description_set_family(font_desc, "serif");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_SANSSERIF:
            pango_font_description_set_family(font_desc, "sans-serif");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_MONOSPACE:
            pango_font_description_set_family(font_desc, "monospace");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_DEFAULT:
            // fall through
        default:
            // do nothing
            break;
        }
    }

    pango_font_description_set_absolute_size(font_desc, description->size * PANGO_SCALE);
    pango_font_description_set_weight(font_desc, (description->weight + 1) * 100);
    if (description->italic)
        pango_font_description_set_style(font_desc, PANGO_STYLE_ITALIC);
    if (description->small_caps)
        pango_font_description_set_variant(font_desc, PANGO_VARIANT_SMALL_CAPS);

    return font_desc;
}

struct PP_FontDescription_Dev
pp_browser_font_desc_to_pp_font_desc(const struct PP_BrowserFont_Trusted_Description *descr)
{
    struct PP_FontDescription_Dev font_descr = {
        .face =             descr->face,
        .family =           (PP_FontFamily_Dev)descr->family,
        .size =             descr->size,
        .weight =           (PP_FontWeight_Dev)descr->weight,
        .italic =           descr->italic,
        .small_caps =       descr->small_caps,
        .letter_spacing =   descr->letter_spacing,
        .word_spacing =     descr->word_spacing,
        .padding =          descr->padding
    };
    return font_descr;
}

PangoFontDescription *
pp_browser_font_desc_to_pango_font_desc(const struct PP_BrowserFont_Trusted_Description *descr)
{
    struct PP_FontDescription_Dev font_descr = pp_browser_font_desc_to_pp_font_desc(descr);
    return pp_font_desc_to_pango_font_desc(&font_descr);
}
