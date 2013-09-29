#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <stddef.h>


const PPB_URLUtil_Dev ppb_url_util_dev = {
    .Canonicalize = NULL,
    .ResolveRelativeToURL = NULL,
    .ResolveRelativeToDocument = NULL,
    .IsSameSecurityOrigin = NULL,
    .DocumentCanRequest = NULL,
    .DocumentCanAccessDocument = NULL,
    .GetDocumentURL = NULL,
    .GetPluginInstanceURL = NULL
};
