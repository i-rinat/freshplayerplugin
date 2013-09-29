/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From pp_rect.idl modified Wed Oct  5 14:06:02 2011. */

#ifndef PPAPI_C_PP_RECT_H_
#define PPAPI_C_PP_RECT_H_

#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_point.h"
#include "ppapi/c/pp_size.h"
#include "ppapi/c/pp_stdint.h"

/**
 * @file
 * This file defines the APIs for creating a 2 dimensional rectangle.
 */


/**
 * @addtogroup Structs
 * @{
 */
/**
 * The <code>PP_Rect</code> struct contains the size and location of a 2D
 * rectangle.
 */
struct PP_Rect {
  /**
   * This value represents the x and y coordinates of the upper-left corner of
   * the rectangle.
   */
  struct PP_Point point;
  /** This value represents the width and height of the rectangle. */
  struct PP_Size size;
};
PP_COMPILE_ASSERT_STRUCT_SIZE_IN_BYTES(PP_Rect, 16);
/**
 * @}
 */


/**
 * @addtogroup Functions
 * @{
 */

/**
 * PP_MakeRectFromXYWH() creates a <code>PP_Rect</code> given x and y
 * coordinates and width and height dimensions as int32_t values.
 *
 * @param[in] x An int32_t value representing a horizontal coordinate of a
 * point, starting with 0 as the left-most coordinate.
 * @param[in] y An int32_t value representing a vertical coordinate of a point,
 * starting with 0 as the top-most coordinate.
 * @param[in] w An int32_t value representing a width.
 * @param[in] h An int32_t value representing a height.
 *
 * @return A <code>PP_Rect</code> structure.
 */
PP_INLINE struct PP_Rect PP_MakeRectFromXYWH(int32_t x, int32_t y,
                                             int32_t w, int32_t h) {
  struct PP_Rect ret;
  ret.point.x = x;
  ret.point.y = y;
  ret.size.width = w;
  ret.size.height = h;
  return ret;
}
/**
 * @}
 */

#endif  /* PPAPI_C_PP_RECT_H_ */

