#include "ppb_nacl_private.h"
#include <stdlib.h>
#include <ppapi/c/pp_errors.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"


void
ppb_nacl_launch_sel_ldr(PP_Instance instance, const char *alleged_url, PP_Bool uses_irt,
                        PP_Bool uses_ppapi, PP_Bool uses_nonsfi_mode, PP_Bool enable_ppapi_dev,
                        PP_Bool enable_dyncode_syscalls, PP_Bool enable_exception_handling,
                        PP_Bool enable_crash_throttling, void *imc_handle,
                        struct PP_Var *error_message, struct PP_CompletionCallback callback)
{
}

PP_ExternalPluginResult
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
ppb_nacl_get_readonly_pnacl_fd(const char *filename)
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

int32_t
ppb_nacl_get_nexe_fd(PP_Instance instance, const char *pexe_url, uint32_t abi_version,
                     uint32_t opt_level, const char *last_modified, const char *etag,
                     PP_Bool has_no_store_header, const char *sandbox_isa, const char *extra_flags,
                     PP_Bool *is_hit, PP_FileHandle *nexe_handle,
                     struct PP_CompletionCallback callback)
{
    return -1;
}

void
ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success)
{
}

PP_FileHandle
ppb_nacl_open_nacl_executable(PP_Instance instance, const char *file_url, uint64_t *file_token_lo,
                              uint64_t *file_token_hi)
{
    return -1;
}

void
ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type, const char *resource_url,
                        PP_Bool length_is_computable, uint64_t loaded_bytes, uint64_t total_bytes)
{
}

void
ppb_nacl_set_read_only_property(PP_Instance instance, struct PP_Var key, struct PP_Var value)
{
}

void
ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error, const char *error_message,
                           const char *console_message)
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
    return "amd64";
}

PP_UrlSchemeType
ppb_nacl_get_url_scheme(struct PP_Var url)
{
    return PP_SCHEME_OTHER;
}

void
ppb_nacl_log_to_console(PP_Instance instance, const char *message)
{
}

PP_Bool
ppb_nacl_get_nexe_error_reported(PP_Instance instance)
{
    return PP_FALSE;
}

void
ppb_nacl_set_nexe_error_reported(PP_Instance instance, PP_Bool error_reported)
{
}

PP_NaClReadyState
ppb_nacl_get_nacl_ready_state(PP_Instance instance)
{
    return 0;
}

void
ppb_nacl_set_nacl_ready_state(PP_Instance instance, PP_NaClReadyState ready_state)
{
}

PP_Bool
ppb_nacl_get_is_installed(PP_Instance instance)
{
    return PP_TRUE;
}

void
ppb_nacl_set_is_installed(PP_Instance instance, PP_Bool is_installed)
{
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_nacl_launch_sel_ldr(PP_Instance instance, const char *alleged_url, PP_Bool uses_irt,
                              PP_Bool uses_ppapi, PP_Bool uses_nonsfi_mode,
                              PP_Bool enable_ppapi_dev, PP_Bool enable_dyncode_syscalls,
                              PP_Bool enable_exception_handling, PP_Bool enable_crash_throttling,
                              void *imc_handle, struct PP_Var *error_message,
                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s instance=%d, alleged_url=%s, uses_irt=%d, uses_ppapi=%d, "
               "uses_nonsfi_mode=%d, enable_ppapi_dev=%d, enable_dyncode_syscalls=%d, "
               "enable_exception_handling=%d, enable_crash_throttling=%d, imc_handle=%p, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, instance,
               alleged_url, uses_irt, uses_ppapi, uses_nonsfi_mode, enable_ppapi_dev,
               enable_dyncode_syscalls, enable_exception_handling, enable_crash_throttling,
               imc_handle, callback.func, callback.user_data, callback.flags);
    return ppb_nacl_launch_sel_ldr(instance, alleged_url, uses_irt, uses_ppapi, uses_nonsfi_mode,
                                   enable_ppapi_dev, enable_dyncode_syscalls,
                                   enable_exception_handling, enable_crash_throttling, imc_handle,
                                   error_message, callback);
}

TRACE_WRAPPER
PP_ExternalPluginResult
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
trace_ppb_nacl_get_readonly_pnacl_fd(const char *filename)
{
    trace_info("[PPB] {zilch} %s filename=%s\n", __func__+6, filename);
    return ppb_nacl_get_readonly_pnacl_fd(filename);
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
int32_t
trace_ppb_nacl_get_nexe_fd(PP_Instance instance, const char *pexe_url, uint32_t abi_version,
                           uint32_t opt_level, const char *last_modified, const char *etag,
                           PP_Bool has_no_store_header, const char *sandbox_isa,
                           const char *extra_flags, PP_Bool *is_hit, PP_FileHandle *nexe_handle,
                           struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s instance=%d, pexe_url=%s, abi_version=%u, opt_level=%u, "
               "last_modified=%s, etag=%s, has_no_store_header=%d, sandbox_isa=%s, "
               "extra_flags=%s, callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6,
               instance, pexe_url, abi_version, opt_level, last_modified, etag, has_no_store_header,
               sandbox_isa, extra_flags, callback.func, callback.user_data, callback.flags);
    return ppb_nacl_get_nexe_fd(instance, pexe_url, abi_version, opt_level, last_modified, etag,
                                has_no_store_header, sandbox_isa, extra_flags, is_hit, nexe_handle,
                                callback);
}

TRACE_WRAPPER
void
trace_ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success)
{
    trace_info("[PPB] {zilch} %s instance=%d, success=%u\n", __func__+6, instance, success);
    return ppb_nacl_report_translation_finished(instance, success);
}

TRACE_WRAPPER
PP_FileHandle
trace_ppb_nacl_open_nacl_executable(PP_Instance instance, const char *file_url,
                                    uint64_t *file_token_lo, uint64_t *file_token_hi)
{
    trace_info("[PPB] {zilch} %s instance=%d, file_url=%s\n", __func__+6, instance, file_url);
    return ppb_nacl_open_nacl_executable(instance, file_url, file_token_lo, file_token_hi);
}

TRACE_WRAPPER
void
trace_ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type,
                              const char *resource_url, PP_Bool length_is_computable,
                              uint64_t loaded_bytes, uint64_t total_bytes)
{
    trace_info("[PPB] {zilch} %s instance=%d, event_type=%s, resource_url=%s, "
               "length_is_computable=%u, loaded_bytes=%lu, total_bytes=%lu\n", __func__+6,
               instance, reverse_nacl_event_type(event_type), resource_url, length_is_computable,
               loaded_bytes, total_bytes);
    return ppb_nacl_dispatch_event(instance, event_type, resource_url, length_is_computable,
                                   loaded_bytes, total_bytes);
}

TRACE_WRAPPER
void
trace_ppb_nacl_set_read_only_property(PP_Instance instance, struct PP_Var key, struct PP_Var value)
{
    gchar *s_key = trace_var_as_string(key);
    gchar *s_value = trace_var_as_string(value);
    trace_info("[PPB] {zilch} %s instance=%d, key=%s, value=%s\n", __func__+6, instance,
               s_key, s_value);
    g_free(s_key);
    g_free(s_value);
    return ppb_nacl_set_read_only_property(instance, key, value);
}

TRACE_WRAPPER
void
trace_ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error,
                                 const char *error_message, const char *console_message)
{
    trace_info("[PPB] {zilch} %s instance=%d, error=%s, error_message=%s, console_message=%s\n",
               __func__+6, instance, reverse_nacl_error(error), error_message, console_message);
    return ppb_nacl_report_load_error(instance, error, error_message, console_message);
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
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_nacl_get_sandbox_arch();
}

TRACE_WRAPPER
PP_UrlSchemeType
trace_ppb_nacl_get_url_scheme(struct PP_Var url)
{
    gchar *s_url = trace_var_as_string(url);
    trace_info("[PPB] {zilch} %s url=%s\n", __func__+6, s_url);
    g_free(s_url);
    return ppb_nacl_get_url_scheme(url);
}

TRACE_WRAPPER
void
trace_ppb_nacl_log_to_console(PP_Instance instance, const char *message)
{
    trace_info("[PPB] {zilch} %s instance=%d, message=%s\n", __func__+6, instance, message);
    return ppb_nacl_log_to_console(instance, message);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_get_nexe_error_reported(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_get_nexe_error_reported(instance);
}

TRACE_WRAPPER
void
trace_ppb_nacl_set_nexe_error_reported(PP_Instance instance, PP_Bool error_reported)
{
    trace_info("[PPB] {zilch} %s instance=%d, error_reported=%u\n", __func__+6,
               instance, error_reported);
    return ppb_nacl_set_nexe_error_reported(instance, error_reported);
}

TRACE_WRAPPER
PP_NaClReadyState
trace_ppb_nacl_get_nacl_ready_state(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_get_nacl_ready_state(instance);
}

TRACE_WRAPPER
void
trace_ppb_nacl_set_nacl_ready_state(PP_Instance instance, PP_NaClReadyState ready_state)
{
    trace_info("[PPB] {zilch} %s instance=%d, ready_state=%s\n", __func__+6,
               instance, reverse_nacl_ready_state(ready_state));
    return ppb_nacl_set_nacl_ready_state(instance, ready_state);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_nacl_get_is_installed(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_nacl_get_is_installed(instance);
}

TRACE_WRAPPER
void
trace_ppb_nacl_set_is_installed(PP_Instance instance, PP_Bool is_installed)
{
    trace_info("[PPB] {zilch} %s instance=%d, is_installed=%u\n", __func__+6,
               instance, is_installed);
    return ppb_nacl_set_is_installed(instance, is_installed);
}


const struct PPB_NaCl_Private_1_0 ppb_nacl_private_interface_1_0 = {
    .LaunchSelLdr =             TWRAPZ(ppb_nacl_launch_sel_ldr),
    .StartPpapiProxy =          TWRAPZ(ppb_nacl_start_ppapi_proxy),
    .UrandomFD =                TWRAPF(ppb_nacl_urandom_fd),
    .Are3DInterfacesDisabled =  TWRAPZ(ppb_nacl_are_3d_interfaces_disabled),
    .BrokerDuplicateHandle =    TWRAPZ(ppb_nacl_broker_duplicate_handle),
    .GetReadonlyPnaclFd =       TWRAPZ(ppb_nacl_get_readonly_pnacl_fd),
    .CreateTemporaryFile =      TWRAPZ(ppb_nacl_create_temporary_file),
    .GetNumberOfProcessors =    TWRAPZ(ppb_nacl_get_number_of_processors),
    .IsNonSFIModeEnabled =      TWRAPZ(ppb_nacl_is_non_sfi_mode_enabled),
    .GetNexeFd =                TWRAPZ(ppb_nacl_get_nexe_fd),
    .ReportTranslationFinished = TWRAPZ(ppb_nacl_report_translation_finished),
    .OpenNaClExecutable =       TWRAPZ(ppb_nacl_open_nacl_executable),
    .DispatchEvent =            TWRAPZ(ppb_nacl_dispatch_event),
    .SetReadOnlyProperty =      TWRAPZ(ppb_nacl_set_read_only_property),
    .ReportLoadError =          TWRAPZ(ppb_nacl_report_load_error),
    .InstanceCreated =          TWRAPZ(ppb_nacl_instance_created),
    .InstanceDestroyed =        TWRAPZ(ppb_nacl_instance_destroyed),
    .NaClDebugEnabledForURL =   TWRAPZ(ppb_nacl_nacl_debug_enabled_for_url),
    .GetSandboxArch =           TWRAPZ(ppb_nacl_get_sandbox_arch),
    .GetUrlScheme =             TWRAPZ(ppb_nacl_get_url_scheme),
    .LogToConsole =             TWRAPZ(ppb_nacl_log_to_console),
    .GetNexeErrorReported =     TWRAPZ(ppb_nacl_get_nexe_error_reported),
    .SetNexeErrorReported =     TWRAPZ(ppb_nacl_set_nexe_error_reported),
    .GetNaClReadyState =        TWRAPZ(ppb_nacl_get_nacl_ready_state),
    .SetNaClReadyState =        TWRAPZ(ppb_nacl_set_nacl_ready_state),
    .GetIsInstalled =           TWRAPZ(ppb_nacl_get_is_installed),
    .SetIsInstalled =           TWRAPZ(ppb_nacl_set_is_installed),
};
