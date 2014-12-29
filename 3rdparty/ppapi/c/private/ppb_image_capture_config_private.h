/* Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From private/ppb_image_capture_config_private.idl,
 *   modified Wed Aug 13 17:43:55 2014.
 */

#ifndef PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_CONFIG_PRIVATE_H_
#define PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_CONFIG_PRIVATE_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_size.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_IMAGECAPTURECONFIG_PRIVATE_INTERFACE_0_1 \
    "PPB_ImageCaptureConfig_Private;0.1"
#define PPB_IMAGECAPTURECONFIG_PRIVATE_INTERFACE \
    PPB_IMAGECAPTURECONFIG_PRIVATE_INTERFACE_0_1

/**
 * @file
 * This file defines the PPB_ImageCaptureConfig_Private interface for
 * establishing an image capture configuration resource within the browser.
 */


/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * The <code>PPB_ImageCaptureConfig_Private</code> interface contains pointers
 * to several functions for establishing image capture configuration within the
 * browser. The new configuration will take effect after <code>
 * PPB_ImageCapture_Private.SetConfig</code> is called.
 */
struct PPB_ImageCaptureConfig_Private_0_1 {
  /**
   * Creates a PPB_ImageCaptureConfig_Private resource.
   *
   * @param[in] instance A <code>PP_Instance</code> identifying one instance of
   * a module.
   *
   * @return A <code>PP_Resource</code> corresponding to a
   * PPB_ImageCaptureConfig_Private resource if successful, 0 if failed.
   */
  PP_Resource (*Create)(PP_Instance instance);
  /**
   * IsImageCaptureConfig() determines if the given resource is a
   * <code>PPB_ImageCaptureConfig_Private</code>.
   *
   * @param[in] resource A <code>PP_Resource</code> corresponding to an image
   * capture config resource.
   *
   * @return A <code>PP_Bool</code> containing <code>PP_TRUE</code> if the given
   * resource is an <code>ImageCaptureConfig_Private</code> resource, otherwise
   * <code>PP_FALSE</code>.
   */
  PP_Bool (*IsImageCaptureConfig)(PP_Resource resource);
  /**
   * GetPreviewSize() returns the preview image size in pixels for the given
   * <code>PPB_ImageCaptureConfig_Private</code>.
   *
   * @param[in] config A <code>PP_Resource</code> corresponding to an image
   * capture config resource.
   * @param[out] preview_size A <code>PP_Size</code> that indicates the
   * requested preview image size.
   */
  void (*GetPreviewSize)(PP_Resource config, struct PP_Size* preview_size);
  /**
   * SetPreviewSize() sets the preview image size for the given <code>
   * PPB_ImageCaptureConfig_Private</code>.
   *
   * @param[in] config A <code>PP_Resource</code> corresponding to a
   * <code>PPB_ImageCaptureConfig_Private</code>.
   * @param[in] preview_size A <code>PP_Size</code> that indicates the requested
   * preview image size.
   */
  void (*SetPreviewSize)(PP_Resource config,
                         const struct PP_Size* preview_size);
  /**
   * GetJpegSize() returns the JPEG image size in pixels for the given
   * <code>PPB_ImageCaptureConfig_Private</code>.
   *
   * @param[in] config A <code>PP_Resource</code> corresponding to an image
   * capture config resource.
   * @param[out] jpeg_size A <code>PP_Size</code> that indicates the current
   * JPEG image size.
   */
  void (*GetJpegSize)(PP_Resource config, struct PP_Size* jpeg_size);
  /**
   * SetJpegSize() sets the JPEG image size for the given
   * <code>PPB_ImageCaptureConfig_Private</code>.
   *
   * @param[in] config A <code>PP_Resource</code> corresponding to a
   * <code>PPB_ImageCaptureConfig_Private</code>.
   * @param[in] jpeg_size A <code>PP_Size</code> that indicates the requested
   * JPEG image size.
   */
  void (*SetJpegSize)(PP_Resource config, const struct PP_Size* jpeg_size);
};

typedef struct PPB_ImageCaptureConfig_Private_0_1
    PPB_ImageCaptureConfig_Private;
/**
 * @}
 */

#endif  /* PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_CONFIG_PRIVATE_H_ */

