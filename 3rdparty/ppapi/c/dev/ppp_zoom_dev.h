/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppp_zoom_dev.idl modified Wed Oct  5 14:06:02 2011. */

#ifndef PPAPI_C_DEV_PPP_ZOOM_DEV_H_
#define PPAPI_C_DEV_PPP_ZOOM_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_stdint.h"

#define PPP_ZOOM_DEV_INTERFACE_0_3 "PPP_Zoom(Dev);0.3"
#define PPP_ZOOM_DEV_INTERFACE PPP_ZOOM_DEV_INTERFACE_0_3

/**
 * @file
 * Implementation of the Zoom interface.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * Zoom interface should only apply to those full-page "plugin-document".
 */
struct PPP_Zoom_Dev_0_3 {
  /**
   * Instruct plugin to zoom according to the given factor and whether the zoom
   * only applies to text only.  The scale factor is the percentage divided by
   * 100, i.e. 150% zoom is 1.5.
   */
  void (*Zoom)(PP_Instance instance, double factor, PP_Bool text_only);
};

typedef struct PPP_Zoom_Dev_0_3 PPP_Zoom_Dev;
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPP_ZOOM_DEV_H_ */

