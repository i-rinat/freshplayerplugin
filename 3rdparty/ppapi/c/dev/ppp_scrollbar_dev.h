/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From dev/ppp_scrollbar_dev.idl modified Tue Nov 27 14:46:25 2012. */

#ifndef PPAPI_C_DEV_PPP_SCROLLBAR_DEV_H_
#define PPAPI_C_DEV_PPP_SCROLLBAR_DEV_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"

#define PPP_SCROLLBAR_DEV_INTERFACE_0_2 "PPP_Scrollbar(Dev);0.2"
#define PPP_SCROLLBAR_DEV_INTERFACE_0_3 "PPP_Scrollbar(Dev);0.3"
#define PPP_SCROLLBAR_DEV_INTERFACE PPP_SCROLLBAR_DEV_INTERFACE_0_3

/**
 * @file
 * This file defines the <code>PPP_Scrollbar_Dev</code> interface.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
struct PPP_Scrollbar_Dev_0_3 {
  /**
   *  Informs the instance that the scrollbar's value has changed.
   */
  void (*ValueChanged)(PP_Instance instance,
                       PP_Resource scrollbar,
                       uint32_t value);
  /**
   * Informs the instance that the user has changed the system
   * scrollbar style.
   */
  void (*OverlayChanged)(PP_Instance instance,
                         PP_Resource scrollbar,
                         PP_Bool overlay);
};

typedef struct PPP_Scrollbar_Dev_0_3 PPP_Scrollbar_Dev;

struct PPP_Scrollbar_Dev_0_2 {
  void (*ValueChanged)(PP_Instance instance,
                       PP_Resource scrollbar,
                       uint32_t value);
};
/**
 * @}
 */

#endif  /* PPAPI_C_DEV_PPP_SCROLLBAR_DEV_H_ */

