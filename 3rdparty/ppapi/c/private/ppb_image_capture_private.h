/* Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From private/ppb_image_capture_private.idl,
 *   modified Wed Aug 13 17:26:13 2014.
 */

#ifndef PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_PRIVATE_H_
#define PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_PRIVATE_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"

#define PPB_IMAGECAPTURE_PRIVATE_INTERFACE_0_1 "PPB_ImageCapture_Private;0.1"
#define PPB_IMAGECAPTURE_PRIVATE_INTERFACE \
    PPB_IMAGECAPTURE_PRIVATE_INTERFACE_0_1

/**
 * @file
 * Defines the <code>PPB_ImageCapture_Private</code> interface. Used for
 * acquiring a single still image from a camera source.
 */


/**
 * @addtogroup Typedefs
 * @{
 */
/**
 * Callback function for <code>PPB_ImageCapture_Private.CaptureStillImage
 * </code> to indicate the image has been captured from the sensor. This is a
 * good opportunity to play a shutter sound or give other feedback of camera
 * operation. This will occur after the image was captured, but before the
 * actual data is available.
 *
 * Parameters:
 *  |user_data| The same pointer that was passed into <code>
 *  PPB_ImageCapture_Private.Create()</code>.
 *  |sequence_id| The sequence ID of the image capture, same as the one from
 *  CaptureStillImage.
 */
typedef void (*PPB_ImageCapture_Private_ShutterCallback)(void* user_data,
                                                         int64_t sequence_id);

/**
 * Callback function for <code>PPB_ImageCapture_Private.CaptureStillImage
 * </code> to deliver a preview image. The client can use this to show the
 * captured image. See <code>PPB_ImageCapture_Private.CaptureStillImage
 * </code> for more information.
 *
 * Parameters:
 *  |user_data| The same pointer that was passed into <code>
 *  PPB_ImageCapture_Private.Create()</code>.
 *  |sequence_id| The sequence ID of the image capture, same as the one from
 *  CaptureStillImage.
 *  |preview| A <code>PP_Resource</code> corresponding to a VideoFrame
 *  resource used to store the preview image.
 */
typedef void (*PPB_ImageCapture_Private_PreviewCallback)(void* user_data,
                                                         int64_t sequence_id,
                                                         PP_Resource preview);

/**
 * Callback function for <code>PPB_ImageCapture_Private.CaptureStillImage
 * </code> to deliver a still JPEG image. See <code>
 * PPB_ImageCapture_Private.CaptureStillImage</code> for more information.
 *
 * Parameters:
 *  |user_data| The same pointer that was passed into <code>
 *  PPB_ImageCapture_Private.Create()</code>.
 *  |sequence_id| The sequence ID of the image capture, same as the one from
 *  CaptureStillImage.
 *  |jpeg| A <code>PP_Resource</code> corresponding to a VideoFrame
 *  resource used to store the JPEG image.
 */
typedef void (*PPB_ImageCapture_Private_JpegCallback)(void* user_data,
                                                      int64_t sequence_id,
                                                      PP_Resource jpeg);

/**
 * Callback function for <code>PPB_ImageCapture_Private.CaptureStillImage
 * </code> to indicate the image capture has failed.
 *
 * Parameters:
 *  |user_data| The same pointer that was passed into <code>
 *  PPB_ImageCapture_Private.Create()</code>.
 *  |sequence_id| The sequence ID of the image capture, same as the one from
 *  CaptureStillImage.
 *  |int32_t| An error code from <code>pp_errors.h</code>.
 */
typedef void (*PPB_ImageCapture_Private_ErrorCallback)(void* user_data,
                                                       int64_t sequence_id,
                                                       int32_t pp_error);
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/**
 * To capture a still image with this class, use the following steps.
 * 1. Get a PPB_ImageCapture_Private object by Create().
 * 2. Call GetCameraCapabilities to get the supported preview sizes.
 * 3. For optimal performance, set one of the supported preview size as the
 *    constraints of getUserMedia. Use the created MediaStreamVideoTrack for
 *    camera previews.
 * 4. Set the same preview size and other settings by SetConfig.
 * 5. Call CaptureStillImage to capture a still image. Play the shutter sound in
 *    the shutter callback. The image from the preview callback can be used for
 *    display. JPEG image will be returned to the JPEG callback.
 */
struct PPB_ImageCapture_Private_0_1 {
  /**
   * Creates a PPB_ImageCapture_Private resource.
   *
   * @param[in] instance A <code>PP_Instance</code> identifying one instance
   * of a module.
   * @param[in] camera_source_id A <code>PP_Var</code> identifying a camera
   * source. The type is string. The ID can be obtained from
   * MediaStreamTrack.getSources() or MediaStreamVideoTrack.id. If a
   * MediaStreamVideoTrack is associated with the same source and the track
   * is closed, this PPB_ImageCapture_Private object can still do image capture.
   * @param[in] error_callback A <code>PPB_ImageCapture_Private_ErrorCallback
   * </code> callback to indicate the image capture has failed.
   * @param[inout] user_data An opaque pointer that will be passed to the
   * callbacks of PPB_ImageCapture_Private.
   *
   * @return A <code>PP_Resource</code> corresponding to a
   * PPB_ImageCapture_Private resource if successful, 0 if failed.
   */
  PP_Resource (*Create)(PP_Instance instance,
                        struct PP_Var camera_source_id,
                        PPB_ImageCapture_Private_ErrorCallback error_callback,
                        void* user_data);
  /**
   * Determines if a resource is an image capture resource.
   *
   * @param[in] resource The <code>PP_Resource</code> to test.
   *
   * @return A <code>PP_Bool</code> with <code>PP_TRUE</code> if the given
   * resource is an image capture resource or <code>PP_FALSE</code>
   * otherwise.
   */
  PP_Bool (*IsImageCapture)(PP_Resource resource);
  /**
   * Disconnects from the camera and cancels all pending capture requests.
   * After this returns, no callbacks will be called. If <code>
   * PPB_ImageCapture_Private</code> is destroyed and is not closed yet, this
   * function will be automatically called. Calling this more than once has no
   * effect.
   *
   * @param[in] image_capture A <code>PP_Resource</code> corresponding to an
   * image capture resource.
   * @param[in] callback <code>PP_CompletionCallback</code> to be called upon
   * completion of <code>Close()</code>.
   *
   * @return An int32_t containing a result code from <code>pp_errors.h</code>.
   */
  int32_t (*Close)(PP_Resource resource, struct PP_CompletionCallback callback);
  /**
   * Sets the configuration of the image capture.
   * If <code>SetConfig()</code> is not called, default settings will be used.
   *
   * @param[in] image_capture A <code>PP_Resource</code> corresponding to an
   * image capture resource.
   * @param[in] config A <code>PP_ImageCaptureConfig_Private</code> object.
   * @param[in] callback <code>PP_CompletionCallback</code> to be called upon
   * completion of <code>SetConfig()</code>.
   *
   * @return An int32_t containing a result code from <code>pp_errors.h</code>.
   * Returns <code>PP_ERROR_INPROGRESS</code> if there is a pending call of
   * <code>SetConfig()</code> or <code>CaptureStillImage()</code>.
   * If an error is returned, the configuration will not be changed.
   */
  int32_t (*SetConfig)(PP_Resource image_capture,
                       PP_Resource config,
                       struct PP_CompletionCallback callback);
  /**
   * Gets the configuration of the image capture.
   *
   * @param[in] image_capture A <code>PP_Resource</code> corresponding to an
   * image capture resource.
   * @param[out] config A <code>PP_ImageCaptureConfig_Private</code> for storing
   * the current image capture config on success. Otherwise, the values will not
   * be changed.
   * @param[in] callback <code>PP_CompletionCallback</code> to be called upon
   * completion of <code>GetConfig()</code>.
   *
   * @return An int32_t containing a result code from <code>pp_errors.h</code>.
   */
  int32_t (*GetConfig)(PP_Resource image_capture,
                       PP_Resource* config,
                       struct PP_CompletionCallback callback);
  /**
   * Gets the camera capabilities.
   *
   * The camera capabilities do not change for a given camera source.
   *
   * @param[in] image_capture A <code>PP_Resource</code> corresponding to an
   * image capture resource.
   * @param[out] capabilities A <code>PPB_CameraCapabilities_Private</code> for
   * storing the image capture capabilities on success. Otherwise, the value
   * will not be changed.
   * @param[in] callback <code>PP_CompletionCallback</code> to be called upon
   * completion of <code>GetCameraCapabilities()</code>.
   *
   * @return An int32_t containing a result code from <code>pp_errors.h</code>.
   */
  int32_t (*GetCameraCapabilities)(PP_Resource image_capture,
                                   PP_Resource* capabilities,
                                   struct PP_CompletionCallback callback);
  /**
   * Captures a still JPEG image from the camera.
   *
   * Triggers an asynchronous image capture. The camera will initiate a series
   * of callbacks to the application as the image capture progresses. The
   * callbacks will be invoked in the order of shutter callback, preview
   * callback, and JPEG callback. The shutter callback occurs after the image is
   * captured. This can be used to trigger a sound to let the user know that
   * image has been captured. The preview callback occurs when a scaled, fully
   * processed preview image is available. The JPEG callback occurs when the
   * compressed image is available. If there is an error after the capture is in
   * progress, the error callback passed to <code>
   * PPB_ImageCapture_Private.Create()</code> will be invoked. All the callbacks
   * are invoked by the thread that calls this function.
   *
   * The size of the preview image in preview callback is determined by
   * <code>PPB_ImageCaptureConfig_Private.SetPreviewSize</code>. The format is
   * decided by the camera and can be got from <code>PPB_VideoFrame.GetFormat
   * </code>. The size of the JPEG image is determined by <code>
   * PPB_ImageCaptureConfig_Private.SetJpegSize</code>.
   *
   * The camera may need to stop and re-start streaming during image capture. If
   * some MediaStreamVideoTrack are associated with the camera source, they will
   * receive mute and unmute events. The mute event will be received before all
   * the callbacks. The unmute event will be received after all the callbacks.
   * The preview image will not be sent to the video tracks associated with the
   * camera.
   *
   * @param[in] image_capture A <code>PP_Resource</code> corresponding to an
   * image capture resource.
   * @param[in] shutter_callback A <code>
   * PPB_ImageCapture_Private_ShutterCallback</code> callback to indicate the
   * image has been taken.
   * @param[in] preview_callback A <code>
   * PPB_ImageCapture_Private_PreviewCallback</code> callback to return a
   * preview of the captured image.
   * @param[in] jpeg_callback A <code>
   * PPB_ImageCapture_Private_JpegCallback</code> callback to return captured
   * JPEG image.
   * @param[out] sequence_id The sequence ID is a unique monotonically
   * increasing value starting from 0, incremented every time a new request like
   * image capture is submitted.
   *
   * @return An int32_t containing a result code from <code>pp_errors.h</code>.
   * PP_OK means the callbacks will be triggered. Other values mean the
   * callbacks will not be triggered.
   */
  int32_t (*CaptureStillImage)(
      PP_Resource image_capture,
      PPB_ImageCapture_Private_ShutterCallback shutter_callback,
      PPB_ImageCapture_Private_PreviewCallback preview_callback,
      PPB_ImageCapture_Private_JpegCallback jpeg_callback,
      int64_t* sequence_id);
};

typedef struct PPB_ImageCapture_Private_0_1 PPB_ImageCapture_Private;
/**
 * @}
 */

#endif  /* PPAPI_C_PRIVATE_PPB_IMAGE_CAPTURE_PRIVATE_H_ */

