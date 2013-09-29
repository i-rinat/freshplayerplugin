#include <ppapi/c/ppb_var.h>
#include <stddef.h>


const PPB_Var ppb_var = {
    .AddRef = NULL,
    .Release = NULL,
    .VarFromUtf8 = NULL,
    .VarToUtf8 = NULL
};
