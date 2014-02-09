#include <assert.h>
#include <inttypes.h>
#include <npapi.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <pthread.h>
#include "trace.h"
#include "reverse_constant.h"
#include "np.h"
#include "pp_interface.h"
#include "interface_list.h"
#include "tables.h"
#include <ppapi/c/ppp_instance.h>
#include "globals.h"

#define NO_IMPL assert(0 && "no implementation yet")

static
PP_Instance
generate_new_pp_instance_id(void)
{
    static int32_t instance_id = 10;
    static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

    pthread_mutex_lock(&m);
    int32_t result = instance_id++;
    pthread_mutex_unlock(&m);

    return result;
}

NPError
NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode, int16_t argc, char* argn[],
        char* argv[], NPSavedData* saved)
{
    // TODO: mode parameter handling
    int k;
    struct np_priv_s *priv;
    trace_info("[NPP] {part} %s pluginType=%s instance=%p, mode=%d, argc=%d, saved=%p\n", __func__,
               pluginType, instance, mode, argc, saved);

    if (!obligatory_npp_instance)
        obligatory_npp_instance = instance;

    for (k = 0; k < argc; k ++)
        trace_info("            argn[%d] = %s, argv[%d] = %s\n", k, argn[k], k, argv[k]);

    priv = calloc(sizeof(*priv), 1);
    instance->pdata = priv;
    if (!priv)
        return NPERR_OUT_OF_MEMORY_ERROR;

    priv->ppp_instance_1_1 = ppp_get_interface(PPP_INSTANCE_INTERFACE_1_1);
    if (!priv->ppp_instance_1_1)
        return NPERR_GENERIC_ERROR;

    priv->argc = argc;
    priv->argn = malloc(argc * sizeof(char*));
    priv->argv = malloc(argc * sizeof(char*));
    for (k = 0; k < argc; k ++) {
        priv->argn[k] = strdup(argn[k] ? argn[k] : "");
        priv->argv[k] = strdup(argv[k] ? argv[k] : "");

        if (strcasecmp(argn[k], "src") == 0) {
            priv->instance_url = strdup(argv[k]);
        }
    }

    priv->pp_instance_id = generate_new_pp_instance_id();
    priv->instance_loaded = 1;
    tables_add_pp_np_mapping(priv->pp_instance_id, priv);

    // request windowless operation
    npn.setvalue(instance, NPPVpluginWindowBool, (void*)0);

    priv->ppp_instance_1_1->DidCreate(priv->pp_instance_id, priv->argc, priv->argn, priv->argv);
    trace_info("-----------------------------------------\n");
    PP_Resource urll = ppb_url_loader_create(priv->pp_instance_id);
    PP_Resource urlri = ppb_url_request_info_create(priv->pp_instance_id);
    ppb_url_loader_open(urll, urlri, PP_BlockUntilComplete());
    priv->ppp_instance_1_1->HandleDocumentLoad(priv->pp_instance_id, urll);
    trace_info("=========================================\n");

    return NPERR_NO_ERROR;
}

NPError
NPP_Destroy(NPP instance, NPSavedData** save)
{
    trace_info("[NPP] {full} %s instance=%p, save=%p\n", __func__, instance, save);
    struct np_priv_s *priv = instance->pdata;
    priv->ppp_instance_1_1->DidDestroy(priv->pp_instance_id);
    if (save)
        *save = NULL;
    return NPERR_NO_ERROR;
}

NPError
NPP_SetWindow(NPP instance, NPWindow* window)
{
    char *window_str = trace_np_window_as_string(window);
    trace_info("[NPP] {part} %s instance=%p, window=%s\n", __func__, instance, window_str);
    free(window_str);

    struct np_priv_s *priv = instance->pdata;
    priv->wnd = (Window)window->window;
    priv->x = window->x;
    priv->y = window->y;
    priv->width = window->width;
    priv->height = window->height;
    priv->clip_rect = window->clipRect;
    priv->ws_info = window->ws_info;
    priv->window_type = window->type;
    return NPERR_NO_ERROR;
}

NPError
NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
    trace_info("[NPP] {zilch} %s instance=%p, type=%s, stream={.pdata=%p, .ndata=%p, .url=%s, "
               "end=%u, lastmodified=%u, .headers=%s}, seekable=%d\n", __func__, instance, type,
               stream->pdata, stream->ndata, stream->url, stream->end, stream->lastmodified,
               stream->headers, seekable);

    PP_Resource loader = tables_pop_url_pair(stream->url);
    if (!loader) {
        // ignoring unrequested streams
        return NPERR_NO_ERROR;
    }

    printf("Hi from NPP_NewStream. loader = %d\n", loader);

    return NPERR_NO_ERROR;
}

NPError
NPP_DestroyStream(NPP instance, NPStream* stream, NPReason reason)
{
    trace_info("[NPP] {zilch} %s instance=%p, stream=%p, reason=%d\n", __func__,
               instance, stream, reason);
    return NPERR_NO_ERROR;
}

int32_t
NPP_WriteReady(NPP instance, NPStream* stream)
{
    trace_info("[NPP] {fake} %s instance=%p, stream=%p\n", __func__, instance, stream);
    return 1024*1024;
}

int32_t
NPP_Write(NPP instance, NPStream* stream, int32_t offset, int32_t len, void* buffer)
{
    trace_info("[NPP] {fake} %s instance=%p, stream=%p, offset=%d, len=%d, buffer=%p\n", __func__,
               instance, stream, offset, len, buffer);
    return len;
}

void
NPP_StreamAsFile(NPP instance, NPStream* stream, const char* fname)
{
    trace_info("[NPP] {zilch} %s instance=%p, stream=%p, fname=%s\n", __func__,
               instance, stream, fname);
    return;
}

void
NPP_Print(NPP instance, NPPrint* platformPrint)
{
    trace_info("[NPP] {zilch} %s instance=%p, platformPrint=%p\n", __func__,
               instance, platformPrint);
    return;
}

int16_t
NPP_HandleEvent(NPP instance, void* event)
{
    trace_info("[NPP] {zilch} %s instance=%p, event=%p\n", __func__, instance, event);
    return 0;
}

void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
    trace_info("[NPP] {zilch} %s instance=%p, url=%s, reason=%d, notifyData=%p\n", __func__,
               instance, url, reason, notifyData);
    return;
}

NPError
NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    trace_info("[NPP] {part} %s instance=%p, variable=%s\n", __func__,
               instance, reverse_npp_variable(variable));
    switch (variable) {
    case NPPVpluginNameString:
        NO_IMPL; break;
    case NPPVpluginDescriptionString:
        NO_IMPL; break;
    case NPPVpluginWindowBool:
        NO_IMPL; break;
    case NPPVpluginTransparentBool:
        NO_IMPL; break;
    case NPPVjavaClass:
        NO_IMPL; break;
    case NPPVpluginWindowSize:
        NO_IMPL; break;
    case NPPVpluginTimerInterval:
        NO_IMPL; break;
    case NPPVpluginScriptableInstance:
        NO_IMPL; break;
    case NPPVpluginScriptableIID:
        NO_IMPL; break;
    case NPPVjavascriptPushCallerBool:
        NO_IMPL; break;
    case NPPVpluginKeepLibraryInMemory:
        NO_IMPL; break;
    case NPPVpluginNeedsXEmbed:
        *(int *)value = 1;
        break;
    case NPPVpluginScriptableNPObject:
        *(void **)value = NULL;
        break;
    case NPPVformValue:
        NO_IMPL; break;
    case NPPVpluginUrlRequestsDisplayedBool:
        NO_IMPL; break;
    case NPPVpluginWantsAllNetworkStreams:
        NO_IMPL; break;
    case NPPVpluginNativeAccessibleAtkPlugId:
        NO_IMPL; break;
    case NPPVpluginCancelSrcStream:
        NO_IMPL; break;
    case NPPVsupportsAdvancedKeyHandling:
        NO_IMPL; break;
    case NPPVpluginUsesDOMForCursorBool:
        NO_IMPL; break;
    case NPPVpluginDrawingModel:
        NO_IMPL; break;
    default:
        NO_IMPL; break;
    }

    return NPERR_NO_ERROR;
}

NPError
NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    trace_info("[NPP] {zilch} %s instance=%p, variable=%s, value=%p\n", __func__,
               instance, reverse_npn_variable(variable), value);
    return NPERR_NO_ERROR;
}

NPBool
NPP_GotFocus(NPP instance, NPFocusDirection direction)
{
    trace_info("[NPP] {zilch} %s instance=%p, direction=%d\n", __func__, instance, direction);
    return 1;
}

void
NPP_LostFocus(NPP instance)
{
    trace_info("[NPP] {zilch} %s instance=%p\n", __func__, instance);
    return;
}

void
NPP_URLRedirectNotify(NPP instance, const char* url, int32_t status, void* notifyData)
{
    trace_info("[NPP] {zilch} %s instance=%p, url=%s, status=%d, notifyData=%p\n", __func__,
               instance, url, status, notifyData);
    return;
}

NPError
NPP_ClearSiteData(const char* site, uint64_t flags, uint64_t maxAge)
{
    trace_info("[NPP] {zilch} %s site=%s, flags=%"PRIu64", maxAge=%"PRIu64"\n", __func__,
               site, flags, maxAge);
    return NPERR_NO_ERROR;
}

char**
NPP_GetSitesWithData(void)
{
    trace_info("[NPP] {zilch} %s\n", __func__);
    return NULL;
}

void
NPP_DidComposite(NPP instance)
{
    trace_info("[NPP] {zilch} %s instance=%p\n", __func__, instance);
    return;
}
