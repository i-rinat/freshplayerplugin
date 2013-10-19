#include <npapi.h>
#include <stdlib.h>
#include "trace.h"




NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[],
        char* argv[], NPSavedData* saved)
{
    int k;
    trace_info("[NPP] %s pluginType=%s instance=%p, mode=%d, argc=%d, saved=%p\n", __func__,
               pluginType, instance, mode, argc, saved);
    for (k = 0; k < argc; k ++)
        trace_info("            argn[%d] = %s, argv[%d] = %s\n", k, argn[k], k, argv[k]);
    return NPERR_NO_ERROR;
}


NPError
NPP_Destroy(NPP instance, NPSavedData** save)
{
    trace_info("[NPP] %s instance=%p, save=%p\n", __func__, instance, save);
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow* window)
{
    trace_info("[NPP] %s instance=%p, window=%p\n", __func__, instance, window);
    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    trace_info("[NPP] %s instance=%p, type=%s, stream=%p, seekable=%d, stype=%p\n", __func__,
               instance, type, stream, seekable, stype);
    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    trace_info("[NPP] %s instance=%p, stream=%p, reason=%d\n", __func__,
               instance, stream, reason);
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream* stream)
{
    trace_info("[NPP] %s instance=%p, stream=%p\n", __func__, instance, stream);
    return 0;
}

int32_t
NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    trace_info("[NPP] %s instance=%p, stream=%p, offset=%d, len=%d, buffer=%p\n", __func__,
               instance, stream, offset, len, buffer);
    return 0;
}

void
NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    trace_info("[NPP] %s instance=%p, stream=%p, fname=%s\n", __func__, instance, stream, fname);
    return;
}

void
NPP_Print(NPP instance, NPPrint* platformPrint)
{
    trace_info("[NPP] %s instance=%p, platformPrint=%p\n", __func__, instance, platformPrint);
    return;
}

int16_t
NPP_HandleEvent(NPP instance, void* event)
{
    trace_info("[NPP] %s instance=%p, event=%p\n", __func__, instance, event);
    return 0;
}

void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    trace_info("[NPP] %s instance=%p, url=%s, reason=%d, notifyData=%p\n", __func__,
               instance, url, reason, notifyData);
    return;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    trace_info("[NPP] %s instance=%p, variable=%d, value=%p\n", __func__,
               instance, variable, value);
    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    trace_info("[NPP] %s instance=%p, variable=%d, value=%p\n", __func__,
               instance, variable, value);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP instance, NPFocusDirection direction)
{
    trace_info("[NPP] %s instance=%p, direction=%d\n", __func__, instance, direction);
    return 1;
}

void
NPP_LostFocus(NPP instance)
{
    trace_info("[NPP] %s instance=%p\n", __func__, instance);
    return;
}

void
NPP_URLRedirectNotify(NPP instance, const char* url, int32_t status, void* notifyData)
{
    trace_info("[NPP] %s instance=%p, url=%s, status=%d, notifyData=%p\n", __func__,
               instance, url, status, notifyData);
    return;
}

NPError
NPP_ClearSiteData(const char* site, uint64_t flags, uint64_t maxAge)
{
    trace_info("[NPP] %s site=%s, flags=%llu, maxAge=%llu\n", __func__, site, flags, maxAge);
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
    trace_info("[NPP] %s instance=%p\n", __func__, instance);
    return;
}
