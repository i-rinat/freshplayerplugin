/* Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef PPAPI_C_DEV_IMPL_PPB_MESSAGING_DEPRECATED_H_
#define PPAPI_C_DEV_IMPL_PPB_MESSAGING_DEPRECATED_H_

#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

/* dev, deprecated */
#define PPB_MESSAGING_INTERFACE_1_1_DEPRECATED "PPB_Messaging;1.1"

/**
 * This file defines a dev-channel-only API, PPB_Messaging;1.1 that is
 * deprecated, but which still should provide ABI compatibility for a little
 * bit longer to give time to transition off of it. The header is *not* provided
 * in ppapi/c/ppb_messaging.h, in order to push clients to the new API,
 * version 1.2.
 * TODO(dmichael): Delete this API altogether when all uses are gone.
 *                 crbug.com/414398
 */
struct PPP_MessageHandler_0_1_Deprecated { /* dev */
  void (*HandleMessage)(PP_Instance instance,
                        void* user_data,
                        struct PP_Var message);
  struct PP_Var (*HandleBlockingMessage)(PP_Instance instance,
                                void* user_data,
                                struct PP_Var message);
  void (*Destroy)(PP_Instance instance, void* user_data);
};

struct PPB_Messaging_1_1_Deprecated { /* dev */
  void (*PostMessage)(PP_Instance instance, struct PP_Var message);
  int32_t (*RegisterMessageHandler)(
      PP_Instance instance,
      void* user_data,
      const struct PPP_MessageHandler_0_1_Deprecated* handler,
      PP_Resource message_loop);
  void (*UnregisterMessageHandler)(PP_Instance instance);
};

#endif  /* PPAPI_C_DEV_IMPL_PPB_MESSAGING_DEPRECATED_H_ */

