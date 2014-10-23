// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is auto-generated from
// gpu/command_buffer/build_gles2_cmd_buffer.py
// It's formatted by clang-format using chromium coding style:
//    clang-format -i -style=chromium filename
// DO NOT EDIT!

// OpenGL ES interface.
#ifndef PPAPI_C_DEV_PPB_OPENGLES2EXT_DEV_H_
#define PPAPI_C_DEV_PPB_OPENGLES2EXT_DEV_H_

#include "ppapi/c/pp_resource.h"
#include "ppapi/c/ppb_opengles2.h"

#define PPB_OPENGLES2_DRAWBUFFERS_DEV_INTERFACE_1_0 \
  "PPB_OpenGLES2DrawBuffers(Dev);1.0"
#define PPB_OPENGLES2_DRAWBUFFERS_DEV_INTERFACE \
  PPB_OPENGLES2_DRAWBUFFERS_DEV_INTERFACE_1_0

struct PPB_OpenGLES2DrawBuffers_Dev {
  void (*DrawBuffersEXT)(PP_Resource context,
                         GLsizei count,
                         const GLenum* bufs);
};

#endif  // PPAPI_C_DEV_PPB_OPENGLES2EXT_DEV_H_
