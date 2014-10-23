/* Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/* From ppb_file_mapping.idl modified Mon Jan 27 11:00:43 2014. */

#ifndef PPAPI_C_PPB_FILE_MAPPING_H_
#define PPAPI_C_PPB_FILE_MAPPING_H_

#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_instance.h"
#include "ppapi/c/pp_macros.h"
#include "ppapi/c/pp_resource.h"
#include "ppapi/c/pp_stdint.h"

#define PPB_FILEMAPPING_INTERFACE_0_1 "PPB_FileMapping;0.1" /* dev */
/**
 * @file
 * This file defines methods for mapping and unmapping files into and out of
 * memory.
 */


/**
 * @addtogroup Enums
 * @{
 */
/**
 * The PP_FileMapProtection values indicate the permissions requested for the
 * file mapping. These should be used in a uint32_t bitfield.
 */
typedef enum {
  /** Requests read access to the mapped address. */
  PP_FILEMAPPROTECTION_READ = 1u << 0,
  /** Requests write access to the mapped address. */
  PP_FILEMAPPROTECTION_WRITE = 1u << 1
} PP_FileMapProtection;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_FileMapProtection, 4);

/**
 * The PP_FileMapFlags contain flag values for use with Map().
 */
typedef enum {
  /**
   * Requests a shared mapping. If this flag is set, changes written to the
   * memory region will be reflected in the underlying file and will thus
   * eventually be visible to other processes which have opened the file. The
   * file may not actually be updated until Unmap() is called. This is only
   * valid if the PPB_FileIO resource was opened with write permission.
   */
  PP_FILEMAPFLAG_SHARED = 1u << 0,
  /**
   * Requests a copy-on-write mapping. If this flag is set, changes are not
   * written to the underlying file, but only in the memory of the process
   * (copy-on-write).
   */
  PP_FILEMAPFLAG_PRIVATE = 1u << 1,
  /**
   * Forces Map() to map the file contents at the provided |address|. If Map()
   * can not comply, Map() will fail.
   */
  PP_FILEMAPFLAG_FIXED = 1u << 2
} PP_FileMapFlags;
PP_COMPILE_ASSERT_SIZE_IN_BYTES(PP_FileMapFlags, 4);
/**
 * @}
 */

/**
 * @addtogroup Interfaces
 * @{
 */
/**
 *  PPB_FileMapping contains functions for mapping and unmapping files into and
 *  out of memory.
 */
struct PPB_FileMapping_0_1 { /* dev */
  /**
   * Map() maps the contents from an offset of the file into memory.
   *
   * @param[in] instance A <code>PP_Instance</code> identifying one instance of
   * a module.
   * @param[in] file_io A <code>PPB_FileIO</code> <code>PP_Resource</code>
   * corresponding to the file that should be mapped in to memory.
   * @param[in] length The number of bytes to map.
   * @param[in] map_protection A bitfield containing values from
   * <code>PP_FileMapProtection</code>, indicating what memory operations
   * should be permitted on the mapped region.
   * @param[in] map_flags A bitfield containing values from
   * <code>PP_FileMapFlags</code>, providing options for the behavior of Map.
   * If the region is to be writeable, then exactly one of
   * <code>PP_FILEMAPFLAG_SHARED</code> or <code>PP_FILEMAPFLAG_PRIVATE</code>
   * must be set.
   * @param[in] offset The offset into the file. Must be a multiple of the
   * Map page size as returned by GetMapPageSize().
   * @param[inout] address The value of <code>*address</code>, if non-NULL,
   * will be used as a hint to determine where in memory the file should be
   * mapped. If the value is NULL, the host operating system will choose
   * <code>address</code>. Upon Map() completing, <code>*address</code> will
   * contain the actual memory location at which the file was mapped. If the
   * plugin provides a non-NULL <code>*address</code>, it must be a multiple of
   * the map page size as returned by GetMapPageSize().
   * @param[in] callback A <code>PP_CompletionCallback</code> to be called upon
   * completion of Map().
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   */
  int32_t (*Map)(PP_Instance instance,
                 PP_Resource file_io,
                 int64_t length,
                 uint32_t map_protection,
                 uint32_t map_flags,
                 int64_t offset,
                 void** address,
                 struct PP_CompletionCallback callback);
  /**
   * Unmap() deletes the mapping of the specified address.  The specified
   * address must have been retrieved with Map().
   * @param[in] instance A <code>PP_Instance</code> identifying the instance.
   * @param[in] address The starting address of the address in memory to
   * be unmapped.
   * @param[in] length The length of the region to unmap.
   * @param[in] callback A <code>PP_CompletionCallback</code> to be called upon
   * completion of Unmap().
   *
   * @return An int32_t containing an error code from <code>pp_errors.h</code>.
   */
  int32_t (*Unmap)(PP_Instance instance,
                   const void* address,
                   int64_t length,
                   struct PP_CompletionCallback callback);
  /**
   * GetMapPageSize() retrieves the size of pages that Map() uses.
   *
   * @param[in] instance A <code>PP_Instance</code> identifying the instance.
   *
   * @return The size of pages that Map() uses. Returns 0 on failure.
   */
  int64_t (*GetMapPageSize)(PP_Instance instance);
};
/**
 * @}
 */

#endif  /* PPAPI_C_PPB_FILE_MAPPING_H_ */

