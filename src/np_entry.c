/*
 * Copyright © 2013-2014  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <dlfcn.h>
#include <npapi/npapi.h>
#include <npapi/npfunctions.h>
#include <string.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/pp_module.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "reverse_constant.h"
#include "pp_interface.h"


__attribute__((visibility("default")))
const char *
NP_GetMIMEDescription(void)
{
    trace_info("[NP] %s\n", __func__);
    // TODO: get MIME info from manifest or plugin itself
    return "application/x-shockwave-flash:swf:Shockwave Flash";
}

__attribute__((visibility("default")))
char *
NP_GetPluginVersion(void)
{
    trace_info("[NP] %s\n", __func__);
    // TODO: get version from manifest
    return (char*)"14.0.0.125";
}

__attribute__((visibility("default")))
NPError
NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    trace_info("[NP] %s instance=%p, variable=%s, value=%p\n", __func__, instance,
               reverse_npp_variable(variable), value);
    switch (variable) {
    case NPPVpluginNameString:
        *(const char **)value = "Shockwave Flash";
        break;
    case NPPVpluginDescriptionString:
        *(const char **)value = "Shockwave Flash 14.0 r0";
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
    int32_t (*ppp_initialize_module)(PP_Module module_id, PPB_GetInterface get_browser_interface);

    trace_info("[NP] %s aNPNFuncs=%p, aNPPFuncs=%p, browser API version = %u\n", __func__,
               aNPNFuncs, aNPPFuncs, aNPNFuncs->version);

    memset(&npn, 0, sizeof(npn));
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

    fpp_config_initialize();

    void *h = dlopen(config.plugin_path, RTLD_LAZY);
    if (!h) {
        trace_error("%s, can't open %s\n", __func__, config.plugin_path);
        return NPERR_GENERIC_ERROR;
    }

    ppp_initialize_module = dlsym(h, "PPP_InitializeModule");
    ppp_get_interface = dlsym(h, "PPP_GetInterface");

    if (!ppp_initialize_module || !ppp_get_interface) {
        trace_error("%s, one of required PPP_* is missing\n", __func__);
        return NPERR_GENERIC_ERROR;
    }

    // TODO: make module ids distinct
    int res = ppp_initialize_module(42, ppb_get_interface);
    if (0 != res) {
        trace_error("%s, PPP_InitializeModule returned %d\n", __func__, res);
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
NPError
NP_Shutdown(void)
{
    trace_info("[NP] %s\n", __func__);
    fpp_config_destroy();
    return NPERR_NO_ERROR;
}
