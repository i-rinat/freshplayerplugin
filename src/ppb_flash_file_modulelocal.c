#include <ppapi/c/private/ppb_flash_file.h>
#include <stddef.h>


const struct PPB_Flash_File_ModuleLocal_3_0 ppb_flash_file_modulelocal_interface_3_0 = {
    .CreateThreadAdapterForInstance = NULL,
    .ClearThreadAdapterForInstance = NULL,
    .OpenFile = NULL,
    .RenameFile = NULL,
    .DeleteFileOrDir = NULL,
    .CreateDir = NULL,
    .QueryFile = NULL,
    .GetDirContents = NULL,
    .FreeDirContents = NULL,
    .CreateTemporaryFile = NULL
};
