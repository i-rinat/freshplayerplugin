/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include "ppb_nacl.h"
#include <stdlib.h>
#include <inttypes.h>
#include <ppapi/c/pp_errors.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"


void
ppb_nacl_launch_sel_ldr(PP_Instance instance, PP_Bool main_service_runtime, const char *alleged_url,
                        const struct PP_NaClFileInfo *nexe_file_info, PP_Bool uses_nonsfi_mode,
                        PP_Bool enable_ppapi_dev, PP_NaClAppProcessType process_type,
                        void *imc_handle, struct PP_CompletionCallback callback)
{
}

PP_Bool
ppb_nacl_start_ppapi_proxy(PP_Instance instance)
{
    return PP_EXTERNAL_PLUGIN_OK;
}

int32_t
ppb_nacl_urandom_fd(void)
{
    int fd = tables_get_urandom_fd();
    if (fd < 0)
        return 0;
    else
        return fd;
}

PP_Bool
ppb_nacl_are_3d_interfaces_disabled(void)
{
    return PP_FALSE;
}

int32_t
ppb_nacl_broker_duplicate_handle(PP_FileHandle source_handle, uint32_t process_id,
                                 PP_FileHandle *target_handle, uint32_t desired_access,
                                 uint32_t options)
{
    return -1;
}

PP_FileHandle
ppb_nacl_create_temporary_file(PP_Instance instance)
{
    return -1;
}

int32_t
ppb_nacl_get_number_of_processors(void)
{
    return 2;
}

PP_Bool
ppb_nacl_is_non_sfi_mode_enabled(void)
{
    return PP_FALSE;
}

void
ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success, int32_t opt_level,
                                     int64_t pexe_size, int64_t compile_time_us)
{
}

void
ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type, const char *resource_url,
                        PP_Bool length_is_computable, uint64_t loaded_bytes, uint64_t total_bytes)
{
}

void
ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error, const char *error_message)
{
}

void
ppb_nacl_instance_created(PP_Instance instance)
{
}

void
ppb_nacl_instance_destroyed(PP_Instance instance)
{
}

PP_Bool
ppb_nacl_nacl_debug_enabled_for_url(const char *alleged_nmf_url)
{
    return PP_FALSE;
}

const char *
ppb_nacl_get_sandbox_arch(void)
{
#if defined(__x86_64__)
    return "x86-64";
#elif defined(__i386__)
    return "x86-32";
#elif defined(__mips__)
    return "mips32";
#elif defined(__arm__)
    return "arm";
#else
    return "x86-32"; // fallback
#endif
}

void
ppb_nacl_log_to_console(PP_Instance instance, const char *message)
{
}

PP_NaClReadyState
ppb_nacl_get_nacl_ready_state(PP_Instance instance)
{
    return 0;
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_nacl_launch_sel_ldr(PP_Instance instance, PP_Bool main_service_runtime,
                              const char *alleged_url, const struct PP_NaClFileInfo *nexe_file_info,
                              PP_Bool uses_nonsfi_mode, PP_Bool enable_ppapi_dev,
                              PP_NaClAppProcessType process_type, void *imc_handle,
                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s instance=%d, main_service_runtime=%u, alleged_url=%s, "
               "nexe_file_info=%p, uses_nonsfi_mode=%d, enable_ppapi_dev=%d, process_type=%u, "
               "imc_handle=%p, callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6,
               instance, main_service_runtime, alleged_url, nexe_file_info, uses_nonsfi_mode,
               enable_ppapi_dev, process_type, imc_handle, callback.func, callback.user_data,
               callback.flags);
    return ppb_nacl_launch_sel_ldr(instance, main_service_runtime, alleged_url, nexe_file_info,
                                   uses_nonsfi_mode, enable_ppapi_dev, process_type, imc_handle,
                                   callback);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_start_ppapi_proxy(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_start_ppapi_proxy(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_nacl_urandom_fd(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_nacl_urandom_fd();
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_are_3d_interfaces_disabled(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_nacl_are_3d_interfaces_disabled();
}

TRACE_WRAPPER
int32_t
trace_ppb_nacl_broker_duplicate_handle(PP_FileHandle source_handle, uint32_t process_id,
                                       PP_FileHandle *target_handle, uint32_t desired_access,
                                       uint32_t options)
{
    trace_info("[PPB] {zilch} %s source_handle=%d, process_id=%u, desired_access=%u, options=%u\n",
               __func__+6, source_handle, process_id, desired_access, options);
    return ppb_nacl_broker_duplicate_handle(source_handle, process_id, target_handle,
                                            desired_access, options);
}

TRACE_WRAPPER
PP_FileHandle
trace_ppb_nacl_create_temporary_file(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_create_temporary_file(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_nacl_get_number_of_processors(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_nacl_get_number_of_processors();
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_is_non_sfi_mode_enabled(void)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_nacl_is_non_sfi_mode_enabled();
}

TRACE_WRAPPER
void
trace_ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success, int32_t opt_level,
                                           int64_t pexe_size, int64_t compile_time_us)
{
    trace_info("[PPB] {zilch} %s instance=%d, success=%u, opt_level=%d, pexe_size=%"PRId64" "
               "compile_time_us=%"PRId64"\n", __func__+6, instance, success, opt_level,
               pexe_size, compile_time_us);
    return ppb_nacl_report_translation_finished(instance, success, opt_level, pexe_size,
                                                compile_time_us);
}

TRACE_WRAPPER
void
trace_ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type,
                              const char *resource_url, PP_Bool length_is_computable,
                              uint64_t loaded_bytes, uint64_t total_bytes)
{
    trace_info("[PPB] {zilch} %s instance=%d, event_type=%s, resource_url=%s, "
               "length_is_computable=%u, loaded_bytes=%"PRIu64", total_bytes=%"PRIu64"\n",
               __func__+6, instance, reverse_nacl_event_type(event_type), resource_url,
               length_is_computable, loaded_bytes, total_bytes);
    return ppb_nacl_dispatch_event(instance, event_type, resource_url, length_is_computable,
                                   loaded_bytes, total_bytes);
}

TRACE_WRAPPER
void
trace_ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error,
                                 const char *error_message)
{
    trace_info("[PPB] {zilch} %s instance=%d, error=%s, error_message=%s\n",
               __func__+6, instance, reverse_nacl_error(error), error_message);
    return ppb_nacl_report_load_error(instance, error, error_message);
}

TRACE_WRAPPER
void
trace_ppb_nacl_instance_created(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_instance_created(instance);
}

TRACE_WRAPPER
void
trace_ppb_nacl_instance_destroyed(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_instance_destroyed(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_nacl_debug_enabled_for_url(const char *alleged_nmf_url)
{
    trace_info("[PPB] {zilch} %s alleged_nmf_url=%s\n", __func__+6, alleged_nmf_url);
    return ppb_nacl_nacl_debug_enabled_for_url(alleged_nmf_url);
}

TRACE_WRAPPER
const char *
trace_ppb_nacl_get_sandbox_arch(void)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_nacl_get_sandbox_arch();
}

TRACE_WRAPPER
void
trace_ppb_nacl_log_to_console(PP_Instance instance, const char *message)
{
    trace_info("[PPB] {zilch} %s instance=%d, message=%s\n", __func__+6, instance, message);
    return ppb_nacl_log_to_console(instance, message);
}

TRACE_WRAPPER
PP_NaClReadyState
trace_ppb_nacl_get_nacl_ready_state(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_get_nacl_ready_state(instance);
}


const struct PPB_NaCl_Private_1_0 ppb_nacl_private_interface_1_0 = {
    .LaunchSelLdr =             TWRAPZ(ppb_nacl_launch_sel_ldr),
    .StartPpapiProxy =          TWRAPZ(ppb_nacl_start_ppapi_proxy),
    .UrandomFD =                TWRAPF(ppb_nacl_urandom_fd),
    .Are3DInterfacesDisabled =  TWRAPZ(ppb_nacl_are_3d_interfaces_disabled),
    .BrokerDuplicateHandle =    TWRAPZ(ppb_nacl_broker_duplicate_handle),
    .CreateTemporaryFile =      TWRAPZ(ppb_nacl_create_temporary_file),
    .GetNumberOfProcessors =    TWRAPZ(ppb_nacl_get_number_of_processors),
    .IsNonSFIModeEnabled =      TWRAPZ(ppb_nacl_is_non_sfi_mode_enabled),
    .ReportTranslationFinished = TWRAPZ(ppb_nacl_report_translation_finished),
    .DispatchEvent =            TWRAPZ(ppb_nacl_dispatch_event),
    .ReportLoadError =          TWRAPZ(ppb_nacl_report_load_error),
    .InstanceCreated =          TWRAPZ(ppb_nacl_instance_created),
    .InstanceDestroyed =        TWRAPZ(ppb_nacl_instance_destroyed),
    .NaClDebugEnabledForURL =   TWRAPZ(ppb_nacl_nacl_debug_enabled_for_url),
    .GetSandboxArch =           TWRAPF(ppb_nacl_get_sandbox_arch),
    .LogToConsole =             TWRAPZ(ppb_nacl_log_to_console),
    .GetNaClReadyState =        TWRAPZ(ppb_nacl_get_nacl_ready_state),
};
