/* Copyright (c) 2012 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From private/ppb_nacl_private.idl modified Thu Mar 27 14:44:04 2014. */

#ifndef PPAPI_C_PRIVATE_PPB_NACL_PRIVATE_H_
#define PPAPI_C_PRIVATE_PPB_NACL_PRIVATE_H_

#include "ppapi/c/pp_bool.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/private/ppb_instance_private.h"

#define PPB_NACL_PRIVATE_INTERFACE_1_0 "PPB_NaCl_Private;1.0"
#define PPB_NACL_PRIVATE_INTERFACE PPB_NACL_PRIVATE_INTERFACE_1_0

/**
 * @file
 * This file contains NaCl private interfaces. This interface is not versioned
 * and is for internal Chrome use. It may change without notice. */


#include "ppapi/c/private/pp_file_handle.h"
#include "ppapi/c/private/ppb_instance_private.h"

/**
 * @addtogroup Enums
 * @{
 */
/** NaCl-specific errors that should be reported to the user.
 *  These error codes are reported via UMA so, if you edit them:
 *   1) make sure you understand UMA first.
 *   2) update src/tools/metrics/histograms/histograms.xml
 *  Values are explicitly specified to make sure they don't shift around when
 *  edited, and also to make reading about:histograms easier.
 */
typedef enum {
  PP_NACL_ERROR_LOAD_SUCCESS = 0,
  PP_NACL_ERROR_LOAD_ABORTED = 1,
  PP_NACL_ERROR_UNKNOWN = 2,
  PP_NACL_ERROR_MANIFEST_RESOLVE_URL = 3,
  PP_NACL_ERROR_MANIFEST_LOAD_URL = 4,
  PP_NACL_ERROR_MANIFEST_STAT = 5,
  PP_NACL_ERROR_MANIFEST_TOO_LARGE = 6,
  PP_NACL_ERROR_MANIFEST_OPEN = 7,
  PP_NACL_ERROR_MANIFEST_MEMORY_ALLOC = 8,
  PP_NACL_ERROR_MANIFEST_READ = 9,
  PP_NACL_ERROR_MANIFEST_PARSING = 10,
  PP_NACL_ERROR_MANIFEST_SCHEMA_VALIDATE = 11,
  PP_NACL_ERROR_MANIFEST_GET_NEXE_URL = 12,
  PP_NACL_ERROR_NEXE_LOAD_URL = 13,
  PP_NACL_ERROR_NEXE_ORIGIN_PROTOCOL = 14,
  PP_NACL_ERROR_NEXE_FH_DUP = 15,
  PP_NACL_ERROR_NEXE_STAT = 16,
  PP_NACL_ERROR_ELF_CHECK_IO = 17,
  PP_NACL_ERROR_ELF_CHECK_FAIL = 18,
  PP_NACL_ERROR_SEL_LDR_INIT = 19,
  PP_NACL_ERROR_SEL_LDR_CREATE_LAUNCHER = 20,
  PP_NACL_ERROR_SEL_LDR_FD = 21,
  PP_NACL_ERROR_SEL_LDR_LAUNCH = 22,
  /* Deprecated, safe to reuse the value because it's never logged in UMA.
   */
  PP_NACL_ERROR_SEL_LDR_SEND_NEXE = 24,
  PP_NACL_ERROR_SEL_LDR_HANDLE_PASSING = 25,
  PP_NACL_ERROR_SEL_LDR_START_MODULE = 26,
  PP_NACL_ERROR_SEL_LDR_START_STATUS = 27,
  PP_NACL_ERROR_SRPC_CONNECTION_FAIL = 28,
  PP_NACL_ERROR_START_PROXY_CHECK_PPP = 29,
  PP_NACL_ERROR_START_PROXY_ALLOC = 30,
  PP_NACL_ERROR_START_PROXY_MODULE = 31,
  PP_NACL_ERROR_START_PROXY_INSTANCE = 32,
  PP_NACL_ERROR_SEL_LDR_COMMUNICATION_CMD_CHANNEL = 33,
  PP_NACL_ERROR_SEL_LDR_COMMUNICATION_REV_SETUP = 34,
  PP_NACL_ERROR_SEL_LDR_COMMUNICATION_WRAPPER = 35,
  PP_NACL_ERROR_SEL_LDR_COMMUNICATION_REV_SERVICE = 36,
  PP_NACL_ERROR_START_PROXY_CRASH = 37,
  PP_NACL_ERROR_MANIFEST_PROGRAM_MISSING_ARCH = 38,
  PP_NACL_ERROR_PNACL_CACHE_OPEN_INPROGRESS = 39,
  PP_NACL_ERROR_PNACL_CACHE_OPEN_NOACCESS = 40,
  PP_NACL_ERROR_PNACL_CACHE_OPEN_NOQUOTA = 41,
  PP_NACL_ERROR_PNACL_CACHE_OPEN_NOSPACE = 42,
  PP_NACL_ERROR_PNACL_CACHE_OPEN_OTHER = 43,
  PP_NACL_ERROR_PNACL_CACHE_DIRECTORY_CREATE = 44,
  PP_NACL_ERROR_PNACL_CACHE_FILEOPEN_NOACCESS = 45,
  PP_NACL_ERROR_PNACL_CACHE_FILEOPEN_NOQUOTA = 46,
  PP_NACL_ERROR_PNACL_CACHE_FILEOPEN_NOSPACE = 47,
  PP_NACL_ERROR_PNACL_CACHE_FILEOPEN_NOTAFILE = 48,
  PP_NACL_ERROR_PNACL_CACHE_FILEOPEN_OTHER = 49,
  PP_NACL_ERROR_PNACL_CACHE_FETCH_NOACCESS = 50,
  PP_NACL_ERROR_PNACL_CACHE_FETCH_NOTFOUND = 51,
  PP_NACL_ERROR_PNACL_CACHE_FETCH_OTHER = 52,
  PP_NACL_ERROR_PNACL_CACHE_FINALIZE_COPY_NOQUOTA = 53,
  PP_NACL_ERROR_PNACL_CACHE_FINALIZE_COPY_NOSPACE = 54,
  PP_NACL_ERROR_PNACL_CACHE_FINALIZE_COPY_OTHER = 55,
  PP_NACL_ERROR_PNACL_CACHE_FINALIZE_RENAME_NOACCESS = 56,
  PP_NACL_ERROR_PNACL_CACHE_FINALIZE_RENAME_OTHER = 57,
  PP_NACL_ERROR_PNACL_RESOURCE_FETCH = 58,
  PP_NACL_ERROR_PNACL_PEXE_FETCH_ABORTED = 59,
  PP_NACL_ERROR_PNACL_PEXE_FETCH_NOACCESS = 60,
  PP_NACL_ERROR_PNACL_PEXE_FETCH_OTHER = 61,
  PP_NACL_ERROR_PNACL_THREAD_CREATE = 62,
  PP_NACL_ERROR_PNACL_LLC_SETUP = 63,
  PP_NACL_ERROR_PNACL_LD_SETUP = 64,
  PP_NACL_ERROR_PNACL_LLC_INTERNAL = 65,
  PP_NACL_ERROR_PNACL_LD_INTERNAL = 66,
  PP_NACL_ERROR_PNACL_CREATE_TEMP = 67,
  /* This entry is no longer used, but should not be removed, because UMA
   */
  PP_NACL_ERROR_PNACL_NOT_ENABLED = 68,
  PP_NACL_ERROR_MANIFEST_NOACCESS_URL = 69,
  PP_NACL_ERROR_NEXE_NOACCESS_URL = 70,
  PP_NACL_ERROR_PNACL_CRASH_THROTTLED = 71,
  /* If you add a code, read the enum comment above on how to update
   */
  PP_NACL_ERROR_MAX
} PP_NaClError;

/** Event types that NaCl may use when reporting load progress or errors. */
typedef enum {
  PP_NACL_EVENT_LOADSTART,
  PP_NACL_EVENT_PROGRESS,
  PP_NACL_EVENT_ERROR,
  PP_NACL_EVENT_ABORT,
  PP_NACL_EVENT_LOAD,
  PP_NACL_EVENT_LOADEND,
  PP_NACL_EVENT_CRASH
} PP_NaClEventType;

typedef enum {
  PP_SCHEME_CHROME_EXTENSION,
  PP_SCHEME_DATA,
  PP_SCHEME_OTHER
} PP_UrlSchemeType;

typedef enum {
  /* The trusted plugin begins in this ready state. */
  PP_NACL_READY_STATE_UNSENT = 0,
  /* The manifest file has been requested, but not yet received. */
  PP_NACL_READY_STATE_OPENED = 1,
  /* The manifest file has been received and the nexe successfully requested. */
  PP_NACL_READY_STATE_LOADING = 3,
  /* The nexe has been loaded and the proxy started, so it is ready for
   */
  PP_NACL_READY_STATE_DONE = 4
} PP_NaClReadyState;
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/* PPB_NaCl_Private */
struct PPB_NaCl_Private_1_0 {
  /* Launches NaCl's sel_ldr process.  Returns PP_EXTERNAL_PLUGIN_OK on success
   * and writes a NaClHandle to imc_handle. Returns PP_EXTERNAL_PLUGIN_FAILED on
   * failure. The |enable_ppapi_dev| parameter controls whether GetInterface
   * returns 'Dev' interfaces to the NaCl plugin.  The |uses_ppapi| flag
   * indicates that the nexe run by sel_ldr will use the PPAPI APIs.
   * This implies that LaunchSelLdr is run from the main thread.  If a nexe
   * does not need PPAPI, then it can run off the main thread.
   * The |uses_irt| flag indicates whether the IRT should be loaded in this
   * NaCl process.  This is true for ABI stable nexes.
   * The |uses_nonsfi_mode| flag indicates whether or not nonsfi-mode should
   * be used with the binary pointed by the url.
   * The |enable_dyncode_syscalls| flag indicates whether or not the nexe
   * will be able to use dynamic code system calls (e.g., mmap with PROT_EXEC).
   * The |enable_exception_handling| flag indicates whether or not the nexe
   * will be able to use hardware exception handling.
   * The |enable_crash_throttling| flag indicates whether or not crashes of
   * the nexe contribute to crash throttling statisics and whether nexe starts
   * are throttled by crash throttling.
   */
  void (*LaunchSelLdr)(PP_Instance instance,
                       const char* alleged_url,
                       PP_Bool uses_irt,
                       PP_Bool uses_ppapi,
                       PP_Bool uses_nonsfi_mode,
                       PP_Bool enable_ppapi_dev,
                       PP_Bool enable_dyncode_syscalls,
                       PP_Bool enable_exception_handling,
                       PP_Bool enable_crash_throttling,
                       void* imc_handle,
                       struct PP_Var* error_message,
                       struct PP_CompletionCallback callback);
  /* This function starts the IPC proxy so the nexe can communicate with the
   * browser. Returns PP_EXTERNAL_PLUGIN_OK on success, otherwise a result code
   * indicating the failure. PP_EXTERNAL_PLUGIN_FAILED is returned if
   * LaunchSelLdr wasn't called with the instance.
   * PP_EXTERNAL_PLUGIN_ERROR_MODULE is returned if the module can't be
   * initialized. PP_EXTERNAL_PLUGIN_ERROR_INSTANCE is returned if the instance
   * can't be initialized.
   */
  PP_ExternalPluginResult (*StartPpapiProxy)(PP_Instance instance);
  /* On POSIX systems, this function returns the file descriptor of
   * /dev/urandom.  On non-POSIX systems, this function returns 0.
   */
  int32_t (*UrandomFD)(void);
  /* Whether the Pepper 3D interfaces should be disabled in the NaCl PPAPI
   * proxy. This is so paranoid admins can effectively prevent untrusted shader
   * code to be processed by the graphics stack.
   */
  PP_Bool (*Are3DInterfacesDisabled)(void);
  /* This is Windows-specific.  This is a replacement for DuplicateHandle() for
   * use inside the Windows sandbox.  Note that we provide this via dependency
   * injection only to avoid the linkage problems that occur because the NaCl
   * plugin is built as a separate DLL/DSO
   * (see http://code.google.com/p/chromium/issues/detail?id=114439#c8).
   */
  int32_t (*BrokerDuplicateHandle)(PP_FileHandle source_handle,
                                   uint32_t process_id,
                                   PP_FileHandle* target_handle,
                                   uint32_t desired_access,
                                   uint32_t options);
  /* Returns a read-only file descriptor of a file rooted in the Pnacl
   * component directory, or an invalid handle on failure.
   */
  PP_FileHandle (*GetReadonlyPnaclFd)(const char* filename);
  /* This creates a temporary file that will be deleted by the time
   * the last handle is closed (or earlier on POSIX systems), and
   * returns a posix handle to that temporary file.
   */
  PP_FileHandle (*CreateTemporaryFile)(PP_Instance instance);
  /* Return the number of processors in the system as reported by the OS */
  int32_t (*GetNumberOfProcessors)(void);
  /* Return whether the non-SFI mode is enabled. */
  PP_Bool (*IsNonSFIModeEnabled)(void);
  /* Create a temporary file, which will be deleted by the time the
   * last handle is closed (or earlier on POSIX systems), to use for
   * the nexe with the cache information given by |pexe_url|,
   * |abi_version|, |opt_level|, |last_modified|, |etag|, and
   * |has_no_store_header|. If the nexe is already present in the
   * cache, |is_hit| is set to PP_TRUE and the contents of the nexe
   * will be copied into the temporary file. Otherwise |is_hit| is set
   * to PP_FALSE and the temporary file will be writeable.  Currently
   * the implementation is a stub, which always sets is_hit to false
   * and calls the implementation of CreateTemporaryFile. In a
   * subsequent CL it will call into the browser which will remember
   * the association between the cache key and the fd, and copy the
   * nexe into the cache after the translation finishes.
   */
  int32_t (*GetNexeFd)(PP_Instance instance,
                       const char* pexe_url,
                       uint32_t abi_version,
                       uint32_t opt_level,
                       const char* last_modified,
                       const char* etag,
                       PP_Bool has_no_store_header,
                       const char* sandbox_isa,
                       const char* extra_flags,
                       PP_Bool* is_hit,
                       PP_FileHandle* nexe_handle,
                       struct PP_CompletionCallback callback);
  /* Report to the browser that translation of the pexe for |instance|
   * has finished, or aborted with an error. If |success| is true, the
   * browser may then store the translation in the cache. The renderer
   * must first have called GetNexeFd for the same instance. (The browser is
   * not guaranteed to store the nexe even if |success| is true; if there is
   * an error on the browser side, or the file is too big for the cache, or
   * the browser is in incognito mode, no notification will be delivered to
   * the plugin.)
   */
  void (*ReportTranslationFinished)(PP_Instance instance, PP_Bool success);
  /* Opens a NaCl executable file in the application's extension directory
   * corresponding to the file URL and returns a file descriptor, or an invalid
   * handle on failure. |metadata| is left unchanged on failure.
   */
  PP_FileHandle (*OpenNaClExecutable)(PP_Instance instance,
                                      const char* file_url,
                                      uint64_t* file_token_lo,
                                      uint64_t* file_token_hi);
  /* Dispatch a progress event on the DOM element where the given instance is
   * embedded.
   */
  void (*DispatchEvent)(PP_Instance instance,
                        PP_NaClEventType event_type,
                        const char* resource_url,
                        PP_Bool length_is_computable,
                        uint64_t loaded_bytes,
                        uint64_t total_bytes);
  /* Sets a read-only property on the <embed> DOM element that corresponds to
   * the given instance.
   */
  void (*SetReadOnlyProperty)(PP_Instance instance,
                              struct PP_Var key,
                              struct PP_Var value);
  /* Report an error that occured while attempting to load a nexe. */
  void (*ReportLoadError)(PP_Instance instance,
                          PP_NaClError error,
                          const char* error_message,
                          const char* console_message);
  /* Performs internal setup when an instance is created. */
  void (*InstanceCreated)(PP_Instance instance);
  /* Performs internal cleanup when an instance is destroyed. */
  void (*InstanceDestroyed)(PP_Instance instance);
  /* Return true if the NaCl debug stub is enabled and the app loaded from
   * alleged_nmf_url will be attached to a debugger.
   */
  PP_Bool (*NaClDebugEnabledForURL)(const char* alleged_nmf_url);
  /* Returns the kind of SFI sandbox implemented by NaCl on this
   * platform.
   */
  const char* (*GetSandboxArch)(void);
  /* Returns the scheme type for a given url. */
  PP_UrlSchemeType (*GetUrlScheme)(struct PP_Var url);
  /* Logs the message to the console. */
  void (*LogToConsole)(PP_Instance instance, const char* message);
  /* Returns PP_TRUE if an error has been reported loading the nexe. */
  PP_Bool (*GetNexeErrorReported)(PP_Instance instance);
  /* Sets the nexe error reported state for this instance. */
  void (*SetNexeErrorReported)(PP_Instance instance, PP_Bool error_reported);
  /* Returns the NaCl readiness status for this instance. */
  PP_NaClReadyState (*GetNaClReadyState)(PP_Instance instance);
  /* Sets the NaCl readiness status for this instance. */
  void (*SetNaClReadyState)(PP_Instance instance,
                            PP_NaClReadyState ready_state);
  /* Returns true if the plugin is an installed app. */
  PP_Bool (*GetIsInstalled)(PP_Instance instance);
  /* Sets whether the plugin is an installed app. */
  void (*SetIsInstalled)(PP_Instance instance, PP_Bool is_installed);
};

typedef struct PPB_NaCl_Private_1_0 PPB_NaCl_Private;
/**
 * @}
 */

#endif  /* PPAPI_C_PRIVATE_PPB_NACL_PRIVATE_H_ */

