#include <npapi.h>
#include <stdlib.h>
#include "trace.h"




NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[],
        char* argv[], NPSavedData* saved)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}


NPError
NPP_Destroy(NPP instance, NPSavedData** save)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow* window)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream* stream)
{
    trace_info("[NPP] %s\n", __func__);
    return 0;
}

int32_t
NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    trace_info("[NPP] %s\n", __func__);
    return 0;
}

void
NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}

void
NPP_Print(NPP instance, NPPrint* platformPrint)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}

int16_t
NPP_HandleEvent(NPP instance, void* event)
{
    trace_info("[NPP] %s\n", __func__);
    return 0;
}

void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP instance, NPFocusDirection direction)
{
    trace_info("[NPP] %s\n", __func__);
    return 1;
}

void
NPP_LostFocus(NPP instance)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}

void
NPP_URLRedirectNotify(NPP instance, const char* url, int32_t status, void* notifyData)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}

NPError
NPP_ClearSiteData(const char* site, uint64_t flags, uint64_t maxAge)
{
    trace_info("[NPP] %s\n", __func__);
    return NPERR_NO_ERROR;
}

char**
NPP_GetSitesWithData(void)
{
    trace_info("[NPP] %s\n", __func__);
    return NULL;
}

void
NPP_DidComposite(NPP instance)
{
    trace_info("[NPP] %s\n", __func__);
    return;
}
