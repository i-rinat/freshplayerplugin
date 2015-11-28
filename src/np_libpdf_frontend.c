/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include <npapi/npfunctions.h>
#include <stdio.h>
#include <glib.h>
#include <string.h>
#include "trace_core.h"
#include "config.h"
#include "txt_resources.h"


#define NPString_literal(str) { .UTF8Characters = str, .UTF8Length = strlen(str) }

NPNetscapeFuncs     npn;


NPError
NPP_SetWindow(NPP npp, NPWindow *window)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_New(NPMIMEType pluginType, NPP npp, uint16_t mode, int16_t argc, char *argn[],
        char *argv[], NPSavedData *saved)
{
    trace_info_f("%s\n", __func__);

    NPObject *np_plugin_element_obj;

    int err = npn.getvalue(npp, NPNVPluginElementNPObject, &np_plugin_element_obj);
    if (err != NPERR_NO_ERROR) {
        trace_warning("%s, NPN_GetValue returned %d\n", __func__, err);
        goto done;
    }

    NPVariant result;
    NPString script = {
        .UTF8Characters = resource_text_libpdf_frontend_js,
        .UTF8Length = strlen(resource_text_libpdf_frontend_js),
    };

    if (!npn.evaluate(npp, np_plugin_element_obj, &script, &result)) {
        trace_warning("%s, NPN_Evaluate failed\n", __func__);
    }

done:
    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP npp, NPSavedData **save)
{
    trace_info_f("%s\n", __func__);

    if (save)
        *save = NULL;
    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP npp, NPMIMEType type, NPStream *stream, NPBool seekable, uint16_t *stype)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP npp, NPStream *stream, NPReason reason)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP npp, NPStream *stream)
{
    trace_info_f("%s\n", __func__);
    return 1024*1024;
}

int32_t
NPP_Write(NPP npp, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{
    trace_info_f("%s\n", __func__);
    return len;
}

void
NPP_StreamAsFile(NPP npp, NPStream *stream, const char *fname)
{
    trace_info_f("%s\n", __func__);
}

void
NPP_Print(NPP npp, NPPrint *platformPrint)
{
    trace_info_f("%s\n", __func__);
}

int16_t
NPP_HandleEvent(NPP npp, void *event)
{
    trace_info_f("%s\n", __func__);
    return 1;
}

void
NPP_URLNotify(NPP npp, const char *url, NPReason reason, void *notifyData)
{
    trace_info_f("%s\n", __func__);
}

NPError
NPP_GetValue(NPP npp, NPPVariable variable, void *value)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP npp, NPNVariable variable, void *value)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP npp, NPFocusDirection direction)
{
    trace_info_f("%s\n", __func__);
    return 1;
}

void
NPP_LostFocus(NPP npp)
{
    trace_info_f("%s\n", __func__);
}

void
NPP_URLRedirectNotify(NPP npp, const char *url, int32_t status, void *notifyData)
{
    trace_info_f("%s\n", __func__);
}

NPError
NPP_ClearSiteData(const char *site, uint64_t flags, uint64_t maxAge)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}

char**
NPP_GetSitesWithData(void)
{
    trace_info_f("%s\n", __func__);
    return NULL;
}

void
NPP_DidComposite(NPP npp)
{
    trace_info_f("%s\n", __func__);
    return;
}

const char *
NP_GetMIMEDescription(void)
{
    trace_info_f("%s\n", __func__);
    return fpp_config_get_plugin_mime_type();
}

char *
NP_GetPluginVersion(void)
{
    trace_info_f("%s\n", __func__);
    // hope browser won't write to the value
    return (char *)fpp_config_get_default_plugin_version();
}

NPError
NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    trace_info_f("%s\n", __func__);

    switch (variable) {
    case NPPVpluginNameString:
        *(const char **)value = fpp_config_get_plugin_name();
        break;
    case NPPVpluginDescriptionString:
        *(const char **)value = fpp_config_get_default_plugin_descr();
        break;
    default:
        trace_info_z("    not implemented variable %d\n", variable);
    }

    return NPERR_NO_ERROR;
}

NPError
NP_Initialize(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs)
{
    trace_info_f("%s\n", __func__);

    memset(&npn, 0, sizeof(npn));
    memcpy(&npn, aNPNFuncs, MIN(sizeof(npn), aNPNFuncs->size));

    NPPluginFuncs pf;
    memset(&pf, 0, sizeof(NPPluginFuncs));
    pf.size = MIN(aNPPFuncs->size, sizeof(NPPluginFuncs));

    pf.newp =               NPP_New;
    pf.destroy =            NPP_Destroy;
    pf.setwindow =          NPP_SetWindow;
    pf.newstream =          NPP_NewStream;
    pf.destroystream =      NPP_DestroyStream;
    pf.asfile =             NPP_StreamAsFile;
    pf.writeready =         NPP_WriteReady;
    pf.write =              NPP_Write;
    pf.print =              NPP_Print;
    pf.event =              NPP_HandleEvent;
    pf.urlnotify =          NPP_URLNotify;
    pf.getvalue =           NPP_GetValue;
    pf.setvalue =           NPP_SetValue;
    pf.gotfocus =           NPP_GotFocus;
    pf.lostfocus =          NPP_LostFocus;
    pf.urlredirectnotify =  NPP_URLRedirectNotify;
    pf.clearsitedata =      NPP_ClearSiteData;
    pf.getsiteswithdata =   NPP_GetSitesWithData;
    pf.didComposite =       NPP_DidComposite;

    memcpy(aNPPFuncs, &pf, pf.size);

    return NPERR_NO_ERROR;
}

NPError
NP_Shutdown(void)
{
    trace_info_f("%s\n", __func__);
    return NPERR_NO_ERROR;
}
