#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <stddef.h>


const PPB_FileChooser_Dev ppb_file_chooser_dev = {
    .Create = NULL,
    .IsFileChooser = NULL,
    .Show = NULL
};
