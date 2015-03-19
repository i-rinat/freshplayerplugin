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

#ifndef FPP_PPB_PDF_H
#define FPP_PPB_PDF_H

#include <ppapi/c/dev/deprecated_bool.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <ppapi/c/private/ppb_pdf.h>


struct PP_Var
ppb_pdf_get_localized_string(PP_Instance instance, PP_ResourceString string_id);

PP_Resource
ppb_pdf_get_resource_image(PP_Instance instance, PP_ResourceImage image_id);

PP_Resource
ppb_pdf_get_font_file_with_fallback(PP_Instance instance,
                                    const struct PP_BrowserFont_Trusted_Description *description,
                                    PP_PrivateFontCharset charset);

bool
ppb_pdf_get_font_table_for_private_font_file(PP_Resource font_file, uint32_t table, void *output,
                                             uint32_t *output_length);

void
ppb_pdf_search_string(PP_Instance instance, const unsigned short *string,
                      const unsigned short *term, bool case_sensitive,
                      struct PP_PrivateFindResult **results, int *count);

void
ppb_pdf_did_start_loading(PP_Instance instance);

void
ppb_pdf_did_stop_loading(PP_Instance instance);

void
ppb_pdf_set_content_restriction(PP_Instance instance, int restrictions);

void
ppb_pdf_histogram_p_d_f_page_count(PP_Instance instance, int count);

void
ppb_pdf_user_metrics_record_action(PP_Instance instance, struct PP_Var action);

void
ppb_pdf_has_unsupported_feature(PP_Instance instance);

void
ppb_pdf_save_as(PP_Instance instance);

void
ppb_pdf_print(PP_Instance instance);

PP_Bool
ppb_pdf_is_feature_enabled(PP_Instance instance, PP_PDFFeature feature);

PP_Resource
ppb_pdf_get_resource_image_for_scale(PP_Instance instance, PP_ResourceImage image_id, float scale);

struct PP_Var
ppb_pdf_modal_prompt_for_password(PP_Instance instance, struct PP_Var message);

PP_Bool
ppb_pdf_is_out_of_process(PP_Instance instance);

void
ppb_pdf_set_selected_text(PP_Instance instance, const char *selected_text);

void
ppb_pdf_set_link_under_cursor(PP_Instance instance, const char *url);

void
ppb_pdf_get_v8_external_snapshot_data(PP_Instance instance, const char **natives_data_out,
                                      int *natives_size_out, const char **snapshot_data_out,
                                      int *snapshot_size_out);

#endif // FPP_PPB_PDF_H
