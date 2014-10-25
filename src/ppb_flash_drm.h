#ifndef FPP_PPB_FLASH_DRM_H
#define FPP_PPB_FLASH_DRM_H

#include <ppapi/c/private/ppb_flash_drm.h>


PP_Resource
ppb_flash_drm_create(PP_Instance instance);

void
ppb_flash_drm_destroy(void *p);

int32_t
ppb_flash_drm_get_device_id(PP_Resource drm, struct PP_Var *id,
                            struct PP_CompletionCallback callback);

PP_Bool
ppb_flash_drm_get_hmonitor(PP_Resource drm, int64_t *hmonitor);

int32_t
ppb_flash_drm_get_voucher_file(PP_Resource drm, PP_Resource *file_ref,
                               struct PP_CompletionCallback callback);

int32_t
ppb_flash_drm_monitor_is_external(PP_Resource drm, PP_Bool *is_external,
                                  struct PP_CompletionCallback callback);


#endif // FPP_PPB_FLASH_DRM_H

