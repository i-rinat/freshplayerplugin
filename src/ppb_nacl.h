/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#ifndef FPP_PPB_NACL_H
#define FPP_PPB_NACL_H

#include <ppapi/c/private/ppb_nacl_private.h>


void
ppb_nacl_launch_sel_ldr(PP_Instance instance, PP_Bool main_service_runtime, const char *alleged_url,
                        const struct PP_NaClFileInfo *nexe_file_info, PP_Bool uses_irt,
                        PP_Bool uses_ppapi, PP_Bool uses_nonsfi_mode, PP_Bool enable_ppapi_dev,
                        PP_Bool enable_dyncode_syscalls, PP_Bool enable_exception_handling,
                        PP_Bool enable_crash_throttling, void *imc_handle,
                        struct PP_CompletionCallback callback);

PP_Bool
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
ppb_nacl_create_temporary_file(PP_Instance instance);

int32_t
ppb_nacl_get_number_of_processors(void);

PP_Bool
ppb_nacl_is_non_sfi_mode_enabled(void);

void
ppb_nacl_report_translation_finished(PP_Instance instance, PP_Bool success, int32_t opt_level,
                                     int64_t pexe_size, int64_t compile_time_us);

void
ppb_nacl_dispatch_event(PP_Instance instance, PP_NaClEventType event_type,
                        const char *resource_url, PP_Bool length_is_computable,
                        uint64_t loaded_bytes, uint64_t total_bytes);

void
ppb_nacl_report_load_error(PP_Instance instance, PP_NaClError error, const char *error_message);

void
ppb_nacl_instance_created(PP_Instance instance);

void
ppb_nacl_instance_destroyed(PP_Instance instance);

PP_Bool
ppb_nacl_nacl_debug_enabled_for_url(const char *alleged_nmf_url);

const char *
ppb_nacl_get_sandbox_arch(void);

void
ppb_nacl_log_to_console(PP_Instance instance, const char *message);

PP_NaClReadyState
ppb_nacl_get_nacl_ready_state(PP_Instance instance);

#endif // FPP_PPB_NACL_H
