#include <ppapi/c/trusted/ppb_url_loader_trusted.h>
#include <stddef.h>
#include "trace.h"


void
ppb_url_loader_trusted_grant_universal_access(PP_Resource loader)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__, loader);
}

void
ppb_url_loader_trusted_register_status_callback(PP_Resource loader,
                                                PP_URLLoaderTrusted_StatusCallback cb)
{
    trace_info("[PPB] {zilch} %s loader=%d, cb=%p\n", __func__, loader, cb);
}

const struct PPB_URLLoaderTrusted_0_3 ppb_url_loader_trusted_interface_0_3 = {
    .GrantUniversalAccess =     ppb_url_loader_trusted_grant_universal_access,
    .RegisterStatusCallback =   ppb_url_loader_trusted_register_status_callback,
};
