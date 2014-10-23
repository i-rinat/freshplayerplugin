/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From private/ppb_talk_private.idl modified Thu Jun  6 10:54:19 2013. */

#ifndef PPAPI_C_PRIVATE_PPB_TALK_PRIVATE_H_
#define PPAPI_C_PRIVATE_PPB_TALK_PRIVATE_H_

#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_TALK_PRIVATE_INTERFACE_1_0 "PPB_Talk_Private;1.0"
#define PPB_TALK_PRIVATE_INTERFACE_2_0 "PPB_Talk_Private;2.0"
#define PPB_TALK_PRIVATE_INTERFACE PPB_TALK_PRIVATE_INTERFACE_2_0

/**
 * @file
 * This file contains the <code>PPB_Talk_Private</code> interface.
 */


/**
 * @addtogroup Enums
 * @{
 */
/**
 * The type of permissions that can be requested from the user.
 */
typedef enum {
  /**
   * Request permission for screencast.
   */
  PP_TALKPERMISSION_SCREENCAST,
  /**
   * Request permission for Remote Desktop.
   */
  PP_TALKPERMISSION_REMOTING,
  /**
   * Request permission for continuing Remote Desktop.
   */
  PP_TALKPERMISSION_REMOTING_CONTINUE,
  /**
   * Number of permissions.
   */
  PP_TALKPERMISSION_NUM_PERMISSIONS
} PP_TalkPermission;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_TalkPermission, 4);

/**
 * Talk event types reported by the browser.
 */
typedef enum {
  /**
   * Indicates that the user took action to terminate the remoting session.
   */
  PP_TALKEVENT_TERMINATE,
  /**
   * Indicates that an error occurred (e.g. failed to show the UI).
   */
  PP_TALKEVENT_ERROR,
  /**
   * Number of events
   */
  PP_TALKEVENT_NUM_EVENTS
} PP_TalkEvent;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_TalkEvent, 4);
/**
 * @}
 */

/**
 * @addtogroup Typedefs
 * @{
 */
/**
 * Callback for Talk events.
 */
typedef void (*PP_TalkEventCallback)(void* user_data, PP_TalkEvent event);
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * Extra interface for Talk.
 */
struct PPB_Talk_Private_2_0 {
  /**
   * Creates a Talk_Private resource.
   */
  PP_Resource (*Create)(PP_Instance instance);
  /**
   * Requests permission from the user using a system modal dialog.
   *
   * <code>permission</code> specifies the type of permission to request from
   * the user.
   *
   * <code>callback</code> is the completion callback. It will be issued with 1
   * as the result if the user gave permission, or 0 if the user denied.
   *
   * You can only have one call pending. It will return PP_OK_COMPLETIONPENDING
   * if the request is queued, or PP_ERROR_INPROGRESS if there is already a
   * request in progress.
   */
  int32_t (*RequestPermission)(PP_Resource talk_resource,
                               PP_TalkPermission permission,
                               struct PP_CompletionCallback callback);
  /**
   * Shows the remoting-in-progress UI and registers a callback for events.
   *
   * <code>event_callback</code> is the callback for session releated events.
   * It will only start receiving events after the completion callback has been
   * issued. This callback will be called on the pepper main thread.
   *
   * <code>user_data</code> is an opaque value used when
   * <code>event_callback</code> is invoked.
   *
   * <code>callback</code> is the completion callback.
   *
   * You can only have one call pending. It will return PP_OK_COMPLETIONPENDING
   * if the request is queued, or PP_ERROR_INPROGRESS if there is already a
   * request in progress. PP_ERROR_INPROGRESS will also be returned if a
   * previous call to StartRemoting succeeded without a corresponding
   * StopRemoting call.
   */
  int32_t (*StartRemoting)(PP_Resource talk_resource,
                           PP_TalkEventCallback event_callback,
                           void* user_data,
                           struct PP_CompletionCallback callback);
  /**
   * Hides the remoting-in-progress UI and unregisters the event callback.
   *
   * <code>callback</code> is the completion callback.
   */
  int32_t (*StopRemoting)(PP_Resource talk_resource,
                          struct PP_CompletionCallback callback);
};

typedef struct PPB_Talk_Private_2_0 PPB_Talk_Private;

struct PPB_Talk_Private_1_0 {
  PP_Resource (*Create)(PP_Instance instance);
  int32_t (*GetPermission)(PP_Resource talk_resource,
                           struct PP_CompletionCallback callback);
};
/**
 * @}
 */

#endif  /* PPAPI_C_PRIVATE_PPB_TALK_PRIVATE_H_ */

