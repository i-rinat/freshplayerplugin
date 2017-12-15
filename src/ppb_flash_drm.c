/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_crypto.h"
#include "ppb_flash_drm.h"
#include "ppb_message_loop.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include <ppapi/c/pp_errors.h>
#include <stdio.h>
#include <stdlib.h>

#define salt_length             32

STATIC_ASSERT_LESS_OR_EQ(salt_length, 32);

struct pp_flash_drm_s {
    COMMON_STRUCTURE_FIELDS
};

STATIC_ASSERT(sizeof(struct pp_flash_drm_s) <= LARGEST_RESOURCE_SIZE);

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

    pp_resource_release(flash_drm);
    return flash_drm;
}

static
void
ppb_flash_drm_destroy(void *p)
{
}

static
void
get_system_salt(char *salt)
{
    do {
        FILE *fp = fopen("/etc/machine-id", "rb");
        if (!fp)
            break;
        if (fread(salt, 1, 32, fp) != 32) {
            fclose(fp);
            break;
        }

        fclose(fp);
        return;
    } while (0);

    do {
        FILE *fp = fopen("/var/lib/dbus/machine-id", "rb");
        if (!fp)
            break;
        if (fread(salt, 1, 32, fp) != 32) {
            fclose(fp);
            break;
        }

        fclose(fp);
        return;
    } while (0);

    // fall back to random number generator
    unsigned char salt_raw[salt_length / 2];
    ppb_crypto_get_random_bytes((char *)salt_raw, salt_length / 2);

    // make hexadecimal string
    for (unsigned int k = 0; k < salt_length / 2; k ++) {
        const char tbl[] = "0123456789abcdef";
        salt[2 * k] =     tbl[(salt_raw[k] >> 4) & 0xf];
        salt[2 * k + 1] = tbl[salt_raw[k] & 0xf];
    }
}

int32_t
ppb_flash_drm_get_device_id(PP_Resource drm, struct PP_Var *id,
                            struct PP_CompletionCallback callback)
{
    const char *salt_fname = fpp_config_get_pepper_salt_file_name();
    FILE *fp;
    char salt[salt_length];

    (void)drm;

    fp = fopen(salt_fname, "rb");
    if (!fp) {
        // create salt
        trace_info("%s, creating salt file\n", __func__);
        fp = fopen(salt_fname, "wb");
        if (!fp) {
            trace_error("%s, can't create salt file\n", __func__);
            return PP_ERROR_FAILED;
        }

        get_system_salt(salt);
        size_t written = fwrite(salt, 1, salt_length, fp);
        fclose(fp);

        if (written != salt_length) {
            trace_error("%s, can't write to salt file\n", __func__);
            return PP_ERROR_FAILED;
        }

        fp = fopen(salt_fname, "rb");
        if (!fp) {
            trace_error("%s, can't open salt file\n", __func__);
            return PP_ERROR_FAILED;
        }
    }

    size_t read_bytes = fread(salt, 1, salt_length, fp);
    fclose(fp);

    if (read_bytes != salt_length) {
        trace_error("%s, truncated salt file. Remove it and try again\n", __func__);
        return PP_ERROR_FAILED;
    }

    *id = ppb_var_var_from_utf8(salt, salt_length);
    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, PP_OK, 0,
                                           __func__);
    return PP_OK_COMPLETIONPENDING;
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
    trace_info("[PPB] {full} %s drm=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, drm, callback.func, callback.user_data, callback.flags);
    return ppb_flash_drm_get_device_id(drm, id, callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_drm_get_hmonitor(PP_Resource drm, int64_t *hmonitor)
{
    trace_info("[PPB] {zilch} %s drm=%d\n", __func__+6, drm);
    return ppb_flash_drm_get_hmonitor(drm, hmonitor);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_drm_get_voucher_file(PP_Resource drm, PP_Resource *file_ref,
                                     struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s drm=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, drm, callback.func, callback.user_data, callback.flags);
    return ppb_flash_drm_get_voucher_file(drm, file_ref, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_drm_monitor_is_external(PP_Resource drm, PP_Bool *is_external,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s drm=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, drm, callback.func, callback.user_data, callback.flags);
    return ppb_flash_drm_monitor_is_external(drm, is_external, callback);
}


const struct PPB_Flash_DRM_1_1 ppb_flash_drm_interface_1_1 = {
    .Create =               TWRAPF(ppb_flash_drm_create),
    .GetDeviceID =          TWRAPF(ppb_flash_drm_get_device_id),
    .GetHmonitor =          TWRAPZ(ppb_flash_drm_get_hmonitor),
    .GetVoucherFile =       TWRAPZ(ppb_flash_drm_get_voucher_file),
    .MonitorIsExternal =    TWRAPZ(ppb_flash_drm_monitor_is_external),
};

const struct PPB_Flash_DRM_1_0 ppb_flash_drm_interface_1_0 = {
    .Create =           TWRAPF(ppb_flash_drm_create),
    .GetDeviceID =      TWRAPF(ppb_flash_drm_get_device_id),
    .GetHmonitor =      TWRAPZ(ppb_flash_drm_get_hmonitor),
    .GetVoucherFile =   TWRAPZ(ppb_flash_drm_get_voucher_file),
};

static
void
__attribute__((constructor))
constructor_ppb_flash_drm(void)
{
    register_interface(PPB_FLASH_DRM_INTERFACE_1_0, &ppb_flash_drm_interface_1_0);
    register_interface(PPB_FLASH_DRM_INTERFACE_1_1, &ppb_flash_drm_interface_1_1);
    register_resource(PP_RESOURCE_FLASH_DRM, ppb_flash_drm_destroy);
}
