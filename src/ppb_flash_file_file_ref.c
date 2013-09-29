#include <ppapi/c/private/ppb_flash_file.h>
#include <stddef.h>


const struct PPB_Flash_File_FileRef ppb_flash_file_file_ref = {
    .OpenFile = NULL,
    .QueryFile = NULL
};
