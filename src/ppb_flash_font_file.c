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
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_flash_font_file.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include <arpa/inet.h>  // for htonl()
#include <glib.h>
#include <pango/pangoft2.h>
#include <stdlib.h>

#include <ft2build.h>
#include FT_TRUETYPE_TABLES_H

struct pp_flash_font_file_s {
    COMMON_STRUCTURE_FIELDS
    PangoFont              *font;
    FT_Face                 ft_face;
    PP_PrivateFontCharset   charset;
};

STATIC_ASSERT(sizeof(struct pp_flash_font_file_s) <= LARGEST_RESOURCE_SIZE);

PP_Resource
ppb_flash_font_file_create(PP_Instance instance,
                           const struct PP_BrowserFont_Trusted_Description *description,
                           PP_PrivateFontCharset charset)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource font_file = pp_resource_allocate(PP_RESOURCE_FLASH_FONT_FILE, pp_i);
    struct pp_flash_font_file_s *fff = pp_resource_acquire(font_file, PP_RESOURCE_FLASH_FONT_FILE);
    if (!fff) {
        trace_error("%s, resource allocation error\n", __func__);
        return 0;
    }
    PangoFontDescription *font_desc = pp_browser_font_desc_to_pango_font_desc(description);

    fff->font = pango_context_load_font(tables_get_pango_ctx(), font_desc);
    pango_font_description_free(font_desc);
    fff->ft_face = pango_fc_font_lock_face(PANGO_FC_FONT(fff->font));

    pp_resource_release(font_file);
    return font_file;
}

static
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
    if (!fff) {
        trace_error("%s, bad resource\n", __func__);
        return PP_FALSE;
    }

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

    if (ft_error != 0) {
        trace_info_f("               ft_error=0x%02x\n", ft_error);
    }

    pp_resource_release(font_file);
    return retval;
}

PP_Bool
ppb_flash_font_file_is_supported_for_windows(void)
{
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
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
    g_free(s_face);
    return ppb_flash_font_file_create(instance, description, charset);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_font_file_is_flash_font_file(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_flash_font_file_is_flash_font_file(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_font_file_get_font_table(PP_Resource font_file, uint32_t table, void *output,
                                         uint32_t *output_length)
{
    trace_info("[PPB] {full} %s font_file=%d, table=%c%c%c%c, output=%p\n", __func__+6, font_file,
               table & 0xffu, (table >> 8) & 0xffu, (table >> 16) & 0xffu, (table >> 24) & 0xffu,
               output);
    return ppb_flash_font_file_get_font_table(font_file, table, output, output_length);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_font_file_is_supported_for_windows(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_flash_font_file_is_supported_for_windows();
}


const struct PPB_Flash_FontFile_0_1 ppb_flash_font_file_interface_0_1 = {
    .Create =           TWRAPF(ppb_flash_font_file_create),
    .IsFlashFontFile =  TWRAPF(ppb_flash_font_file_is_flash_font_file),
    .GetFontTable =     TWRAPF(ppb_flash_font_file_get_font_table),
};

const struct PPB_Flash_FontFile_0_2 ppb_flash_font_file_interface_0_2 = {
    .Create =           TWRAPF(ppb_flash_font_file_create),
    .IsFlashFontFile =  TWRAPF(ppb_flash_font_file_is_flash_font_file),
    .GetFontTable =     TWRAPF(ppb_flash_font_file_get_font_table),
    .IsSupportedForWindows = TWRAPF(ppb_flash_font_file_is_supported_for_windows),
};

static
void
__attribute__((constructor))
constructor_ppb_flash_font_file(void)
{
    register_interface(PPB_FLASH_FONTFILE_INTERFACE_0_1, &ppb_flash_font_file_interface_0_1);
    register_interface(PPB_FLASH_FONTFILE_INTERFACE_0_2, &ppb_flash_font_file_interface_0_2);
    register_resource(PP_RESOURCE_FLASH_FONT_FILE, ppb_flash_font_file_destroy);
}
