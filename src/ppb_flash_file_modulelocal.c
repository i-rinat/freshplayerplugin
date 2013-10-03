#include <ppapi/c/private/ppb_flash_file.h>
#include <stddef.h>
#include "trace.h"

static
PP_Bool
ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("{full} %s\n", __func__);
    return PP_TRUE;
}

static
void
ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance)
{
    trace_info("{full} %s\n", __func__);
}

static
int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file)
{
    trace_info("{zilch} %s\n        path = %s, mode = %d\n", __func__, path, mode);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                       const char *path_to)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path)
{
    trace_info("{zilch} %s\n        instance = %d, path = %s\n", __func__, instance, path);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info)
{
    trace_info("{zilch} %s\n        instance = %d, path = %s\n", __func__, instance, path);
    return 0;
}

static
int32_t
ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                            struct PP_DirContents_Dev **contents)
{
    trace_info("{zilch} %s\n        instance = %d, path = %s\n", __func__, instance, path);
    return 0;
}


static
void
ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                             struct PP_DirContents_Dev *contents)
{
    trace_info("{zilch} %s\n", __func__);
}

static
int32_t
ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file)
{
    trace_info("{zilch} %s\n", __func__);
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
