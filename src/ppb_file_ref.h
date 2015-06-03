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

#ifndef FPP_PPB_FILE_REF_H
#define FPP_PPB_FILE_REF_H

#include <ppapi/c/ppb_file_ref.h>


PP_Resource
ppb_file_ref_create(PP_Resource file_system, const char *path);

/// can access any file, outside plugin's filesystem
///
/// this function is not exported in any PPB interface
PP_Resource
ppb_file_ref_create_unrestricted(const char *path, int read_only);

PP_Bool
ppb_file_ref_is_file_ref(PP_Resource resource);

PP_FileSystemType
ppb_file_ref_get_file_system_type(PP_Resource file_ref);

struct PP_Var
ppb_file_ref_get_name(PP_Resource file_ref);

struct PP_Var
ppb_file_ref_get_path(PP_Resource file_ref);

PP_Resource
ppb_file_ref_get_parent(PP_Resource file_ref);

int32_t
ppb_file_ref_make_directory(PP_Resource directory_ref, PP_Bool make_ancestors,
                            struct PP_CompletionCallback callback);

int32_t
ppb_file_ref_touch(PP_Resource file_ref, PP_Time last_access_time, PP_Time last_modified_time,
                   struct PP_CompletionCallback callback);

int32_t
ppb_file_ref_delete(PP_Resource file_ref, struct PP_CompletionCallback callback);

int32_t
ppb_file_ref_rename(PP_Resource file_ref, PP_Resource new_file_ref,
                    struct PP_CompletionCallback callback);

int32_t
ppb_file_ref_query(PP_Resource file_ref, struct PP_FileInfo *info,
                   struct PP_CompletionCallback callback);

int32_t
ppb_file_ref_read_directory_entries(PP_Resource file_ref, struct PP_ArrayOutput output,
                                    struct PP_CompletionCallback callback);

#endif // FPP_PPB_FILE_REF_H
