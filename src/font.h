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

#ifndef FPP_FONT_H
#define FPP_FONT_H

#include <ppapi/c/dev/ppb_font_dev.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <pango/pango.h>


struct fpp_font {
    PangoFont              *font;
    PangoFontDescription   *font_desc;
    PangoLayout            *measure_layout;
    int32_t                 letter_spacing;
    int32_t                 word_spacing;
    int32_t                 family;
};


struct PP_Var
fpp_font_get_font_families(void);

void
fpp_font_init(struct fpp_font *ff, const struct PP_FontDescription_Dev *description);

void
fpp_font_destroy(struct fpp_font *ff);

PP_Bool
fpp_font_describe(struct fpp_font *ff, struct PP_FontDescription_Dev *description,
                  struct PP_FontMetrics_Dev *metrics);

PP_Bool
fpp_font_draw_text_at(struct fpp_font *ff, PP_Resource image_data,
                      const struct PP_TextRun_Dev *text, const struct PP_Point *position,
                      uint32_t color, const struct PP_Rect *clip, PP_Bool image_data_is_opaque);

int32_t
fpp_font_measure_text(struct fpp_font *ff, const struct PP_TextRun_Dev *text);

uint32_t
fpp_font_character_offset_for_pixel(struct fpp_font *ff, const struct PP_TextRun_Dev *text,
                                    int32_t pixel_position);

int32_t
fpp_font_pixel_offset_for_character(struct fpp_font *ff, const struct PP_TextRun_Dev *text,
                                    uint32_t char_offset);

PangoFontDescription *
pp_font_desc_to_pango_font_desc(const struct PP_FontDescription_Dev *description);

struct PP_FontDescription_Dev
pp_browser_font_desc_to_pp_font_desc(const struct PP_BrowserFont_Trusted_Description *descr);

PangoFontDescription *
pp_browser_font_desc_to_pango_font_desc(const struct PP_BrowserFont_Trusted_Description *descr);


#endif // FPP_FONT_H
