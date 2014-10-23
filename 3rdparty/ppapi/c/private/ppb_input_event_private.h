/* Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From private/ppb_input_event_private.idl,
 *   modified Tue May 20 18:31:39 2014.
 */

#ifndef PPAPI_C_PRIVATE_PPB_INPUT_EVENT_PRIVATE_H_
#define PPAPI_C_PRIVATE_PPB_INPUT_EVENT_PRIVATE_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_INPUTEVENT_PRIVATE_INTERFACE_0_1 "PPB_InputEvent_Private;0.1"
#define PPB_INPUTEVENT_PRIVATE_INTERFACE PPB_INPUTEVENT_PRIVATE_INTERFACE_0_1

/**
 * @file
 * This file defines the PPB_InputEvent_Private interfaces.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The <code>PPB_InputEvent_Private</code> interface contains pointers to
    several
 * functions related to generic input events on the browser.
 */
struct PPB_InputEvent_Private_0_1 {
  /**
   * TraceInputLatency() traces the latency of the input event. The input
   * latency is shown in the trace viewer. The starting point of the input
   * latency is when the input event is sent from renderer to plugin. If the
   * input event does not cause any rendering damage, the end point of input
   * latency is when TraceInputLatency() is called on the event. If the input
   * event does cause rendering damage, the end point of input latency is when
   * the resulted plugin frame eventually reaches screen.
   *
   * Notes: In the current version, only Graphics2D damage is considered as
   * rendering damage. And it works only when the Graphics2Ds are bound to
   * the plugin instance.
   *
   * @param[in] event A <code>PP_Resource</code> corresponding to an input
   * event.
   *
   * @param[in] has_damage A bool indicating whether the event has caused any
   * rendering damage.
   *
   * @return <code>PP_TRUE</code> if the latency for the given event is tracked.
   */
  PP_Bool (*TraceInputLatency)(PP_Resource event, PP_Bool has_damage);
  /**
   * StartTrackingLatency() requests input latency to be tracked.
   *
   * Without calling StartTrackingLatency() first, TraceInputLatency() won't
   * take effect.
   *
   * @param[in] instance The <code>PP_Instance</code> of the instance requesting
   * to start tracking input latency.
   */
  void (*StartTrackingLatency)(PP_Instance instance);
};

typedef struct PPB_InputEvent_Private_0_1 PPB_InputEvent_Private;
/**
 * @}
 */

#endif  /* PPAPI_C_PRIVATE_PPB_INPUT_EVENT_PRIVATE_H_ */

