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

const char *
reverse_pp_url_request_property(PP_URLRequestProperty prop)
{
    switch (prop) {
    CASE(PP_URLREQUESTPROPERTY_URL);
    CASE(PP_URLREQUESTPROPERTY_METHOD);
    CASE(PP_URLREQUESTPROPERTY_HEADERS);
    CASE(PP_URLREQUESTPROPERTY_STREAMTOFILE);
    CASE(PP_URLREQUESTPROPERTY_FOLLOWREDIRECTS);
    CASE(PP_URLREQUESTPROPERTY_RECORDDOWNLOADPROGRESS);
    CASE(PP_URLREQUESTPROPERTY_RECORDUPLOADPROGRESS);
    CASE(PP_URLREQUESTPROPERTY_CUSTOMREFERRERURL);
    CASE(PP_URLREQUESTPROPERTY_ALLOWCROSSORIGINREQUESTS);
    CASE(PP_URLREQUESTPROPERTY_ALLOWCREDENTIALS);
    CASE(PP_URLREQUESTPROPERTY_CUSTOMCONTENTTRANSFERENCODING);
    CASE(PP_URLREQUESTPROPERTY_PREFETCHBUFFERUPPERTHRESHOLD);
    CASE(PP_URLREQUESTPROPERTY_PREFETCHBUFFERLOWERTHRESHOLD);
    CASE(PP_URLREQUESTPROPERTY_CUSTOMUSERAGENT);
    default: return "UNKNONWPROP";
    }
}

const char *
reverse_pp_text_input_type(PP_TextInput_Type type)
{
    switch (type) {
    CASE(PP_TEXTINPUT_TYPE_NONE);
    CASE(PP_TEXTINPUT_TYPE_TEXT);
    CASE(PP_TEXTINPUT_TYPE_PASSWORD);
    CASE(PP_TEXTINPUT_TYPE_SEARCH);
    CASE(PP_TEXTINPUT_TYPE_EMAIL);
    CASE(PP_TEXTINPUT_TYPE_NUMBER);
    CASE(PP_TEXTINPUT_TYPE_TELEPHONE);
    CASE(PP_TEXTINPUT_TYPE_URL);
    default: return "UNKNOWNTYPE";
    }
}

const char *
reverse_pp_flash_setting(PP_FlashSetting setting)
{
    switch (setting) {
    CASE(PP_FLASHSETTING_3DENABLED);
    CASE(PP_FLASHSETTING_INCOGNITO);
    CASE(PP_FLASHSETTING_STAGE3DENABLED);
    CASE(PP_FLASHSETTING_LANGUAGE);
    CASE(PP_FLASHSETTING_NUMCORES);
    CASE(PP_FLASHSETTING_LSORESTRICTIONS);
    CASE(PP_FLASHSETTING_STAGE3DBASELINEENABLED);
    default: return "UNKNOWNSETTING";
    }
}
