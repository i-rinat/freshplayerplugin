// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PPAPI_C_PRIVATE_PPB_PDF_H_
#define PPAPI_C_PRIVATE_PPB_PDF_H_

#include "ppapi/c/dev/ppb_font_dev.h"
#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/private/pp_private_font_charset.h"

#define PPB_PDF_INTERFACE "PPB_PDF;1"

// From the public PPB_Font_Dev file.
struct PP_FontDescription_Dev;

typedef enum {
  PP_RESOURCESTRING_PDFGETPASSWORD = 0,
  PP_RESOURCESTRING_PDFLOADING = 1,
  PP_RESOURCESTRING_PDFLOAD_FAILED = 2,
  PP_RESOURCESTRING_PDFPROGRESSLOADING = 3
} PP_ResourceString;

typedef enum {
  PP_RESOURCEIMAGE_PDF_BUTTON_FTP = 0,
  PP_RESOURCEIMAGE_PDF_BUTTON_FTP_HOVER = 1,
  PP_RESOURCEIMAGE_PDF_BUTTON_FTP_PRESSED = 2,
  PP_RESOURCEIMAGE_PDF_BUTTON_FTW = 3,
  PP_RESOURCEIMAGE_PDF_BUTTON_FTW_HOVER = 4,
  PP_RESOURCEIMAGE_PDF_BUTTON_FTW_PRESSED = 5,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN = 6,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_HOVER = 7,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_PRESSED = 8,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT = 9,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_HOVER = 10,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_PRESSED = 11,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_0 = 12,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_1 = 13,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_2 = 14,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_3 = 15,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_4 = 16,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_5 = 17,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_6 = 18,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_7 = 19,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_8 = 20,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_9 = 21,
  PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_NUM_BACKGROUND = 22,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_0 = 23,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_1 = 24,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_2 = 25,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_3 = 26,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_4 = 27,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_5 = 28,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_6 = 29,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_7 = 30,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_8 = 31,
  PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_BACKGROUND = 32,
  PP_RESOURCEIMAGE_PDF_PAGE_DROPSHADOW = 33,
  PP_RESOURCEIMAGE_PDF_BUTTON_SAVE = 34,
  PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_HOVER = 35,
  PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_PRESSED = 36,
  PP_RESOURCEIMAGE_PDF_BUTTON_PRINT = 37,
  PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_HOVER = 38,
  PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_PRESSED = 39,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END = 40,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_HOVER = 41,
  PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_PRESSED = 42,
  PP_RESOURCEIMAGE_PDF_PAN_SCROLL_ICON = 43,
  PP_RESOURCEIMAGE_PDF_PAGE_INDICATOR_BACKGROUND = 44,
  PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_DISABLED = 45
} PP_ResourceImage;

typedef enum {
  PP_PDFFEATURE_HIDPI = 0,
  PP_PDFFEATURE_PRINTING = 1
} PP_PDFFeature;

struct PP_PrivateFontFileDescription {
  const char* face;
  uint32_t weight;
  bool italic;
};

struct PP_PrivateFindResult {
  int start_index;
  int length;
};

struct PPB_PDF {
  // Returns a localized string.
  struct PP_Var (*GetLocalizedString)(PP_Instance instance,
                                      PP_ResourceString string_id);

  // Returns a resource image.
  PP_Resource (*GetResourceImage)(PP_Instance instance,
                                  PP_ResourceImage image_id);

  // Returns a resource identifying a font file corresponding to the given font
  // request after applying the browser-specific fallback.
  //
  // Currently Linux-only.
  PP_Resource (*GetFontFileWithFallback)(
      PP_Instance instance,
      const struct PP_BrowserFont_Trusted_Description* description,
      PP_PrivateFontCharset charset);

  // Given a resource previously returned by GetFontFileWithFallback, returns
  // a pointer to the requested font table. Linux only.
  bool (*GetFontTableForPrivateFontFile)(PP_Resource font_file,
                                         uint32_t table,
                                         void* output,
                                         uint32_t* output_length);

  // Search the given string using ICU.  Use PPB_Core's MemFree on results when
  // done.
  void (*SearchString)(
     PP_Instance instance,
     const unsigned short* string,
     const unsigned short* term,
     bool case_sensitive,
     struct PP_PrivateFindResult** results,
     int* count);

  // Since WebFrame doesn't know about PPAPI requests, it'll think the page has
  // finished loading even if there are outstanding requests by the plugin.
  // Take this out once WebFrame knows about requests by PPAPI plugins.
  void (*DidStartLoading)(PP_Instance instance);
  void (*DidStopLoading)(PP_Instance instance);

  // Sets content restriction for a full-page plugin (i.e. can't copy/print).
  // The value is a bitfield of ContentRestriction enums.
  void (*SetContentRestriction)(PP_Instance instance, int restrictions);

  // Use UMA so we know average pdf page count.
  void (*HistogramPDFPageCount)(PP_Instance instance, int count);

  // Notifies the browser that the given action has been performed.
  void (*UserMetricsRecordAction)(PP_Instance instance, struct PP_Var action);

  // Notifies the browser that the PDF has an unsupported feature.
  void (*HasUnsupportedFeature)(PP_Instance instance);

  // Invoke SaveAs... dialog, similar to the right-click or wrench menu.
  void (*SaveAs)(PP_Instance instance);

  // Invoke Print dialog for plugin.
  void (*Print)(PP_Instance instance);

  PP_Bool(*IsFeatureEnabled)(PP_Instance instance, PP_PDFFeature feature);

  // Returns a resource image appropriate for a device with |scale| density.
  // Returns 0 (NULL resource) if there is no resource at that scale
  PP_Resource (*GetResourceImageForScale)(PP_Instance instance,
                                          PP_ResourceImage image_id,
                                          float scale);

  // Invoke password dialog for plugin.
  struct PP_Var (*ModalPromptForPassword)(PP_Instance instance,
                                          struct PP_Var message);

  // Returns PP_TRUE if the plugin is out of process.
  PP_Bool (*IsOutOfProcess)(PP_Instance instance);

  // Sets the selected text of the plugin.
  void(*SetSelectedText)(PP_Instance instance, const char* selected_text);

  // Sets the link currently under the cursor.
  void (*SetLinkUnderCursor)(PP_Instance instance, const char* url);
};

#endif  // PPAPI_C_PRIVATE_PPB_PDF_H_
