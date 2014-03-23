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

#define _GNU_SOURCE
#include "ppb_flash_file_modulelocal.h"
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/pp_file_info.h>
#include <ppapi/c/ppb_file_io.h>
#include <stdlib.h>
#include <stdio.h>
#include <Uri.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include "trace.h"


static const char *pepper_data_dir = "/tmp/freshplayerplugin";


/// resolve pp module local path to absolute one
static
char *
to_abs_path(const char *s)
{
    char *rel_path = NULL;
    UriParserStateA state;
    UriUriA uri;
    int len;

    asprintf(&rel_path, "/%s", s);
    state.uri = &uri;
    if (uriParseUriA(&state, rel_path) != URI_SUCCESS)
        goto err;
    if (uriNormalizeSyntaxExA(&uri, URI_NORMALIZE_PATH) != URI_SUCCESS)
        goto err;

    if (uriToStringCharsRequiredA(&uri, &len) != URI_SUCCESS)
        goto err;

    char *rel_path_norm = malloc(len + 1);
    if (!rel_path_norm)
        goto err;
    if (uriToStringA(rel_path_norm, &uri, len + 1, NULL) != URI_SUCCESS)
        goto err2;

    char *next_slash = strchr(rel_path_norm+1, '/');
    if (next_slash) {
        char *module_path;
        struct stat sb;
        const int module_name_len = next_slash - rel_path_norm;
        asprintf(&module_path, "%s%.*s", pepper_data_dir, module_name_len, rel_path_norm);
        int ret = lstat(module_path, &sb);
        if (ret < 0 && ENOENT == errno) {
            // recursively create directories
            char *ptr = module_path + 1;
            while (*ptr != 0) {
                if ('/' == *ptr) {
                    *ptr = '\0';
                    mkdir(module_path, 0777);
                    *ptr = '/';
                }
                ptr ++;
            }
            mkdir(module_path, 0777);
        } else if (ret < 0) {
            goto err2;
        }
        free(module_path);
    }

    char *result;
    asprintf(&result, "%s%s", pepper_data_dir, rel_path_norm);
    free(rel_path_norm);
    free(rel_path);
    uriFreeUriMembersA(&uri);
    return result;

err2:
    free(rel_path_norm);
err:
    uriFreeUriMembersA(&uri);
    free(rel_path);
    return NULL;
}

PP_Bool
ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance)
{
    // Deprecated. Returns true.
    return PP_TRUE;
}

void
ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance)
{
    // Deprecated. Does nothing.
    return;
}

static
int
pp_mode_to_open_mode(int32_t mode)
{
    int ret = 0;
    if ((mode & PP_FILEOPENFLAG_READ) && !(mode & PP_FILEOPENFLAG_WRITE))
        ret = O_RDONLY;
    if (!(mode & PP_FILEOPENFLAG_READ) && (mode & PP_FILEOPENFLAG_WRITE))
        ret = O_WRONLY;
    if ((mode & PP_FILEOPENFLAG_READ) && (mode & PP_FILEOPENFLAG_WRITE))
        ret = O_RDWR;
    if (mode & PP_FILEOPENFLAG_CREATE)
        ret |= O_CREAT;
    if (mode & PP_FILEOPENFLAG_TRUNCATE)
        ret |= O_TRUNC;
    if (mode & PP_FILEOPENFLAG_EXCLUSIVE)
        ret |= O_EXCL;
    if (mode & PP_FILEOPENFLAG_APPEND)
        ret |= O_APPEND;
    return ret;
}

int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file)
{
    char *abs_path = to_abs_path(path);
    int fd = open(abs_path, O_LARGEFILE, pp_mode_to_open_mode(mode));
    free(abs_path);
    *file = fd;
    if (fd > 0)
        return PP_OK;
    else
        switch (errno) {
            case ENOENT: return PP_ERROR_FILENOTFOUND;
            case EACCES: return PP_ERROR_NOACCESS;
            default:     return PP_ERROR_FAILED;
        }
}

int32_t
ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                       const char *path_to)
{
    return 0;
}

int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive)
{
    return 0;
}

int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    char *abs_path = to_abs_path(path);
    int ret = mkdir(abs_path, 0777);
    free(abs_path);
    if (ret < 0) {
        switch (errno) {
        case EACCES: return PP_ERROR_NOACCESS;
        case EEXIST: return PP_ERROR_FILEEXISTS;
        default:     return PP_ERROR_FAILED;
        }
    }

    return PP_OK;
}

int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info)
{
    if (!info)
        return PP_ERROR_FAILED;
    char *abs_path = to_abs_path(path);
    struct stat sb;

    int ret = lstat(abs_path, &sb);
    free(abs_path);
    if (ret < 0) {
        switch (errno) {
            case ENOENT: return PP_ERROR_FILENOTFOUND;
            case EACCES: return PP_ERROR_NOACCESS;
            default: return PP_ERROR_FAILED;
        }
    }

    info->size = sb.st_size;
    if (S_ISREG(sb.st_mode))
        info->type = PP_FILETYPE_REGULAR;
    else if (S_ISDIR(sb.st_mode))
        info->type = PP_FILETYPE_DIRECTORY;
    else
        info->type = PP_FILETYPE_OTHER;
    info->system_type = PP_FILESYSTEMTYPE_ISOLATED;
    info->creation_time = sb.st_ctime;
    info->last_access_time = sb.st_atime;
    info->last_modified_time = sb.st_mtime;

    return PP_OK;
}

int32_t
ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                            struct PP_DirContents_Dev **contents)
{
    struct dirent **namelist;
    char *abs_path = to_abs_path(path);
    int n = scandir(abs_path, &namelist, NULL, alphasort);
    *contents = NULL;
    if (n < 0)
        goto err;

    *contents = malloc(sizeof(struct PP_DirContents_Dev));
    if (!*contents)
        goto err;
    (*contents)->count = n;
    (*contents)->entries = malloc(n * sizeof(struct PP_DirEntry_Dev));
    if (!(*contents)->entries)
        goto err2;

    for (int k = 0; k < n; k ++) {
        char *fname;
        struct stat sb;
        (*contents)->entries[k].name = strdup(namelist[k]->d_name);
        asprintf(&fname, "%s/%s", abs_path, namelist[k]->d_name);
        lstat(fname, &sb);
        free(fname);
        (*contents)->entries[k].is_dir = S_ISDIR(sb.st_mode);
        free(namelist[k]);
    }

    free(namelist);
    free(abs_path);
    return PP_OK;
err2:
    free(*contents);
err:
    free(abs_path);
    return PP_ERROR_FAILED;
}

void
ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                             struct PP_DirContents_Dev *contents)
{
    for (int k = 0; k < contents->count; k ++)
        free((char*)contents->entries[k].name);
    free(contents->entries);
    free(contents);
}

int32_t
ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file)
{
    (void)instance;
    char *tmpfname;
    // TODO: find a good directory for temporary files
    asprintf(&tmpfname, "/tmp/FreshXXXXXX");
    *file = mkstemp(tmpfname);
    free(tmpfname);
    if (*file < 0)
        return PP_ERROR_FAILED;
    return PP_OK;
}

// trace wrappers
static
PP_Bool
trace_ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_file_modulelocal_create_thread_adapter_for_instance(instance);
}

static
void
trace_ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(instance);
}

static
int32_t
trace_ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                           PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s, mode=%d\n", __func__+6, instance, path, mode);
    return ppb_flash_file_modulelocal_open_file(instance, path, mode, file);
}

static
int32_t
trace_ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                             const char *path_to)
{
    trace_info("[PPB] {zilch} %s instance=%d, path_from=%s, path_to=%s\n",
               __func__+6, instance, path_from, path_to);
    return ppb_flash_file_modulelocal_rename_file(instance, path_from, path_to);
}

static
int32_t
trace_ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                                    PP_Bool recursive)
{
    trace_info("[PPB] {zilch} %s instance=%d, path=%s, recursive=%d\n",
               __func__+6, instance, path, recursive);
    return ppb_flash_file_modulelocal_delete_file_or_dir(instance, path, recursive);
}

static
int32_t
trace_ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_create_dir(instance, path);
}

static
int32_t
trace_ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                            struct PP_FileInfo *info)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_query_file(instance, path, info);
}

static
int32_t
trace_ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                                  struct PP_DirContents_Dev **contents)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_get_dir_contents(instance, path, contents);
}

static
void
trace_ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                                   struct PP_DirContents_Dev *contents)
{
    trace_info("[PPB] {full} %s instance=%d, contents=%p\n", __func__+6, instance, contents);
    ppb_flash_file_modulelocal_free_dir_contents(instance, contents);
}

static
int32_t
trace_ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_file_modulelocal_create_temporary_file(instance, file);
}


const struct PPB_Flash_File_ModuleLocal_3_0 ppb_flash_file_modulelocal_interface_3_0 = {
    .CreateThreadAdapterForInstance =
                                trace_ppb_flash_file_modulelocal_create_thread_adapter_for_instance,
    .ClearThreadAdapterForInstance =
                                trace_ppb_flash_file_modulelocal_clear_thread_adapter_for_instance,
    .OpenFile =                 trace_ppb_flash_file_modulelocal_open_file,
    .RenameFile =               trace_ppb_flash_file_modulelocal_rename_file,
    .DeleteFileOrDir =          trace_ppb_flash_file_modulelocal_delete_file_or_dir,
    .CreateDir =                trace_ppb_flash_file_modulelocal_create_dir,
    .QueryFile =                trace_ppb_flash_file_modulelocal_query_file,
    .GetDirContents =           trace_ppb_flash_file_modulelocal_get_dir_contents,
    .FreeDirContents =          trace_ppb_flash_file_modulelocal_free_dir_contents,
    .CreateTemporaryFile =      trace_ppb_flash_file_modulelocal_create_temporary_file
};
