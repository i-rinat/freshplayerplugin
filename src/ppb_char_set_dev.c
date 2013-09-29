#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <stddef.h>


const PPB_CharSet_Dev ppb_char_set_dev = {
    .UTF16ToCharSet = NULL,
    .CharSetToUTF16 = NULL,
    .GetDefaultCharSet = NULL
};
