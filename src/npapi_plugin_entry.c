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
    trace_info("[NP] %s instance=%p, variable=%d, value=%p\n", __func__, instance, variable,
               value);
    switch (variable) {
    case NPPVpluginNameString:
        *(const char **)value = "FreshPrayerPlugin";
        break;
    case NPPVpluginDescriptionString:
        *(const char **)value = "Wrapper around PepperFlash for NPAPI enabled browsers";
        break;
    default:
        trace_info("    not implemented variable %d\n", variable);
    }
    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
NPError
NP_Initialize(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs)
{
    trace_info("[NP] %s aNPNFuncs=%p, aNPPFuncs=%p\n", __func__, aNPNFuncs, aNPPFuncs);
    // TODO: implement
    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
void
NP_Shutdown(void)
{
    trace_info("[NP] %s\n", __func__);
    return;
}
