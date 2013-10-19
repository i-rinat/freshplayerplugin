#include <npapi.h>
#include <npfunctions.h>
#include <string.h>
#include "globals.h"
#include "trace.h"
#include "reverse_constant.h"

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
    trace_info("[NP] %s instance=%p, variable=%s, value=%p\n", __func__, instance,
               reverse_npp_variable(variable), value);
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

    memcpy(&npn, aNPNFuncs, sizeof(npn) < aNPNFuncs->size ? sizeof(npn) : aNPNFuncs->size);

    aNPPFuncs->size = sizeof(NPPluginFuncs);
    aNPPFuncs->version = NP_VERSION_MAJOR + NP_VERSION_MINOR;
    aNPPFuncs->newp = NPP_New;
    aNPPFuncs->destroy = NPP_Destroy;
    aNPPFuncs->setwindow = NPP_SetWindow;
    aNPPFuncs->newstream = NPP_NewStream;
    aNPPFuncs->destroystream = NPP_DestroyStream;
    aNPPFuncs->asfile = NPP_StreamAsFile;
    aNPPFuncs->writeready = NPP_WriteReady;
    aNPPFuncs->write = NPP_Write;
    aNPPFuncs->print = NPP_Print;
    aNPPFuncs->event = NPP_HandleEvent;
    aNPPFuncs->urlnotify = NPP_URLNotify;
    aNPPFuncs->getvalue = NPP_GetValue;
    aNPPFuncs->setvalue = NPP_SetValue;
    aNPPFuncs->gotfocus = NPP_GotFocus;
    aNPPFuncs->lostfocus = NPP_LostFocus;
    aNPPFuncs->urlredirectnotify = NPP_URLRedirectNotify;
    aNPPFuncs->clearsitedata = NPP_ClearSiteData;
    aNPPFuncs->getsiteswithdata = NPP_GetSitesWithData;
    aNPPFuncs->didComposite = NPP_DidComposite;

    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
void
NP_Shutdown(void)
{
    trace_info("[NP] %s\n", __func__);
    return;
}
