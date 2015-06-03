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
#include "font.h"
#include <ppapi/c/pp_errors.h>
#include "pp_interface.h"


struct PP_Var
ppb_font_get_font_families(PP_Instance instance)
{
    return fpp_font_get_font_families();
}

PP_Resource
ppb_font_create(PP_Instance instance, const struct PP_FontDescription_Dev *description)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    PP_Resource font = pp_resource_allocate(PP_RESOURCE_FONT, pp_i);
    struct pp_font_s *f = pp_resource_acquire(font, PP_RESOURCE_FONT);
    if (!f) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    fpp_font_init(&f->ff, description);

    pp_resource_release(font);
    return font;
}

static
void
ppb_font_destroy(void *p)
{
    struct pp_font_s *f = p;
    if (!f)
        return;
    fpp_font_destroy(&f->ff);
}

PP_Bool
ppb_font_is_font(PP_Resource resource)
{
    return PP_RESOURCE_FONT == pp_resource_get_type(resource);
}

PP_Bool
ppb_font_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                  struct PP_FontMetrics_Dev *metrics)
{
    struct pp_font_s *f = pp_resource_acquire(font, PP_RESOURCE_FONT);
    if (!f) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    PP_Bool ret = fpp_font_describe(&f->ff, description, metrics);
    pp_resource_release(font);
    return ret;
}

PP_Bool
ppb_font_draw_text_at(PP_Resource font, PP_Resource image_data, const struct PP_TextRun_Dev *text,
                      const struct PP_Point *position, uint32_t color, const struct PP_Rect *clip,
                      PP_Bool image_data_is_opaque)
{
    struct pp_font_s *f = pp_resource_acquire(font, PP_RESOURCE_FONT);
    if (!f) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

    PP_Bool ret = fpp_font_draw_text_at(&f->ff, image_data, text, position, color, clip,
                                        image_data_is_opaque);
    pp_resource_release(font);
    return ret;
}

int32_t
ppb_font_measure_text(PP_Resource font, const struct PP_TextRun_Dev *text)
{
    struct pp_font_s *f = pp_resource_acquire(font, PP_RESOURCE_FONT);
    if (!f) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_FAILED;
    }

    int32_t ret = fpp_font_measure_text(&f->ff, text);
    pp_resource_release(font);
    return ret;
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
    trace_info("[PPB] {full} %s instance=%d, description={.face=%s, .family=%u, .size=%u, "
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
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_font_is_font(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_font_describe(PP_Resource font, struct PP_FontDescription_Dev *description,
                        struct PP_FontMetrics_Dev *metrics)
{
    trace_info("[PPB] {full} %s font=%d\n", __func__+6, font);
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
    trace_info("[PPB] {full} %s font=%d, image_data=%d, text={.text=%s, .rtl=%u, "
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
    trace_info("[PPB] {full} %s font=%d, text={.text=%s, .rtl=%u, .override_direction=%u}\n",
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
    .Create =                   TWRAPF(ppb_font_create),
    .IsFont =                   TWRAPF(ppb_font_is_font),
    .Describe =                 TWRAPF(ppb_font_describe),
    .DrawTextAt =               TWRAPF(ppb_font_draw_text_at),
    .MeasureText =              TWRAPF(ppb_font_measure_text),
    .CharacterOffsetForPixel =  TWRAPZ(ppb_font_character_offset_for_pixel),
    .PixelOffsetForCharacter =  TWRAPZ(ppb_font_pixel_offset_for_character),
};

static
void
__attribute__((constructor))
constructor_ppb_font(void)
{
    register_interface(PPB_FONT_DEV_INTERFACE_0_6, &ppb_font_dev_interface_0_6);
    register_resource(PP_RESOURCE_FONT, ppb_font_destroy);
}
