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

#include "ppb_pdf.h"
#include <stdlib.h>
#include <libgen.h>
#include <glib.h>
#include "trace.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include "ppb_flash_font_file.h"
#include "ppb_image_data.h"
#include "ppb_var.h"
#include "np_entry.h"
#include "pp_interface.h"


static GMappedFile  *natives_blob = NULL;
static GMappedFile  *snapshot_blob = NULL;


static
void
__attribute__((destructor))
destructor_ppb_pdf(void)
{
    if (natives_blob) {
        g_mapped_file_unref(natives_blob);
        natives_blob = NULL;
    }
    if (snapshot_blob) {
        g_mapped_file_unref(snapshot_blob);
        snapshot_blob = NULL;
    }
}

PP_Resource
ppb_pdf_get_font_file_with_fallback(PP_Instance instance,
                                    const struct PP_BrowserFont_Trusted_Description *description,
                                    PP_PrivateFontCharset charset)
{
    return ppb_flash_font_file_create(instance, description, charset);
}

bool
ppb_pdf_get_font_table_for_private_font_file(PP_Resource font_file, uint32_t table, void *output,
                                             uint32_t *output_length)
{
    return ppb_flash_font_file_get_font_table(font_file, table, output, output_length);
}

void
ppb_pdf_search_string(PP_Instance instance, const unsigned short *string,
                      const unsigned short *term, bool case_sensitive,
                      struct PP_PrivateFindResult **results, int *count)
{
}

void
ppb_pdf_did_start_loading(PP_Instance instance)
{
}

void
ppb_pdf_did_stop_loading(PP_Instance instance)
{
}

void
ppb_pdf_set_content_restriction(PP_Instance instance, int restrictions)
{
}

void
ppb_pdf_user_metrics_record_action(PP_Instance instance, struct PP_Var action)
{
}

void
ppb_pdf_has_unsupported_feature(PP_Instance instance)
{
}

void
ppb_pdf_save_as(PP_Instance instance)
{
}

void
ppb_pdf_print(PP_Instance instance)
{
}

PP_Bool
ppb_pdf_is_feature_enabled(PP_Instance instance, PP_PDFFeature feature)
{
    return false;
}

void
ppb_pdf_set_selected_text(PP_Instance instance, const char *selected_text)
{
}

void
ppb_pdf_set_link_under_cursor(PP_Instance instance, const char *url)
{
}

void
ppb_pdf_get_v8_external_snapshot_data(PP_Instance instance, const char **natives_data_out,
                                      int *natives_size_out, const char **snapshot_data_out,
                                      int *snapshot_size_out)
{
    if (!natives_blob || !snapshot_blob) {
        gchar *tmp = g_strdup(np_entry_get_module_file_name());
        gchar *module_dir = dirname(tmp);
        gchar *natives_path = g_strdup_printf("%s/natives_blob.bin", module_dir);
        gchar *snapshot_path = g_strdup_printf("%s/snapshot_blob.bin", module_dir);

        natives_blob = g_mapped_file_new(natives_path, FALSE, NULL);
        snapshot_blob = g_mapped_file_new(snapshot_path, FALSE, NULL);

        g_free(tmp);
        g_free(natives_path);
        g_free(snapshot_path);
    }

    *natives_data_out = natives_blob ? g_mapped_file_get_contents(natives_blob) : NULL;
    *natives_size_out = natives_blob ? g_mapped_file_get_length(natives_blob) : 0;

    *snapshot_data_out = snapshot_blob ? g_mapped_file_get_contents(snapshot_blob) : NULL;
    *snapshot_size_out = snapshot_blob ? g_mapped_file_get_length(snapshot_blob) : 0;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_pdf_get_font_file_with_fallback(PP_Instance instance,
                                          const struct PP_BrowserFont_Trusted_Description
                                                                                    *description,
                                          PP_PrivateFontCharset charset)
{
    gchar *s_face = trace_var_as_string(description->face);
    trace_info("[PPB] {full} %s instance=%d, description={.face=%s, .family=%u, .size=%u, "
               ".weight=%u, .italic=%u, .small_caps=%u, .letter_spacing=%d, .word_spacing=%d}, "
               "charset=%s(%u)\n",
               __func__+6, instance, s_face, description->family, description->size,
               description->weight, description->italic, description->small_caps,
               description->letter_spacing, description->word_spacing,
               reverse_private_font_charset(charset), charset);
    g_free(s_face);
    return ppb_pdf_get_font_file_with_fallback(instance, description, charset);
}

TRACE_WRAPPER
bool
trace_ppb_pdf_get_font_table_for_private_font_file(PP_Resource font_file, uint32_t table,
                                                   void *output, uint32_t *output_length)
{
    trace_info("[PPB] {full} %s font_file=%d, table=0x%08x\n", __func__+6, font_file, table);
    return ppb_pdf_get_font_table_for_private_font_file(font_file, table, output, output_length);
}

TRACE_WRAPPER
void
trace_ppb_pdf_search_string(PP_Instance instance, const unsigned short *string,
                            const unsigned short *term, bool case_sensitive,
                            struct PP_PrivateFindResult **results, int *count)
{
    trace_info("[PPB] {zilch} %s instance=%d, string=%p, term=%p, case_sensitive=%u\n", __func__+6,
               instance, string, term, case_sensitive);
    ppb_pdf_search_string(instance, string, term, case_sensitive, results, count);
}

TRACE_WRAPPER
void
trace_ppb_pdf_did_start_loading(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_did_start_loading(instance);
}

TRACE_WRAPPER
void
trace_ppb_pdf_did_stop_loading(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_did_stop_loading(instance);
}

TRACE_WRAPPER
void
trace_ppb_pdf_set_content_restriction(PP_Instance instance, int restrictions)
{
    trace_info("[PPB] {zilch} %s instance=%d, restrictions=%d\n", __func__+6, instance,
               restrictions);
    ppb_pdf_set_content_restriction(instance, restrictions);
}

TRACE_WRAPPER
void
trace_ppb_pdf_user_metrics_record_action(PP_Instance instance, struct PP_Var action)
{
    gchar *s_action = trace_var_as_string(action);
    trace_info("[PPB] {zilch} %s instance=%d, action=%s\n", __func__+6, instance, s_action);
    g_free(s_action);
    ppb_pdf_user_metrics_record_action(instance, action);
}

TRACE_WRAPPER
void
trace_ppb_pdf_has_unsupported_feature(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_has_unsupported_feature(instance);
}

TRACE_WRAPPER
void
trace_ppb_pdf_save_as(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_save_as(instance);
}

TRACE_WRAPPER
void
trace_ppb_pdf_print(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_print(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_pdf_is_feature_enabled(PP_Instance instance, PP_PDFFeature feature)
{
    trace_info("[PPB] {zilch} %s instance=%d, feature=%s(%u)\n", __func__+6, instance,
               reverse_pdf_feature(feature), feature);
    return ppb_pdf_is_feature_enabled(instance, feature);
}

TRACE_WRAPPER
void
trace_ppb_pdf_set_selected_text(PP_Instance instance, const char *selected_text)
{
    trace_info("[PPB] {zilch} %s instance=%d, selected_text=%s\n", __func__+6, instance,
               selected_text);
    ppb_pdf_set_selected_text(instance, selected_text);
}

TRACE_WRAPPER
void
trace_ppb_pdf_set_link_under_cursor(PP_Instance instance, const char *url)
{
    trace_info("[PPB] {zilch} %s instance=%d, url=%s\n", __func__+6, instance, url);
    ppb_pdf_set_link_under_cursor(instance, url);
}

TRACE_WRAPPER
void
trace_ppb_pdf_get_v8_external_snapshot_data(PP_Instance instance, const char **natives_data_out,
                                            int *natives_size_out, const char **snapshot_data_out,
                                            int *snapshot_size_out)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_pdf_get_v8_external_snapshot_data(instance, natives_data_out, natives_size_out,
                                          snapshot_data_out, snapshot_size_out);
}


const struct PPB_PDF ppb_pdf_interface = {
    .GetFontFileWithFallback =        TWRAPF(ppb_pdf_get_font_file_with_fallback),
    .GetFontTableForPrivateFontFile = TWRAPF(ppb_pdf_get_font_table_for_private_font_file),
    .SearchString =                   TWRAPZ(ppb_pdf_search_string),
    .DidStartLoading =                TWRAPZ(ppb_pdf_did_start_loading),
    .DidStopLoading =                 TWRAPZ(ppb_pdf_did_stop_loading),
    .SetContentRestriction =          TWRAPZ(ppb_pdf_set_content_restriction),
    .UserMetricsRecordAction =        TWRAPZ(ppb_pdf_user_metrics_record_action),
    .HasUnsupportedFeature =          TWRAPZ(ppb_pdf_has_unsupported_feature),
    .SaveAs =                         TWRAPZ(ppb_pdf_save_as),
    .Print =                          TWRAPZ(ppb_pdf_print),
    .IsFeatureEnabled =               TWRAPZ(ppb_pdf_is_feature_enabled),
    .SetSelectedText =                TWRAPZ(ppb_pdf_set_selected_text),
    .SetLinkUnderCursor =             TWRAPZ(ppb_pdf_set_link_under_cursor),
    .GetV8ExternalSnapshotData =      TWRAPF(ppb_pdf_get_v8_external_snapshot_data),
};

static
void
__attribute__((constructor))
constructor_ppb_pdf(void)
{
    register_interface(PPB_PDF_INTERFACE, &ppb_pdf_interface);
}
