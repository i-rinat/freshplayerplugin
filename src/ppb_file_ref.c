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

#include "ppb_file_ref.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"


PP_Resource
ppb_file_ref_create(PP_Resource file_system, const char *path)
{
    return 0;
}

PP_Bool
ppb_file_ref_is_file_ref(PP_Resource resource)
{
    return PP_TRUE;
}

PP_FileSystemType
ppb_file_ref_get_file_system_type(PP_Resource file_ref)
{
    return 0;
}

struct PP_Var
ppb_file_ref_get_name(PP_Resource file_ref)
{
    return PP_MakeUndefined();
}

struct PP_Var
ppb_file_ref_get_path(PP_Resource file_ref)
{
    return PP_MakeUndefined();
}

PP_Resource
ppb_file_ref_get_parent(PP_Resource file_ref)
{
    return 0;
}

int32_t
ppb_file_ref_make_directory(PP_Resource directory_ref, PP_Bool make_ancestors,
                            struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_ref_touch(PP_Resource file_ref, PP_Time last_access_time, PP_Time last_modified_time,
                   struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_ref_delete(PP_Resource file_ref, struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_ref_rename(PP_Resource file_ref, PP_Resource new_file_ref,
                    struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_ref_query(PP_Resource file_ref, struct PP_FileInfo *info,
                   struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_ref_read_directory_entries(PP_Resource file_ref, struct PP_ArrayOutput output,
                                    struct PP_CompletionCallback callback)
{
    return 0;
}


// trace wrappers
static
PP_Resource
trace_ppb_file_ref_create(PP_Resource file_system, const char *path)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_create(file_system, path);
}

static
PP_Bool
trace_ppb_file_ref_is_file_ref(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_is_file_ref(resource);
}

static
PP_FileSystemType
trace_ppb_file_ref_get_file_system_type(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_get_file_system_type(file_ref);
}

static
struct PP_Var
trace_ppb_file_ref_get_name(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_get_name(file_ref);
}

static
struct PP_Var
trace_ppb_file_ref_get_path(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_get_path(file_ref);
}

static
PP_Resource
trace_ppb_file_ref_get_parent(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_get_parent(file_ref);
}

static
int32_t
trace_ppb_file_ref_make_directory(PP_Resource directory_ref, PP_Bool make_ancestors,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_make_directory(directory_ref, make_ancestors, callback);
}

static
int32_t
trace_ppb_file_ref_touch(PP_Resource file_ref, PP_Time last_access_time, PP_Time last_modified_time,
                         struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_touch(file_ref, last_access_time, last_modified_time, callback);
}

static
int32_t
trace_ppb_file_ref_delete(PP_Resource file_ref, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_delete(file_ref, callback);
}

static
int32_t
trace_ppb_file_ref_rename(PP_Resource file_ref, PP_Resource new_file_ref,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_rename(file_ref, new_file_ref, callback);
}

static
int32_t
trace_ppb_file_ref_query(PP_Resource file_ref, struct PP_FileInfo *info,
                         struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_query(file_ref, info, callback);
}

static
int32_t
trace_ppb_file_ref_read_directory_entries(PP_Resource file_ref, struct PP_ArrayOutput output,
                                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_ref_read_directory_entries(file_ref, output, callback);
}


const struct PPB_FileRef_1_1 ppb_file_ref_interface_1_1 = {
    .Create =               trace_ppb_file_ref_create,
    .IsFileRef =            trace_ppb_file_ref_is_file_ref,
    .GetFileSystemType =    trace_ppb_file_ref_get_file_system_type,
    .GetName =              trace_ppb_file_ref_get_name,
    .GetPath =              trace_ppb_file_ref_get_path,
    .GetParent =            trace_ppb_file_ref_get_parent,
    .MakeDirectory =        trace_ppb_file_ref_make_directory,
    .Touch =                trace_ppb_file_ref_touch,
    .Delete =               trace_ppb_file_ref_delete,
    .Rename =               trace_ppb_file_ref_rename,
    .Query =                trace_ppb_file_ref_query,
    .ReadDirectoryEntries = trace_ppb_file_ref_read_directory_entries,
};

const struct PPB_FileRef_1_0 ppb_file_ref_interface_1_0 = {
    .Create =               trace_ppb_file_ref_create,
    .IsFileRef =            trace_ppb_file_ref_is_file_ref,
    .GetFileSystemType =    trace_ppb_file_ref_get_file_system_type,
    .GetName =              trace_ppb_file_ref_get_name,
    .GetPath =              trace_ppb_file_ref_get_path,
    .GetParent =            trace_ppb_file_ref_get_parent,
    .MakeDirectory =        trace_ppb_file_ref_make_directory,
    .Touch =                trace_ppb_file_ref_touch,
    .Delete =               trace_ppb_file_ref_delete,
    .Rename =               trace_ppb_file_ref_rename,
};
