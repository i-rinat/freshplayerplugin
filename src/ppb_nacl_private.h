#ifndef FPP__PPB_NACL_PRIVATE_H
#define FPP__PPB_NACL_PRIVATE_H

#include <ppapi/c/private/ppb_nacl_private.h>


void
ppb_nacl_launch_sel_ldr(PP_Instance instance, const char *alleged_url, PP_Bool uses_irt,
                        PP_Bool uses_ppapi, PP_Bool uses_nonsfi_mode, PP_Bool enable_ppapi_dev,
                        PP_Bool enable_dyncode_syscalls, PP_Bool enable_exception_handling,
                        PP_Bool enable_crash_throttling, void *imc_handle,
                        struct PP_Var *error_message, struct PP_CompletionCallback callback);

PP_ExternalPluginResult
ppb_nacl_start_ppapi_proxy(PP_Instance instance);

int32_t
ppb_nacl_urandom_fd(void);

PP_Bool
ppb_nacl_are_3d_interfaces_disabled(void);

int32_t
ppb_nacl_broker_duplicate_handle(PP_FileHandle source_handle, uint32_t process_id,
                                 PP_FileHandle *target_handle, uint32_t desired_access,
                                 uint32_t options);

PP_FileHandle
ppb_nacl_get_readonly_pnacl_fd(const char *filename);

PP_FileHandle
ppb_nacl_create_temporary_file(PP_Instance instance);

int32_t
ppb_nacl_get_number_of_processors(void);

PP_Bool
ppb_nacl_is_non_sfi_mode_enabled(void);

int32_t
ppb_nacl_get_nexe_fd(PP_Instance instance, const char *pexe_url, uint32_t abi_version,
                     uint32_t opt_level, const char *last_modified, const char *etag,
                     PP_Bool has_no_store_header, const char *sandbox_isa, const char *extra_flags,
                     PP_Bool *is_hit, PP_FileHandle *nexe_handle,
                     struct PP_CompletionCallback callback);

void
ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success);

PP_FileHandle
ppb_nacl_open_nacl_executable(PP_Instance instance, const char *file_url, uint64_t *file_token_lo,
                              uint64_t *file_token_hi);

void
ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type,
                        const char *resource_url, PP_Bool length_is_computable,
                        uint64_t loaded_bytes, uint64_t total_bytes);

void
ppb_nacl_set_read_only_property(PP_Instance instance, struct PP_Var key, struct PP_Var value);

void
ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error, const char *error_message,
                           const char *console_message);

void
ppb_nacl_instance_created(PP_Instance instance);

void
ppb_nacl_instance_destroyed(PP_Instance instance);

PP_Bool
ppb_nacl_nacl_debug_enabled_for_url(const char *alleged_nmf_url);

const char *
ppb_nacl_get_sandbox_arch(void);

PP_UrlSchemeType
ppb_nacl_get_url_scheme(struct PP_Var url);

void
ppb_nacl_log_to_console(PP_Instance instance, const char *message);

PP_Bool
ppb_nacl_get_nexe_error_reported(PP_Instance instance);

void
ppb_nacl_set_nexe_error_reported(PP_Instance instance, PP_Bool error_reported);

PP_NaClReadyState
ppb_nacl_get_nacl_ready_state(PP_Instance instance);

void
ppb_nacl_set_nacl_ready_state(PP_Instance instance, PP_NaClReadyState ready_state);

PP_Bool
ppb_nacl_get_is_installed(PP_Instance instance);

void
ppb_nacl_set_is_installed(PP_Instance instance, PP_Bool is_installed);

#endif // FPP__PPB_NACL_PRIVATE_H
