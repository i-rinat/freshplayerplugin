/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppb_widget_dev.idl modified Mon Aug 20 10:21:06 2012. */

#ifndef PPAPI_C_DEV_PPB_WIDGET_DEV_H_
#define PPAPI_C_DEV_PPB_WIDGET_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_point.h"
#include "ppapi/c/pp_rect.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_size.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_WIDGET_DEV_INTERFACE_0_3 "PPB_Widget(Dev);0.3"
#define PPB_WIDGET_DEV_INTERFACE_0_4 "PPB_Widget(Dev);0.4"
#define PPB_WIDGET_DEV_INTERFACE PPB_WIDGET_DEV_INTERFACE_0_4

/**
 * @file
 * Implementation of the widgets interface.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The interface for reusing browser widgets.
 */
struct PPB_Widget_Dev_0_4 {
  /**
   * Returns PP_TRUE if the given resource is a Widget. Returns PP_FALSE if the
   * resource is invalid or some type other than an Widget.
   */
  PP_Bool (*IsWidget)(PP_Resource resource);
  /**
   * Paint the given rectangle of the widget into the given image.
   * Returns PP_TRUE on success, PP_FALSE on failure.
   */
  PP_Bool (*Paint)(PP_Resource widget,
                   const struct PP_Rect* rect,
                   PP_Resource image);
  /**
   * Pass in an event to a widget.  It'll return PP_TRUE if the event was
   * consumed.
   */
  PP_Bool (*HandleEvent)(PP_Resource widget, PP_Resource input_event);
  /**
   * Get the location of the widget.
   */
  PP_Bool (*GetLocation)(PP_Resource widget, struct PP_Rect* location);
  /**
   * Set the location of the widget.
   */
  void (*SetLocation)(PP_Resource widget, const struct PP_Rect* location);
  /**
   * Set scale used during paint operations.
   */
  void (*SetScale)(PP_Resource widget, float scale);
};

typedef struct PPB_Widget_Dev_0_4 PPB_Widget_Dev;

struct PPB_Widget_Dev_0_3 {
  PP_Bool (*IsWidget)(PP_Resource resource);
  PP_Bool (*Paint)(PP_Resource widget,
                   const struct PP_Rect* rect,
                   PP_Resource image);
  PP_Bool (*HandleEvent)(PP_Resource widget, PP_Resource input_event);
  PP_Bool (*GetLocation)(PP_Resource widget, struct PP_Rect* location);
  void (*SetLocation)(PP_Resource widget, const struct PP_Rect* location);
};
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPB_WIDGET_DEV_H_ */

