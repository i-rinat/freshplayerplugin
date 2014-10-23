/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppp_widget_dev.idl modified Wed Oct  5 14:06:02 2011. */

#ifndef PPAPI_C_DEV_PPP_WIDGET_DEV_H_
#define PPAPI_C_DEV_PPP_WIDGET_DEV_H_

#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_point.h"
#include "ppapi/c/pp_rect.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_size.h"
#include "ppapi/c/pp_stdint.h"

#define PPP_WIDGET_DEV_INTERFACE_0_2 "PPP_Widget(Dev);0.2"
#define PPP_WIDGET_DEV_INTERFACE PPP_WIDGET_DEV_INTERFACE_0_2

/**
 * @file
 * Implementation of the Widget interface.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The interface for reusing browser widgets.
 */
struct PPP_Widget_Dev_0_2 {
  /**
   * Informs the instance that the given rectangle needs to be repainted.
   */
  void (*Invalidate)(PP_Instance instance,
                     PP_Resource widget,
                     const struct PP_Rect* dirty_rect);
};

typedef struct PPP_Widget_Dev_0_2 PPP_Widget_Dev;
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPP_WIDGET_DEV_H_ */

