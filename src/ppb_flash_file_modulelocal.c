#define _GNU_SOURCE
#include <ppapi/c/private/ppb_flash_file.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/pp_file_info.h>
#include <stdlib.h>
#include <stdio.h>
#include <Uri.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include "trace.h"


static const char *pepper_data_dir = "/tmp/freshplayerplugin";


/// resolve pp module local path to absolute one
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

static
PP_Bool
ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__, instance);
    // Deprecated. Returns true.
    return PP_TRUE;
}

static
void
ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__, instance);
    // Deprecated. Does nothing.
    return;
}

static
int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s, mode=%d\n", __func__, instance, path, mode);
    char *abs_path = to_abs_path(path);
    int fd = open(abs_path, O_LARGEFILE, mode);
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

static
int32_t
ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                       const char *path_to)
{
    trace_info("[PPB] {zilch} %s instance=%d, path_from=%s, path_to=%s\n",
               __func__, instance, path_from, path_to);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive)
{
    trace_info("[PPB] {zilch} %s instance=%d, path=%s, recursive=%d\n",
               __func__, instance, path, recursive);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    trace_info("[PPB] {zilch} %s instance=%d, path=%s\n", __func__, instance, path);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info)
{
    trace_info("[PPB] {full} %s instance=%d, path=%s\n", __func__, instance, path);
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

static
int32_t
ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                            struct PP_DirContents_Dev **contents)
{
    trace_info("[PPB] {zilch} %s instance=%d, path=%s\n", __func__, instance, path);
    return 0;
}


static
void
ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                             struct PP_DirContents_Dev *contents)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
}

static
int32_t
ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
    return 0;
}

const struct PPB_Flash_File_ModuleLocal_3_0 ppb_flash_file_modulelocal_interface_3_0 = {
    .CreateThreadAdapterForInstance = ppb_flash_file_modulelocal_create_thread_adapter_for_instance,
    .ClearThreadAdapterForInstance = ppb_flash_file_modulelocal_clear_thread_adapter_for_instance,
    .OpenFile = ppb_flash_file_modulelocal_open_file,
    .RenameFile = ppb_flash_file_modulelocal_rename_file,
    .DeleteFileOrDir = ppb_flash_file_modulelocal_delete_file_or_dir,
    .CreateDir = ppb_flash_file_modulelocal_create_dir,
    .QueryFile = ppb_flash_file_modulelocal_query_file,
    .GetDirContents = ppb_flash_file_modulelocal_get_dir_contents,
    .FreeDirContents = ppb_flash_file_modulelocal_free_dir_contents,
    .CreateTemporaryFile = ppb_flash_file_modulelocal_create_temporary_file
};
