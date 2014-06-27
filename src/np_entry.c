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
#include <parson/parson.h>
#include <libgen.h>
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "reverse_constant.h"
#include "pp_interface.h"


static void *module_dl_handler;
static gchar *module_version;
static gchar *module_descr;

static
void
use_fallback_version_strings(void)
{
    module_version = g_strdup_printf("13.1.2.3");
    module_descr = g_strdup_printf("Shockwave Flash 13.1 r2");
}

static
uintptr_t
load_ppp_module(void)
{
    int32_t (*ppp_initialize_module)(PP_Module module_id, PPB_GetInterface get_browser_interface);

    if (module_dl_handler) {
        // already loaded
        return 0;
    }

    fpp_config_initialize();

    module_dl_handler = dlopen(config.plugin_path, RTLD_LAZY);
    if (!module_dl_handler) {
        trace_error("%s, can't open %s\n", __func__, config.plugin_path);
        config.quirks.plugin_missing = 1;
        return 1;
    }

    ppp_initialize_module = dlsym(module_dl_handler, "PPP_InitializeModule");
    ppp_get_interface = dlsym(module_dl_handler, "PPP_GetInterface");

    if (!ppp_initialize_module || !ppp_get_interface) {
        trace_error("%s, one of required PPP_* is missing\n", __func__);
        dlclose(module_dl_handler);
        module_dl_handler = NULL;
        return 1;
    }

    // TODO: make module ids distinct
    int res = ppp_initialize_module(42, ppb_get_interface);
    if (0 != res) {
        trace_error("%s, PPP_InitializeModule returned %d\n", __func__, res);
        dlclose(module_dl_handler);
        module_dl_handler = NULL;
        return 1;
    }

    // try to read manifest.json file
    char *manifest_dir = strdup(config.plugin_path);
    gchar *manifest_path = g_strdup_printf("%s/manifest.json", dirname(manifest_dir));
    free(manifest_dir);

    JSON_Value *root_val = json_parse_file(manifest_path);
    g_free(manifest_path);
    if (!root_val) {
        use_fallback_version_strings();
        return 0;
    }

    JSON_Object *root_obj = json_value_get_object(root_val);
    const char *version = json_object_get_string(root_obj, "version");
    if (version) {
        int v1 = 0, v2 = 0, v3 = 0, v4 = 0;
        module_version = g_strdup(version);
        (void)sscanf(module_version, "%d.%d.%d.%d", &v1, &v2, &v3, &v4);
        module_descr = g_strdup_printf("Shockwave Flash %d.%d r%d", v1, v2, v3);
    } else {
        use_fallback_version_strings();
    }

    json_value_free(root_val);
    return 0;
}

static
void
unload_ppp_module(void)
{
    void (*ppp_shutdown_module)(void);

    if (!module_dl_handler) {
        // module not loaded
        return;
    }

    g_free(module_descr); module_descr = NULL;
    g_free(module_version); module_version = NULL;

    // call module shutdown handler if exists
    ppp_shutdown_module = dlsym(module_dl_handler, "PPP_ShutdownModule");
    if (ppp_shutdown_module)
        ppp_shutdown_module();

    dlclose(module_dl_handler);
    module_dl_handler = NULL;

    fpp_config_destroy();
}


__attribute__((visibility("default")))
const char *
NP_GetMIMEDescription(void)
{
    trace_info_f("[NP] %s\n", __func__);
    return "application/x-shockwave-flash:swf:Shockwave Flash";
}

__attribute__((visibility("default")))
char *
NP_GetPluginVersion(void)
{
    trace_info_f("[NP] %s\n", __func__);
    load_ppp_module();
    return module_version;
}

__attribute__((visibility("default")))
NPError
NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    trace_info_f("[NP] %s instance=%p, variable=%s, value=%p\n", __func__, instance,
                 reverse_npp_variable(variable), value);
    load_ppp_module();
    switch (variable) {
    case NPPVpluginNameString:
        *(const char **)value = "Shockwave Flash";
        break;
    case NPPVpluginDescriptionString:
        *(char **)value = module_descr;
        break;
    default:
        trace_info_z("    not implemented variable %d\n", variable);
    }
    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
NPError
NP_Initialize(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs)
{
    trace_info_f("[NP] %s aNPNFuncs=%p, aNPPFuncs=%p, browser API version = %u\n", __func__,
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

    load_ppp_module();

    return NPERR_NO_ERROR;
}

__attribute__((visibility("default")))
NPError
NP_Shutdown(void)
{
    trace_info_f("[NP] %s\n", __func__);

    unload_ppp_module();

    return NPERR_NO_ERROR;
}
