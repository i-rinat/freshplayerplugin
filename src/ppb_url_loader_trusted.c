#include <ppapi/c/trusted/ppb_url_loader_trusted.h>
#include <stddef.h>


const PPB_URLLoaderTrusted ppb_url_loader_trusted = {
    .GrantUniversalAccess = NULL,
    .RegisterStatusCallback = NULL
};
