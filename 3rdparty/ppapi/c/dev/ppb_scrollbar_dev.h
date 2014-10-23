/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppb_scrollbar_dev.idl modified Wed Oct  5 14:06:02 2011. */

#ifndef PPAPI_C_DEV_PPB_SCROLLBAR_DEV_H_
#define PPAPI_C_DEV_PPB_SCROLLBAR_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_point.h"
#include "ppapi/c/pp_rect.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_size.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_SCROLLBAR_DEV_INTERFACE_0_5 "PPB_Scrollbar(Dev);0.5"
#define PPB_SCROLLBAR_DEV_INTERFACE PPB_SCROLLBAR_DEV_INTERFACE_0_5

/**
 * @file
 * This file defines the <code>PPB_Buffer_Dev</code> interface.
 */


/**
 * @addtogroup Enums
 * @{
 */
typedef enum {
  PP_SCROLLBY_PIXEL = 0,
  PP_SCROLLBY_LINE = 1,
  PP_SCROLLBY_PAGE = 2,
  PP_SCROLLBY_DOCUMENT = 3
} PP_ScrollBy_Dev;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_ScrollBy_Dev, 4);
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The interface for a scrollbar.  A scrollbar is a widget, so the functions
 * in PPB_Widget can also be used with scrollbar objects.
 */
struct PPB_Scrollbar_Dev_0_5 {
  /**
   * Create a new scrollbar.  Returns 0 if the instance is invalid.
   */
  PP_Resource (*Create)(PP_Instance instance, PP_Bool vertical);
  /**
   * Returns PP_TRUE if the given resource is a Scrollbar. Returns PP_FALSE if
   * the resource is invalid or some type other than a scrollbar.
   */
  PP_Bool (*IsScrollbar)(PP_Resource resource);
  /**
   * Gets the thickness of a scrollbar.
   */
  uint32_t (*GetThickness)(PP_Resource resource);
  /**
   * Returns PP_TRUE if the system scrollbar style is an overlap scrollbar.
   */
  PP_Bool (*IsOverlay)(PP_Resource scrollbar);
  /**
   * Gets the value of the scrollbar.
   */
  uint32_t (*GetValue)(PP_Resource scrollbar);
  /**
   * Sets the value of the scrollbar.
   */
  void (*SetValue)(PP_Resource scrollbar, uint32_t value);
  /**
   * Set the document size (i.e. total length of the region that's being
   * scrolled).
   */
  void (*SetDocumentSize)(PP_Resource scrollbar, uint32_t size);
  /**
   * Updates the tickmarks.  Only valid for vertical scrollbars.  "tick_marks"
   * contains "count" PP_Rect objects.
   */
  void (*SetTickMarks)(PP_Resource scrollbar,
                       const struct PP_Rect tick_marks[],
                       uint32_t count);
  /**
   * Scroll by "multiplier" pixels/lines/pages units.  Positive values are
   * forward and negative are backward.  If "unit" is document then any positive
   * value goes to the end while any negative value goes to the beginning.
   */
  void (*ScrollBy)(PP_Resource scrollbar,
                   PP_ScrollBy_Dev unit,
                   int32_t multiplier);
};

typedef struct PPB_Scrollbar_Dev_0_5 PPB_Scrollbar_Dev;
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPB_SCROLLBAR_DEV_H_ */

