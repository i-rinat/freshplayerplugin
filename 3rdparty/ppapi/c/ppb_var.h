/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From ppb_var.idl modified Wed Dec 14 18:08:00 2011. */

#ifndef PPAPI_C_PPB_VAR_H_
#define PPAPI_C_PPB_VAR_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

#define PPB_VAR_INTERFACE_1_0 "PPB_Var;1.0"
#define PPB_VAR_INTERFACE_1_1 "PPB_Var;1.1"
#define PPB_VAR_INTERFACE PPB_VAR_INTERFACE_1_1

/**
 * @file
 * This file defines the <code>PPB_Var</code> struct.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * PPB_Var API
 */
struct PPB_Var_1_1 {
  /**
   * AddRef() adds a reference to the given var. If this is not a refcounted
   * object, this function will do nothing so you can always call it no matter
   * what the type.
   *
   * @param[in] var A <code>PP_Var</code> that will have a reference added.
   */
  void (*AddRef)(struct PP_Var var);
  /**
   * Release() removes a reference to given var, deleting it if the internal
   * reference count becomes 0. If the given var is not a refcounted object,
   * this function will do nothing so you can always call it no matter what
   * the type.
   *
   * @param[in] var A <code>PP_Var</code> that will have a reference removed.
   */
  void (*Release)(struct PP_Var var);
  /**
   * VarFromUtf8() creates a string var from a string. The string must be
   * encoded in valid UTF-8 and is NOT NULL-terminated, the length must be
   * specified in <code>len</code>. It is an error if the string is not
   * valid UTF-8.
   *
   * If the length is 0, the <code>*data</code> pointer will not be dereferenced
   * and may be <code>NULL</code>. Note, however if length is 0, the
   * "NULL-ness" will not be preserved, as <code>VarToUtf8</code> will never
   * return <code>NULL</code> on success, even for empty strings.
   *
   * The resulting object will be a refcounted string object. It will be
   * AddRef'ed for the caller. When the caller is done with it, it should be
   * Released.
   *
   * On error (basically out of memory to allocate the string, or input that
   * is not valid UTF-8), this function will return a Null var.
   *
   * @param[in] data A string
   * @param[in] len The length of the string.
   *
   * @return A <code>PP_Var</code> structure containing a reference counted
   * string object.
   */
  struct PP_Var (*VarFromUtf8)(const char* data, uint32_t len);
  /**
   * VarToUtf8() converts a string-type var to a char* encoded in UTF-8. This
   * string is NOT NULL-terminated. The length will be placed in
   * <code>*len</code>. If the string is valid but empty the return value will
   * be non-NULL, but <code>*len</code> will still be 0.
   *
   * If the var is not a string, this function will return NULL and
   * <code>*len</code> will be 0.
   *
   * The returned buffer will be valid as long as the underlying var is alive.
   * If the instance frees its reference, the string will be freed and the
   * pointer will be to arbitrary memory.
   *
   * @param[in] var A PP_Var struct containing a string-type var.
   * @param[in,out] len A pointer to the length of the string-type var.
   *
   * @return A char* encoded in UTF-8.
   */
  const char* (*VarToUtf8)(struct PP_Var var, uint32_t* len);
};

typedef struct PPB_Var_1_1 PPB_Var;

struct PPB_Var_1_0 {
  void (*AddRef)(struct PP_Var var);
  void (*Release)(struct PP_Var var);
  struct PP_Var (*VarFromUtf8)(PP_Module module,
                               const char* data,
                               uint32_t len);
  const char* (*VarToUtf8)(struct PP_Var var, uint32_t* len);
};
/**
 * @}
 */

#endif  /* PPAPI_C_PPB_VAR_H_ */

