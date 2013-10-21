#include "reverse_constant.h"

#define CASE(q) case q: return #q


const char *
reverse_npp_variable(NPPVariable var)
{
    switch (var) {
    CASE(NPPVpluginNameString);
    CASE(NPPVpluginDescriptionString);
    CASE(NPPVpluginWindowBool);
    CASE(NPPVpluginTransparentBool);
    CASE(NPPVjavaClass);
    CASE(NPPVpluginWindowSize);
    CASE(NPPVpluginTimerInterval);
    CASE(NPPVpluginScriptableInstance);
    CASE(NPPVpluginScriptableIID);
    CASE(NPPVjavascriptPushCallerBool);
    CASE(NPPVpluginKeepLibraryInMemory);
    CASE(NPPVpluginNeedsXEmbed);
    CASE(NPPVpluginScriptableNPObject);
    CASE(NPPVformValue);
    CASE(NPPVpluginUrlRequestsDisplayedBool);
    CASE(NPPVpluginWantsAllNetworkStreams);
    CASE(NPPVpluginNativeAccessibleAtkPlugId);
    CASE(NPPVpluginCancelSrcStream);
    CASE(NPPVsupportsAdvancedKeyHandling);
    CASE(NPPVpluginUsesDOMForCursorBool);
    CASE(NPPVpluginDrawingModel);
    default: return "UNKNOWNVAR";
    }
}

const char *
reverse_npn_variable(NPNVariable var)
{
    switch (var) {
    CASE(NPNVxDisplay);
    CASE(NPNVxtAppContext);
    CASE(NPNVnetscapeWindow);
    CASE(NPNVjavascriptEnabledBool);
    CASE(NPNVasdEnabledBool);
    CASE(NPNVisOfflineBool);
    CASE(NPNVserviceManager);
    CASE(NPNVDOMElement);
    CASE(NPNVDOMWindow);
    CASE(NPNVToolkit);
    CASE(NPNVSupportsXEmbedBool);
    CASE(NPNVWindowNPObject);
    CASE(NPNVPluginElementNPObject);
    CASE(NPNVSupportsWindowless);
    CASE(NPNVprivateModeBool);
    CASE(NPNVsupportsAdvancedKeyHandling);
    CASE(NPNVdocumentOrigin);
    CASE(NPNVpluginDrawingModel);
    CASE(NPNVsupportsAsyncBitmapSurfaceBool);
    default: return "UNKNOWNVAR";
    }
}

const char *
reverse_pp_url_response_property(PP_URLResponseProperty prop)
{
    switch (prop) {
    CASE(PP_URLRESPONSEPROPERTY_URL);
    CASE(PP_URLRESPONSEPROPERTY_REDIRECTURL);
    CASE(PP_URLRESPONSEPROPERTY_REDIRECTMETHOD);
    CASE(PP_URLRESPONSEPROPERTY_STATUSCODE);
    CASE(PP_URLRESPONSEPROPERTY_STATUSLINE);
    CASE(PP_URLRESPONSEPROPERTY_HEADERS);
    default: return "UNKNOWNPROP";
    }
}
