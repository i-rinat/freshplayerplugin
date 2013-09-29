#include <ppapi/c/private/ppb_flash_file.h>
#include <stddef.h>


const PPB_Flash_File_ModuleLocal ppb_flash_file_modulelocal = {
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
