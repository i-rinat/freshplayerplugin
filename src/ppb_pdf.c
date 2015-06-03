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
#include "img_resources.h"
#include "pp_interface.h"


static GMappedFile  *natives_blob = NULL;
static GMappedFile  *snapshot_blob = NULL;
static GHashTable   *resource_images_ht = NULL;


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
    if (resource_images_ht) {
        g_hash_table_unref(resource_images_ht);
        resource_images_ht = NULL;
    }
}

struct PP_Var
ppb_pdf_get_localized_string(PP_Instance instance, PP_ResourceString string_id)
{
    // TODO: localization?
    switch (string_id) {
    case PP_RESOURCESTRING_PDFGETPASSWORD:
        return ppb_var_var_from_utf8_z("Need password");
    case PP_RESOURCESTRING_PDFLOADING:
        return ppb_var_var_from_utf8_z("Loading...");
    case PP_RESOURCESTRING_PDFLOAD_FAILED:
        return ppb_var_var_from_utf8_z("Load failed");
    case PP_RESOURCESTRING_PDFPROGRESSLOADING:
        return ppb_var_var_from_utf8_z("Load progress");
    default:
        return ppb_var_var_from_utf8_z("");
    }
}

struct data_reader_ctx {
    size_t      len;
    size_t      pos;
    const char *data;
};

cairo_status_t
data_reader_func(void *closure, unsigned char *data, unsigned int len)
{
    struct data_reader_ctx *ctx = closure;
    if (len > ctx->len - ctx->pos) {
        trace_error("%s, invalid png in resources\n", __func__);
        return CAIRO_STATUS_READ_ERROR;
    }

    memcpy(data, ctx->data + ctx->pos, len);
    ctx->pos += len;

    return CAIRO_STATUS_SUCCESS;
}

PP_Resource
create_image_from_resource(PP_Instance instance, const char *name)
{
    gpointer val;
    if (!g_hash_table_lookup_extended(resource_images_ht, name, NULL, &val))
        return 0;

    const uintptr_t idx = GPOINTER_TO_SIZE(val);
    struct data_reader_ctx ctx = {
        .len = resource_image[idx].len,
        .pos = 0,
        .data = resource_image[idx].data
    };
    cairo_surface_t *surf = cairo_image_surface_create_from_png_stream(data_reader_func, &ctx);
    struct PP_Size image_size = {
        .width = cairo_image_surface_get_width(surf),
        .height = cairo_image_surface_get_height(surf)
    };
    PP_Resource image_data = ppb_image_data_create(instance, PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                                                   &image_size, PP_TRUE);
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (id) {
        cairo_t *cr = cairo_create(id->cairo_surf);
        cairo_set_source_surface(cr, surf, 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_paint(cr);
        cairo_surface_flush(id->cairo_surf);
        cairo_destroy(cr);
        pp_resource_release(image_data);
    }

    cairo_surface_destroy(surf);
    return image_data;
}

PP_Resource
ppb_pdf_get_resource_image(PP_Instance instance, PP_ResourceImage image_id)
{
    switch (image_id) {
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTP:
        return create_image_from_resource(instance, "pdf_button_ftp.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTP_HOVER:
        return create_image_from_resource(instance, "pdf_button_ftp_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTP_PRESSED:
        return create_image_from_resource(instance, "pdf_button_ftp_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTW:
        return create_image_from_resource(instance, "pdf_button_ftw.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTW_HOVER:
        return create_image_from_resource(instance, "pdf_button_ftw_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_FTW_PRESSED:
        return create_image_from_resource(instance, "pdf_button_ftw_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN:
        return create_image_from_resource(instance, "pdf_button_zoomin.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_HOVER:
        return create_image_from_resource(instance, "pdf_button_zoomin_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_PRESSED:
        return create_image_from_resource(instance, "pdf_button_zoomin_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT:
        return create_image_from_resource(instance, "pdf_button_zoomout.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_HOVER:
        return create_image_from_resource(instance, "pdf_button_zoomout_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_PRESSED:
        return create_image_from_resource(instance, "pdf_button_zoomout_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_0:
        return create_image_from_resource(instance, "pdf_thumbnail_0.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_1:
        return create_image_from_resource(instance, "pdf_thumbnail_1.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_2:
        return create_image_from_resource(instance, "pdf_thumbnail_2.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_3:
        return create_image_from_resource(instance, "pdf_thumbnail_3.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_4:
        return create_image_from_resource(instance, "pdf_thumbnail_4.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_5:
        return create_image_from_resource(instance, "pdf_thumbnail_5.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_6:
        return create_image_from_resource(instance, "pdf_thumbnail_6.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_7:
        return create_image_from_resource(instance, "pdf_thumbnail_7.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_8:
        return create_image_from_resource(instance, "pdf_thumbnail_8.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_9:
        return create_image_from_resource(instance, "pdf_thumbnail_9.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_NUM_BACKGROUND:
        return create_image_from_resource(instance, "pdf_thumbnail_num_background.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_0:
        return create_image_from_resource(instance, "pdf_progress_0.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_1:
        return create_image_from_resource(instance, "pdf_progress_1.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_2:
        return create_image_from_resource(instance, "pdf_progress_2.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_3:
        return create_image_from_resource(instance, "pdf_progress_3.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_4:
        return create_image_from_resource(instance, "pdf_progress_4.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_5:
        return create_image_from_resource(instance, "pdf_progress_5.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_6:
        return create_image_from_resource(instance, "pdf_progress_6.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_7:
        return create_image_from_resource(instance, "pdf_progress_7.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_8:
        return create_image_from_resource(instance, "pdf_progress_8.png");
    case PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_BACKGROUND:
        return create_image_from_resource(instance, "pdf_progress_background.png");
    case PP_RESOURCEIMAGE_PDF_PAGE_DROPSHADOW:
        return create_image_from_resource(instance, "pdf_dropshadow.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_SAVE:
        return create_image_from_resource(instance, "pdf_button_save.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_HOVER:
        return create_image_from_resource(instance, "pdf_button_save_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_PRESSED:
        return create_image_from_resource(instance, "pdf_button_save_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_PRINT:
        return create_image_from_resource(instance, "pdf_button_print.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_HOVER:
        return create_image_from_resource(instance, "pdf_button_print_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_PRESSED:
        return create_image_from_resource(instance, "pdf_button_print_pressed.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END:
        return create_image_from_resource(instance, "pdf_button_zoomin_end.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_HOVER:
        return create_image_from_resource(instance, "pdf_button_zoomin_end_hover.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_PRESSED:
        return create_image_from_resource(instance, "pdf_button_zoomin_end_pressed.png");
    case PP_RESOURCEIMAGE_PDF_PAN_SCROLL_ICON:
        return create_image_from_resource(instance, "pan_icon.png");
    case PP_RESOURCEIMAGE_PDF_PAGE_INDICATOR_BACKGROUND:
        return create_image_from_resource(instance, "pdf_page_indicator_background.png");
    case PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_DISABLED:
        return create_image_from_resource(instance, "pdf_button_print_disabled.png");
    default:
        return create_image_from_resource(instance, "");
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
ppb_pdf_histogram_pdf_page_count(PP_Instance instance, int count)
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

PP_Resource
ppb_pdf_get_resource_image_for_scale(PP_Instance instance, PP_ResourceImage image_id, float scale)
{
    return 0;
}

struct PP_Var
ppb_pdf_modal_prompt_for_password(PP_Instance instance, struct PP_Var message)
{
    return PP_MakeUndefined();
}

PP_Bool
ppb_pdf_is_out_of_process(PP_Instance instance)
{
    // TODO: does it matter? Chromium 41 seems to have it hardcoded to PP_FALSE
    return PP_FALSE;
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
struct PP_Var
trace_ppb_pdf_get_localized_string(PP_Instance instance, PP_ResourceString string_id)
{
    trace_info("[PPB] {full} %s instance=%d, string_id=%s(%u)\n", __func__+6, instance,
               reverse_resource_string(string_id), string_id);
    return ppb_pdf_get_localized_string(instance, string_id);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_pdf_get_resource_image(PP_Instance instance, PP_ResourceImage image_id)
{
    trace_info("[PPB] {full} %s instance=%d, image_id=%s(%u)\n", __func__+6, instance,
               reverse_resource_image(image_id), image_id);
    return ppb_pdf_get_resource_image(instance, image_id);
}

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
trace_ppb_pdf_histogram_pdf_page_count(PP_Instance instance, int count)
{
    trace_info("[PPB] {zilch} %s instance=%d, count=%d\n", __func__+6, instance, count);
    ppb_pdf_histogram_pdf_page_count(instance, count);
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
PP_Resource
trace_ppb_pdf_get_resource_image_for_scale(PP_Instance instance, PP_ResourceImage image_id,
                                           float scale)
{
    trace_info("[PPB] {zilch} %s instance=%d, image_id=%s(%u), scale=%f\n", __func__+6, instance,
               reverse_resource_image(image_id), image_id, scale);
    return ppb_pdf_get_resource_image_for_scale(instance, image_id, scale);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_pdf_modal_prompt_for_password(PP_Instance instance, struct PP_Var message)
{
    gchar *s_message = trace_var_as_string(message);
    trace_info("[PPB] {zilch} %s instance=%d, message=%s\n", __func__+6, instance, s_message);
    g_free(s_message);
    return ppb_pdf_modal_prompt_for_password(instance, message);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_pdf_is_out_of_process(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_pdf_is_out_of_process(instance);
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
    .GetLocalizedString =             TWRAPF(ppb_pdf_get_localized_string),
    .GetResourceImage =               TWRAPF(ppb_pdf_get_resource_image),
    .GetFontFileWithFallback =        TWRAPF(ppb_pdf_get_font_file_with_fallback),
    .GetFontTableForPrivateFontFile = TWRAPF(ppb_pdf_get_font_table_for_private_font_file),
    .SearchString =                   TWRAPZ(ppb_pdf_search_string),
    .DidStartLoading =                TWRAPZ(ppb_pdf_did_start_loading),
    .DidStopLoading =                 TWRAPZ(ppb_pdf_did_stop_loading),
    .SetContentRestriction =          TWRAPZ(ppb_pdf_set_content_restriction),
    .HistogramPDFPageCount =          TWRAPZ(ppb_pdf_histogram_pdf_page_count),
    .UserMetricsRecordAction =        TWRAPZ(ppb_pdf_user_metrics_record_action),
    .HasUnsupportedFeature =          TWRAPZ(ppb_pdf_has_unsupported_feature),
    .SaveAs =                         TWRAPZ(ppb_pdf_save_as),
    .Print =                          TWRAPZ(ppb_pdf_print),
    .IsFeatureEnabled =               TWRAPZ(ppb_pdf_is_feature_enabled),
    .GetResourceImageForScale =       TWRAPZ(ppb_pdf_get_resource_image_for_scale),
    .ModalPromptForPassword =         TWRAPZ(ppb_pdf_modal_prompt_for_password),
    .IsOutOfProcess =                 TWRAPF(ppb_pdf_is_out_of_process),
    .SetSelectedText =                TWRAPZ(ppb_pdf_set_selected_text),
    .SetLinkUnderCursor =             TWRAPZ(ppb_pdf_set_link_under_cursor),
    .GetV8ExternalSnapshotData =      TWRAPF(ppb_pdf_get_v8_external_snapshot_data),
};

static
void
__attribute__((constructor))
constructor_ppb_pdf(void)
{
    resource_images_ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    for (uintptr_t k = 0; k < resource_image_count; k ++) {
        g_hash_table_replace(resource_images_ht, g_strdup(resource_image[k].name),
                             GSIZE_TO_POINTER(k));
    }

    register_interface(PPB_PDF_INTERFACE, &ppb_pdf_interface);
}
