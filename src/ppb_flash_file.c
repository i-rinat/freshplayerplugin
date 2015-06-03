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


#define _FILE_OFFSET_BITS  64

#include "ppb_flash_file.h"
#include <stdlib.h>
#include "pp_resource.h"
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/ppb_file_io.h>
#include <errno.h>
#include <ppapi/c/pp_file_info.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <glib.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "pp_interface.h"


/// resolve pp module local path to absolute one
static
char *
to_abs_path(const char *root, const char *s)
{
    char *rel_path = NULL;
    char *abs_path = NULL;
    char *src, *dst;
    int dot_cnt = 0;
    int after_slash = 1;

    rel_path = g_strdup_printf("/%s", s);
    src = dst = rel_path;

    while (*src) {
        switch (*src) {
        case '\\':
        case '/':
            if (dot_cnt == 1 || dot_cnt == 2) {
                for (int k = 0; k < dot_cnt; k ++) {
                    while (dst > rel_path && *(dst - 1) != '/')
                        dst --;
                    if (dst > rel_path)
                        dst --;
                }
            }
            *dst++ = '/';
            dot_cnt = 0;
            after_slash = 1;
            break;
        case '.':
            *dst++ = '.';
            if (after_slash)
                dot_cnt ++;
            break;
        default:
            dot_cnt = 0;
            after_slash = 0;
            *dst++ = *src;
            break;
        }
        src++;
    }

    // treat trailing "/." and "/.."
    if (after_slash && (dot_cnt == 1 || dot_cnt == 2)) {
        for (int k = 0; k < dot_cnt; k ++) {
            while (dst > rel_path && *(dst - 1) != '/')
                dst --;
            if (dst > rel_path)
                dst --;
        }
        *dst++ = '/';
    }

    int len = dst - rel_path;
    abs_path = g_strdup_printf("%s%.*s", root, len, rel_path);
    g_free(rel_path);

    return abs_path;
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
ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref_id, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!fr->path) {
        trace_error("%s, resource have no path\n", __func__);
        pp_resource_release(file_ref_id);
        return PP_ERROR_FAILED;
    }

    int xmode = pp_mode_to_open_mode(mode);

    *file = open(fr->path, xmode, 0666);
    pp_resource_release(file_ref_id);

    if (*file > 0) {
        return PP_OK;
    } else {
        switch (errno) {
        case ENOENT: return PP_ERROR_FILENOTFOUND;
        case EACCES: return PP_ERROR_NOACCESS;
        default:     return PP_ERROR_FAILED;
        }
    }
}

int32_t
ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    if (!info)
        return PP_ERROR_BADARGUMENT;
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref_id, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct stat sb;
    int ret;
    if (fr->type == PP_FILE_REF_TYPE_NAME) {
        ret = stat(fr->path, &sb);
    } else {
        ret = fstat(fr->fd, &sb);
    }

    if (ret == -1) {
        pp_resource_release(file_ref_id);
        // TODO: create and use common errno converter
        switch (errno) {
        case ENOENT: return PP_ERROR_FILENOTFOUND;
        case EACCES: return PP_ERROR_NOACCESS;
        default:     return PP_ERROR_FAILED;
        }
    }

    info->size = sb.st_size;

    if (S_ISREG(sb.st_mode)) {
        info->type = PP_FILETYPE_REGULAR;
    } else if (S_ISDIR(sb.st_mode)) {
        info->type = PP_FILETYPE_DIRECTORY;
    } else {
        info->type = PP_FILETYPE_OTHER;
    }

    info->system_type = PP_FILESYSTEMTYPE_EXTERNAL; // TODO: which fs type should be used?
    info->creation_time =      sb.st_ctim.tv_sec + sb.st_ctim.tv_nsec / 1e9;
    info->last_access_time =   sb.st_atim.tv_sec + sb.st_atim.tv_nsec / 1e9;
    info->last_modified_time = sb.st_mtim.tv_sec + sb.st_mtim.tv_nsec / 1e9;

    pp_resource_release(file_ref_id);
    return PP_OK;
}

bool
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
void
create_subdirectories_recursively(const char *p)
{
    if (!p)
        return;

    struct stat sb;
    char *path = strdup(p);
    char *last_slash = strrchr(path, '/');

    if (!last_slash)
        goto quit;
    *last_slash = '\0';

    // check if directory already exists
    if (lstat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        goto quit;

    char *ptr = strchr(path, '/');
    while (ptr) {
        *ptr = '\0';
        (void)mkdir(path, 0777);
        *ptr = '/';
        ptr = strchr(ptr + 1, '/');
    }

    (void)mkdir(path, 0777);

quit:
    free(path);
}

int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file)
{
    char *abs_path = to_abs_path(fpp_config_get_pepper_data_dir(), path);
    int xmode = pp_mode_to_open_mode(mode);

    if (xmode & O_CREAT)
        create_subdirectories_recursively(abs_path);
    int fd = open(abs_path, xmode, 0666);
    g_free(abs_path);
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
    (void)instance;
    char *abs_path_from = to_abs_path(fpp_config_get_pepper_data_dir(), path_from);
    char *abs_path_to =   to_abs_path(fpp_config_get_pepper_data_dir(), path_to);

    int ret = rename(abs_path_from, abs_path_to);

    g_free(abs_path_from);
    g_free(abs_path_to);

    if (ret < 0) {
        // TODO: implement error mapping
        return PP_ERROR_FAILED;
    }

    return PP_OK;
}

int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive)
{
    (void)instance;
    if (recursive) {
        trace_warning("%s, recursive not implemented\n", __func__);
    }

    char *abs_path = to_abs_path(fpp_config_get_pepper_data_dir(), path);
    int ret = unlink(abs_path);
    g_free(abs_path);

    if (ret < 0) {
        // TODO: implement error mapping
        return PP_ERROR_FAILED;
    }

    return PP_OK;
}

int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    char *abs_path = to_abs_path(fpp_config_get_pepper_data_dir(), path);
    create_subdirectories_recursively(abs_path);
    int ret = mkdir(abs_path, 0777);
    g_free(abs_path);

    if (ret < 0) {
        switch (errno) {
        case EACCES: return PP_ERROR_NOACCESS;
        case EEXIST: return PP_OK;
        default:     return PP_ERROR_FAILED;
        }
    }

    return PP_OK;
}

int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info)
{
    if (!info) {
        trace_error("%s, 'info' is NULL\n", __func__);
        return PP_ERROR_FAILED;
    }
    char *abs_path = to_abs_path(fpp_config_get_pepper_data_dir(), path);
    struct stat sb;

    int ret = lstat(abs_path, &sb);
    g_free(abs_path);
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
    char *abs_path = to_abs_path(fpp_config_get_pepper_data_dir(), path);
    int n = scandir(abs_path, &namelist, NULL, alphasort);
    *contents = NULL;
    if (n < 0)
        goto err;

    *contents = malloc(sizeof(struct PP_DirContents_Dev));
    if (!*contents)
        goto err;
    (*contents)->entries = malloc(n * sizeof(struct PP_DirEntry_Dev));
    if (!(*contents)->entries)
        goto err2;

    int cnt = 0;
    for (int k = 0; k < n; k ++) {
        char *fname = g_strdup_printf("%s/%s", abs_path, namelist[k]->d_name);
        struct stat sb;
        int ret = lstat(fname, &sb);
        g_free(fname);
        if (ret == 0) {
            (*contents)->entries[cnt].is_dir = S_ISDIR(sb.st_mode);
            (*contents)->entries[cnt].name = strdup(namelist[k]->d_name);
            cnt++;
        }
        free(namelist[k]);
    }
    (*contents)->count = cnt;

    free(namelist);
    g_free(abs_path);
    return PP_OK;
err2:
    free(*contents);
err:
    g_free(abs_path);
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
    tmpfname = g_strdup_printf("/tmp/FreshTempXXXXXX");
    *file = mkstemp(tmpfname);
    unlink(tmpfname);
    g_free(tmpfname);
    if (*file < 0)
        return PP_ERROR_FAILED;
    return PP_OK;
}


// trace wrappers
TRACE_WRAPPER
int32_t
trace_ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s file_ref_id=%d, mode=%d\n", __func__+6, file_ref_id, mode);
    return ppb_flash_file_file_ref_open_file(file_ref_id, mode, file);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    trace_info("[PPB] {full} %s file_ref_id=%d\n", __func__+6, file_ref_id);
    return ppb_flash_file_file_ref_query_file(file_ref_id, info);
}

TRACE_WRAPPER
bool
trace_ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_file_modulelocal_create_thread_adapter_for_instance(instance);
}

TRACE_WRAPPER
void
trace_ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                           PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s, mode=%d\n", __func__+6, instance, path, mode);
    return ppb_flash_file_modulelocal_open_file(instance, path, mode, file);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                             const char *path_to)
{
    trace_info("[PPB] {full} %s instance=%d, path_from=%s, path_to=%s\n",
               __func__+6, instance, path_from, path_to);
    return ppb_flash_file_modulelocal_rename_file(instance, path_from, path_to);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                                    PP_Bool recursive)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s, recursive=%d\n",
               __func__+6, instance, path, recursive);
    return ppb_flash_file_modulelocal_delete_file_or_dir(instance, path, recursive);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_create_dir(instance, path);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                            struct PP_FileInfo *info)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_query_file(instance, path, info);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                                  struct PP_DirContents_Dev **contents)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__+6, instance, path);
    return ppb_flash_file_modulelocal_get_dir_contents(instance, path, contents);
}

TRACE_WRAPPER
void
trace_ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                                   struct PP_DirContents_Dev *contents)
{
    trace_info("[PPB] {full} %s instance=%d, contents=%p\n", __func__+6, instance, contents);
    ppb_flash_file_modulelocal_free_dir_contents(instance, contents);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_file_modulelocal_create_temporary_file(instance, file);
}


const struct PPB_Flash_File_FileRef ppb_flash_file_file_ref_interface_2_0 = {
    .OpenFile =     TWRAPF(ppb_flash_file_file_ref_open_file),
    .QueryFile =    TWRAPF(ppb_flash_file_file_ref_query_file),
};

const struct PPB_Flash_File_ModuleLocal_3_0 ppb_flash_file_modulelocal_interface_3_0 = {
    .CreateThreadAdapterForInstance =
                            TWRAPF(ppb_flash_file_modulelocal_create_thread_adapter_for_instance),
    .ClearThreadAdapterForInstance =
                            TWRAPF(ppb_flash_file_modulelocal_clear_thread_adapter_for_instance),
    .OpenFile =             TWRAPF(ppb_flash_file_modulelocal_open_file),
    .RenameFile =           TWRAPF(ppb_flash_file_modulelocal_rename_file),
    .DeleteFileOrDir =      TWRAPF(ppb_flash_file_modulelocal_delete_file_or_dir),
    .CreateDir =            TWRAPF(ppb_flash_file_modulelocal_create_dir),
    .QueryFile =            TWRAPF(ppb_flash_file_modulelocal_query_file),
    .GetDirContents =       TWRAPF(ppb_flash_file_modulelocal_get_dir_contents),
    .FreeDirContents =      TWRAPF(ppb_flash_file_modulelocal_free_dir_contents),
    .CreateTemporaryFile =  TWRAPF(ppb_flash_file_modulelocal_create_temporary_file),
};

static
void
__attribute__((constructor))
constructor_ppb_flash_file(void)
{
    register_interface(PPB_FLASH_FILE_FILEREF_INTERFACE, &ppb_flash_file_file_ref_interface_2_0);
    register_interface(PPB_FLASH_FILE_MODULELOCAL_INTERFACE_3_0,
                       &ppb_flash_file_modulelocal_interface_3_0);
}
