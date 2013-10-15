#include <npapi.h>
#include <npfunctions.h>
#include "trace.h"


__attribute__((visibility("default")))
const char *
NP_GetMIMEDescription(void)
{
    // TODO: get MIME info from manifest or plugin itself
    return "application/x-shockwave-flash:swf:Shockwave Flash";
}

__attribute__((visibility("default")))
const char *
NP_GetPluginVersion(void)
{
    // TODO: get version from manifest
    return "11,9,123,456";
}

__attribute__((visibility("default")))
NPError
NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    trace_info("[NP] %s stub\n", __func__);
    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
NPError
NP_Initialize(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs)
{
    // TODO: implement
    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
void
NP_Shutdown(void)
{
    // TODO: implement
    return;
}
