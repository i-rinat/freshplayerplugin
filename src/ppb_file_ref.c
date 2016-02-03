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

#define _FILE_OFFSET_BITS   64
#define _GNU_SOURCE             // for basename()
#include "ppb_file_ref.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "eintr_retry.h"
#include "ppb_var.h"
#include "pp_interface.h"
#include <ppapi/c/pp_errors.h>


PP_Resource
ppb_file_ref_create(PP_Resource file_system, const char *path)
{
    return PP_ERROR_FAILED;
}

PP_Resource
ppb_file_ref_create_unrestricted(const char *path, int read_only)
{
    // TODO: should PP_Instance be required parameter here?
    PP_Resource file_ref = pp_resource_allocate(PP_RESOURCE_FILE_REF, NULL);
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    fr->type = PP_FILE_REF_TYPE_NAME;
    fr->path = nullsafe_strdup(path);

    pp_resource_release(file_ref);
    return file_ref;
}

static
void
ppb_file_ref_destroy(void *p)
{
    struct pp_file_ref_s *fr = p;
    if (fr->type == PP_FILE_REF_TYPE_FD)
        close(fr->fd);
    free(fr->path);
}

PP_Bool
ppb_file_ref_is_file_ref(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_FILE_REF;
}

PP_FileSystemType
ppb_file_ref_get_file_system_type(PP_Resource file_ref)
{
    return PP_ERROR_FAILED;
}

struct PP_Var
ppb_file_ref_get_name(PP_Resource file_ref)
{
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var var = PP_MakeUndefined();
    char *path = strdup(fr->path ? fr->path : "");
    if (!path)
        goto quit;

    var = ppb_var_var_from_utf8_z(basename(path));
    free(path);
quit:
    pp_resource_release(file_ref);
    return var;
}

struct PP_Var
ppb_file_ref_get_path(PP_Resource file_ref)
{
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_MakeUndefined();
    }

    struct PP_Var var = ppb_var_var_from_utf8_z(fr->path);
    pp_resource_release(file_ref);
    return var;
}

PP_Resource
ppb_file_ref_get_parent(PP_Resource file_ref)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_make_directory(PP_Resource directory_ref, PP_Bool make_ancestors,
                            struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_make_directory_1_2(PP_Resource directory_ref, int32_t make_directory_flags,
                                struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_touch(PP_Resource file_ref, PP_Time last_access_time, PP_Time last_modified_time,
                   struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_delete(PP_Resource file_ref, struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_rename(PP_Resource file_ref, PP_Resource new_file_ref,
                    struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_query(PP_Resource file_ref, struct PP_FileInfo *info,
                   struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_file_ref_read_directory_entries(PP_Resource file_ref, struct PP_ArrayOutput output,
                                    struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_file_ref_create(PP_Resource file_system, const char *path)
{
    trace_info("[PPB] {zilch} %s file_system=%d, path=%s\n", __func__+6, file_system, path);
    return ppb_file_ref_create(file_system, path);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_file_ref_is_file_ref(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_file_ref_is_file_ref(resource);
}

TRACE_WRAPPER
PP_FileSystemType
trace_ppb_file_ref_get_file_system_type(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s file_ref=%d\n", __func__+6, file_ref);
    return ppb_file_ref_get_file_system_type(file_ref);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_file_ref_get_name(PP_Resource file_ref)
{
    trace_info("[PPB] {full} %s file_ref=%d\n", __func__+6, file_ref);
    return ppb_file_ref_get_name(file_ref);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_file_ref_get_path(PP_Resource file_ref)
{
    trace_info("[PPB] {full} %s file_ref=%d\n", __func__+6, file_ref);
    return ppb_file_ref_get_path(file_ref);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_file_ref_get_parent(PP_Resource file_ref)
{
    trace_info("[PPB] {zilch} %s file_ref=%d\n", __func__+6, file_ref);
    return ppb_file_ref_get_parent(file_ref);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_make_directory(PP_Resource directory_ref, PP_Bool make_ancestors,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s directory_ref=%d, make_ancestors=%u, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, directory_ref, make_ancestors,
               callback.func, callback.user_data, callback.flags);
    return ppb_file_ref_make_directory(directory_ref, make_ancestors, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_make_directory_1_2(PP_Resource directory_ref, int32_t make_directory_flags,
                                      struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s directory_ref=%d, make_directory_flags=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%u}\n", __func__+6, directory_ref, make_directory_flags,
               callback.func, callback.user_data, callback.flags);
    return ppb_file_ref_make_directory_1_2(directory_ref, make_directory_flags, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_touch(PP_Resource file_ref, PP_Time last_access_time, PP_Time last_modified_time,
                         struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s file_ref=%d, last_access_time=%f, last_modified_time=%f, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, file_ref,
               last_access_time, last_modified_time, callback.func, callback.user_data,
               callback.flags);
    return ppb_file_ref_touch(file_ref, last_access_time, last_modified_time, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_delete(PP_Resource file_ref, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s file_ref=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, file_ref, callback.func, callback.user_data, callback.flags);
    return ppb_file_ref_delete(file_ref, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_rename(PP_Resource file_ref, PP_Resource new_file_ref,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s file_ref=%d, new_file_ref=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, file_ref, new_file_ref, callback.func,
               callback.user_data, callback.flags);
    return ppb_file_ref_rename(file_ref, new_file_ref, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_query(PP_Resource file_ref, struct PP_FileInfo *info,
                         struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s file_ref=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, file_ref, callback.func, callback.user_data, callback.flags);
    return ppb_file_ref_query(file_ref, info, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_ref_read_directory_entries(PP_Resource file_ref, struct PP_ArrayOutput output,
                                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s file_ref=%d, output={.GetDataBuffer=%p, .user_data=%p}, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, file_ref,
               output.GetDataBuffer, output.user_data, callback.func, callback.user_data,
               callback.flags);
    return ppb_file_ref_read_directory_entries(file_ref, output, callback);
}


const struct PPB_FileRef_1_1 ppb_file_ref_interface_1_1 = {
    .Create =               TWRAPZ(ppb_file_ref_create),
    .IsFileRef =            TWRAPF(ppb_file_ref_is_file_ref),
    .GetFileSystemType =    TWRAPZ(ppb_file_ref_get_file_system_type),
    .GetName =              TWRAPF(ppb_file_ref_get_name),
    .GetPath =              TWRAPF(ppb_file_ref_get_path),
    .GetParent =            TWRAPZ(ppb_file_ref_get_parent),
    .MakeDirectory =        TWRAPZ(ppb_file_ref_make_directory),
    .Touch =                TWRAPZ(ppb_file_ref_touch),
    .Delete =               TWRAPZ(ppb_file_ref_delete),
    .Rename =               TWRAPZ(ppb_file_ref_rename),
    .Query =                TWRAPZ(ppb_file_ref_query),
    .ReadDirectoryEntries = TWRAPZ(ppb_file_ref_read_directory_entries),
};

const struct PPB_FileRef_1_0 ppb_file_ref_interface_1_0 = {
    .Create =               TWRAPZ(ppb_file_ref_create),
    .IsFileRef =            TWRAPF(ppb_file_ref_is_file_ref),
    .GetFileSystemType =    TWRAPZ(ppb_file_ref_get_file_system_type),
    .GetName =              TWRAPF(ppb_file_ref_get_name),
    .GetPath =              TWRAPF(ppb_file_ref_get_path),
    .GetParent =            TWRAPZ(ppb_file_ref_get_parent),
    .MakeDirectory =        TWRAPZ(ppb_file_ref_make_directory),
    .Touch =                TWRAPZ(ppb_file_ref_touch),
    .Delete =               TWRAPZ(ppb_file_ref_delete),
    .Rename =               TWRAPZ(ppb_file_ref_rename),
};

const struct PPB_FileRef_1_2 ppb_file_ref_interface_1_2 = {
    .Create =               TWRAPZ(ppb_file_ref_create),
    .IsFileRef =            TWRAPF(ppb_file_ref_is_file_ref),
    .GetFileSystemType =    TWRAPZ(ppb_file_ref_get_file_system_type),
    .GetName =              TWRAPF(ppb_file_ref_get_name),
    .GetPath =              TWRAPF(ppb_file_ref_get_path),
    .GetParent =            TWRAPZ(ppb_file_ref_get_parent),
    .MakeDirectory =        TWRAPZ(ppb_file_ref_make_directory_1_2),
    .Touch =                TWRAPZ(ppb_file_ref_touch),
    .Delete =               TWRAPZ(ppb_file_ref_delete),
    .Rename =               TWRAPZ(ppb_file_ref_rename),
    .Query =                TWRAPZ(ppb_file_ref_query),
    .ReadDirectoryEntries = TWRAPZ(ppb_file_ref_read_directory_entries),
};

static
void
__attribute__((constructor))
constructor_ppb_file_ref(void)
{
    register_interface(PPB_FILEREF_INTERFACE_1_0, &ppb_file_ref_interface_1_0);
    register_interface(PPB_FILEREF_INTERFACE_1_1, &ppb_file_ref_interface_1_1);
    register_interface(PPB_FILEREF_INTERFACE_1_2, &ppb_file_ref_interface_1_2);
    register_resource(PP_RESOURCE_FILE_REF, ppb_file_ref_destroy);
}
