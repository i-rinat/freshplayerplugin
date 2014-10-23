#include "ppb_flash_drm.h"
#include "pp_resource.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"


PP_Resource
ppb_flash_drm_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource flash_drm = pp_resource_allocate(PP_RESOURCE_FLASH_DRM, pp_i);
    struct pp_flash_drm_s *fd = pp_resource_acquire(flash_drm, PP_RESOURCE_FLASH_DRM);
    if (!fd) {
        trace_error("%s, failed to create flash drm resource\n", __func__);
        return 0;
    }

    (void)fd;

    pp_resource_release(flash_drm);
    return flash_drm;
}

void
ppb_flash_drm_destroy(void *p)
{
}

int32_t
ppb_flash_drm_get_device_id(PP_Resource drm, struct PP_Var *id,
                            struct PP_CompletionCallback callback)
{
    return -1;
}

PP_Bool
ppb_flash_drm_get_hmonitor(PP_Resource drm, int64_t *hmonitor)
{
    return PP_FALSE;
}

int32_t
ppb_flash_drm_get_voucher_file(PP_Resource drm, PP_Resource *file_ref,
                               struct PP_CompletionCallback callback)
{
    return -1;
}

int32_t
ppb_flash_drm_monitor_is_external(PP_Resource drm, PP_Bool *is_external,
                                  struct PP_CompletionCallback callback)
{
    return -1;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_flash_drm_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_drm_create(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_drm_get_device_id(PP_Resource drm, struct PP_Var *id,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_drm_get_device_id(drm, id, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_drm_get_hmonitor(PP_Resource drm, int64_t *hmonitor)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_drm_get_hmonitor(drm, hmonitor);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_drm_get_voucher_file(PP_Resource drm, PP_Resource *file_ref,
                                     struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_drm_get_voucher_file(drm, file_ref, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_drm_monitor_is_external(PP_Resource drm, PP_Bool *is_external,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_drm_monitor_is_external(drm, is_external, callback);
}


const struct PPB_Flash_DRM_1_1 ppb_flash_drm_interface_1_1 = {
    .Create =               TWRAPF(ppb_flash_drm_create),
    .GetDeviceID =          TWRAPZ(ppb_flash_drm_get_device_id),
    .GetHmonitor =          TWRAPZ(ppb_flash_drm_get_hmonitor),
    .GetVoucherFile =       TWRAPZ(ppb_flash_drm_get_voucher_file),
    .MonitorIsExternal =    TWRAPZ(ppb_flash_drm_monitor_is_external),
};

const struct PPB_Flash_DRM_1_0 ppb_flash_drm_interface_1_0 = {
    .Create =           TWRAPF(ppb_flash_drm_create),
    .GetDeviceID =      TWRAPZ(ppb_flash_drm_get_device_id),
    .GetHmonitor =      TWRAPZ(ppb_flash_drm_get_hmonitor),
    .GetVoucherFile =   TWRAPZ(ppb_flash_drm_get_voucher_file),
};
