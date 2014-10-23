/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppp_selection_dev.idl modified Fri Nov  9 12:50:33 2012. */

#ifndef PPAPI_C_DEV_PPP_SELECTION_DEV_H_
#define PPAPI_C_DEV_PPP_SELECTION_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

#define PPP_SELECTION_DEV_INTERFACE_0_3 "PPP_Selection(Dev);0.3"
#define PPP_SELECTION_DEV_INTERFACE PPP_SELECTION_DEV_INTERFACE_0_3

/**
 * @file
 * This file defines the PPP_Selection_Dev interface.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
struct PPP_Selection_Dev_0_3 {
  /**
   * Returns the selection, either as plain text or as html depending on "html".
   * If nothing is selected, or if the given format is unavailable, return a
   * void string.
   */
  struct PP_Var (*GetSelectedText)(PP_Instance instance, PP_Bool html);
};

typedef struct PPP_Selection_Dev_0_3 PPP_Selection_Dev;
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPP_SELECTION_DEV_H_ */

