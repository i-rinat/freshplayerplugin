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

#include "ppb_flash_font_file.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "ppb_var.h"
#include <tttables.h>
#include <arpa/inet.h>      // for htonl()


PP_Resource
ppb_flash_font_file_create(PP_Instance instance,
                           const struct PP_BrowserFont_Trusted_Description *description,
                           PP_PrivateFontCharset charset)
{
    // TODO: this code is copy-pasted from ppb_browser_font_trusted.
    //       Find a way to deduplicate
    PP_Resource font_file = pp_resource_allocate(PP_RESOURCE_FLASH_FONT_FILE, instance);
    struct pp_flash_font_file_s *fff = pp_resource_acquire(font_file, PP_RESOURCE_FLASH_FONT_FILE);
    PangoFontDescription *font_desc;

    if (description->face.type == PP_VARTYPE_STRING) {
        uint32_t len;
        const char *sn = ppb_var_var_to_utf8(description->face, &len);
        char *s = malloc(len + 1);

        memcpy(s, sn, len);
        s[len] = 0;
        font_desc = pango_font_description_from_string(s);
        free(s);
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

    fff->font = pango_context_load_font(tables_get_pango_ctx(), font_desc);
    pango_font_description_free(font_desc);
    fff->ft_face = pango_fc_font_lock_face(PANGO_FC_FONT(fff->font));

    pp_resource_release(font_file);
    return font_file;
}

void
ppb_flash_font_file_destroy(void *ptr)
{
    struct pp_flash_font_file_s *fff = ptr;

    pango_fc_font_unlock_face(PANGO_FC_FONT(fff->font));
    if (fff->font) {
        g_object_unref(fff->font);
        fff->font = NULL;
    }
}

PP_Bool
ppb_flash_font_file_is_flash_font_file(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_FLASH_FONT_FILE;
}

PP_Bool
ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                   uint32_t *output_length)
{
    table = htonl(table);
    PP_Bool retval = PP_FALSE;
    if (!output_length)
        return PP_FALSE;
    struct pp_flash_font_file_s *fff = pp_resource_acquire(font_file, PP_RESOURCE_FLASH_FONT_FILE);
    if (!fff)
        return PP_FALSE;

    FT_ULong len = 0;
    FT_Error ft_error = FT_Load_Sfnt_Table(fff->ft_face, table, 0, NULL, &len);

    if (output == NULL) {
        *output_length = len;
        retval = ft_error == 0 ? PP_TRUE : PP_FALSE;
    } else if (*output_length < len) {
        // not enough space
        retval = PP_FALSE;
    } else {
        ft_error = FT_Load_Sfnt_Table(fff->ft_face, table, 0, output, &len);
        retval = ft_error == 0 ? PP_TRUE : PP_FALSE;
    }

printf("=== *output_length = %u, error=%d\n", *output_length, ft_error);
    pp_resource_release(font_file);
    return retval;
}


// trace wrappers
static
PP_Resource
trace_ppb_flash_font_file_create(PP_Instance instance,
                                 const struct PP_BrowserFont_Trusted_Description *description,
                                 PP_PrivateFontCharset charset)
{
    char *s_face = trace_var_as_string(description->face);
    trace_info("[PPB] {full} %s instance=%d, description={.face=%s, .family=%d, .size=%u, "
               ".weight=%d, .italic=%u, .small_caps=%u, .letter_spacing=%d, .word_spacing=%d}, "
               ".charset=%d\n", __func__+6, instance, s_face, description->family,
               description->size, description->weight, description->italic, description->small_caps,
               description->letter_spacing, description->word_spacing, charset);
    free(s_face);
    return ppb_flash_font_file_create(instance, description, charset);
}

static
PP_Bool
trace_ppb_flash_font_file_is_flash_font_file(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_flash_font_file_is_flash_font_file(resource);
}

static
PP_Bool
trace_ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                         uint32_t *output_length)
{
    trace_info("[PPB] {full} %s font_file=%d, table=%c%c%c%c, output=%p\n", __func__+6, font_file,
               table & 0xffu, (table >> 8) & 0xffu, (table >> 16) & 0xffu, (table >> 24) & 0xffu,
               output);
    return ppb_flash_font_file_get_font_table(font_file, table, output, output_length);
}


const struct PPB_Flash_FontFile_0_1 ppb_flash_font_file_interface_0_1 = {
    .Create =           trace_ppb_flash_font_file_create,
    .IsFlashFontFile =  trace_ppb_flash_font_file_is_flash_font_file,
    .GetFontTable =     trace_ppb_flash_font_file_get_font_table,
};
