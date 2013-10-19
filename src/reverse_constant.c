#include "reverse_constant.h"

const char *
reverse_npp_variable(NPPVariable var)
{
    switch (var) {
    case NPPVpluginNameString: return "NPPVpluginNameString";
    case NPPVpluginDescriptionString: return "NPPVpluginDescriptionString";
    case NPPVpluginWindowBool: return "NPPVpluginWindowBool";
    case NPPVpluginTransparentBool: return "NPPVpluginTransparentBool";
    case NPPVjavaClass: return "NPPVjavaClass";
    case NPPVpluginWindowSize: return "NPPVpluginWindowSize";
    case NPPVpluginTimerInterval: return "NPPVpluginTimerInterval";
    case NPPVpluginScriptableInstance: return "NPPVpluginScriptableInstance";
    case NPPVpluginScriptableIID: return "NPPVpluginScriptableIID";
    case NPPVjavascriptPushCallerBool: return "NPPVjavascriptPushCallerBool";
    case NPPVpluginKeepLibraryInMemory: return "NPPVpluginKeepLibraryInMemory";
    case NPPVpluginNeedsXEmbed: return "NPPVpluginNeedsXEmbed";
    case NPPVpluginScriptableNPObject: return "NPPVpluginScriptableNPObject";
    case NPPVformValue: return "NPPVformValue";
    case NPPVpluginUrlRequestsDisplayedBool: return "NPPVpluginUrlRequestsDisplayedBool";
    case NPPVpluginWantsAllNetworkStreams: return "NPPVpluginWantsAllNetworkStreams";
    case NPPVpluginNativeAccessibleAtkPlugId: return "NPPVpluginNativeAccessibleAtkPlugId";
    case NPPVpluginCancelSrcStream: return "NPPVpluginCancelSrcStream";
    case NPPVsupportsAdvancedKeyHandling: return "NPPVsupportsAdvancedKeyHandling";
    case NPPVpluginUsesDOMForCursorBool: return "NPPVpluginUsesDOMForCursorBool";
    case NPPVpluginDrawingModel: return "NPPVpluginDrawingModel";
    default: return "UNKNOWNVARIABLE";
    }
}

const char *
reverse_npn_variable(NPNVariable var)
{
    switch (var) {
    case NPNVxDisplay: return "NPNVxDisplay";
    case NPNVxtAppContext: return "NPNVxtAppContext";
    case NPNVnetscapeWindow: return "NPNVnetscapeWindow";
    case NPNVjavascriptEnabledBool: return "NPNVjavascriptEnabledBool";
    case NPNVasdEnabledBool: return "NPNVasdEnabledBool";
    case NPNVisOfflineBool: return "NPNVisOfflineBool";
    case NPNVserviceManager: return "NPNVserviceManager";
    case NPNVDOMElement: return "NPNVDOMElement";
    case NPNVDOMWindow: return "NPNVDOMWindow";
    case NPNVToolkit: return "NPNVToolkit";
    case NPNVSupportsXEmbedBool: return "NPNVSupportsXEmbedBool";
    case NPNVWindowNPObject: return "NPNVWindowNPObject";
    case NPNVPluginElementNPObject: return "NPNVPluginElementNPObject";
    case NPNVSupportsWindowless: return "NPNVSupportsWindowless";
    case NPNVprivateModeBool: return "NPNVprivateModeBool";
    case NPNVsupportsAdvancedKeyHandling: return "NPNVsupportsAdvancedKeyHandling";
    case NPNVdocumentOrigin: return "NPNVdocumentOrigin";
    case NPNVpluginDrawingModel: return "NPNVpluginDrawingModel";
    case NPNVsupportsAsyncBitmapSurfaceBool: return "NPNVsupportsAsyncBitmapSurfaceBool";
    default: return "UNKNOWNVARIABLE";
    }
}
