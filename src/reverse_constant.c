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

#include "reverse_constant.h"
#include <X11/Xlib.h>
#include <ppapi/c/pp_graphics_3d.h>


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
    CASE(NPNVmuteAudioBool);
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
reverse_pp_text_input_type(PP_TextInput_Type_Dev type)
{
    switch (type) {
    CASE(PP_TEXTINPUT_TYPE_DEV_NONE);
    CASE(PP_TEXTINPUT_TYPE_DEV_TEXT);
    CASE(PP_TEXTINPUT_TYPE_DEV_PASSWORD);
    CASE(PP_TEXTINPUT_TYPE_DEV_SEARCH);
    CASE(PP_TEXTINPUT_TYPE_DEV_EMAIL);
    CASE(PP_TEXTINPUT_TYPE_DEV_NUMBER);
    CASE(PP_TEXTINPUT_TYPE_DEV_TELEPHONE);
    CASE(PP_TEXTINPUT_TYPE_DEV_URL);
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

const char *
reverse_pp_image_data_format(PP_ImageDataFormat fmt)
{
    switch (fmt) {
    CASE(PP_IMAGEDATAFORMAT_BGRA_PREMUL);
    CASE(PP_IMAGEDATAFORMAT_RGBA_PREMUL);
    default: return "UNKNOWNIMAGEDATAFORMAT";
    }
}

const char *
reverse_xevent_type(int type)
{
    switch (type) {
    CASE(KeyPress);
    CASE(KeyRelease);
    CASE(ButtonPress);
    CASE(ButtonRelease);
    CASE(MotionNotify);
    CASE(EnterNotify);
    CASE(LeaveNotify);
    CASE(FocusIn);
    CASE(FocusOut);
    CASE(KeymapNotify);
    CASE(Expose);
    CASE(GraphicsExpose);
    CASE(NoExpose);
    CASE(VisibilityNotify);
    CASE(CreateNotify);
    CASE(DestroyNotify);
    CASE(UnmapNotify);
    CASE(MapNotify);
    CASE(MapRequest);
    CASE(ReparentNotify);
    CASE(ConfigureNotify);
    CASE(ConfigureRequest);
    CASE(GravityNotify);
    CASE(ResizeRequest);
    CASE(CirculateNotify);
    CASE(CirculateRequest);
    CASE(PropertyNotify);
    CASE(SelectionClear);
    CASE(SelectionRequest);
    CASE(SelectionNotify);
    CASE(ColormapNotify);
    CASE(ClientMessage);
    CASE(MappingNotify);
    CASE(GenericEvent);
    default:
        return "UNKNOWNXEVENT";
    }
}

const char *
reverse_char_set_conversion_error(enum PP_CharSet_ConversionError e)
{
    switch (e) {
    CASE(PP_CHARSET_CONVERSIONERROR_FAIL);
    CASE(PP_CHARSET_CONVERSIONERROR_SKIP);
    CASE(PP_CHARSET_CONVERSIONERROR_SUBSTITUTE);
    default:
        return "UNKNOWNCHARSETCONVERSION_ERROR";
    }
}

const char *
reverse_gl_enum(unsigned int e)
{
    switch (e) {
    case 0x0000: return "GL_POINTS";
    case 0x0001: return "GL_LINES";
    case 0x0002: return "GL_LINE_LOOP";
    case 0x0003: return "GL_LINE_STRIP";
    case 0x0004: return "GL_TRIANGLES";
    case 0x0005: return "GL_TRIANGLE_STRIP";
    case 0x0006: return "GL_TRIANGLE_FAN";
    case 0x0007: return "GL_QUADS";
    case 0x0008: return "GL_QUAD_STRIP";
    case 0x0009: return "GL_POLYGON";
    case 0x0100: return "GL_ACCUM";
    case 0x0101: return "GL_LOAD";
    case 0x0102: return "GL_RETURN";
    case 0x0103: return "GL_MULT";
    case 0x0104: return "GL_ADD";
    case 0x0200: return "GL_NEVER";
    case 0x0201: return "GL_LESS";
    case 0x0202: return "GL_EQUAL";
    case 0x0203: return "GL_LEQUAL";
    case 0x0204: return "GL_GREATER";
    case 0x0205: return "GL_NOTEQUAL";
    case 0x0206: return "GL_GEQUAL";
    case 0x0207: return "GL_ALWAYS";
    case 0x0300: return "GL_SRC_COLOR";
    case 0x0301: return "GL_ONE_MINUS_SRC_COLOR";
    case 0x0302: return "GL_SRC_ALPHA";
    case 0x0303: return "GL_ONE_MINUS_SRC_ALPHA";
    case 0x0304: return "GL_DST_ALPHA";
    case 0x0305: return "GL_ONE_MINUS_DST_ALPHA";
    case 0x0306: return "GL_DST_COLOR";
    case 0x0307: return "GL_ONE_MINUS_DST_COLOR";
    case 0x0308: return "GL_SRC_ALPHA_SATURATE";
    case 0x0400: return "GL_FRONT_LEFT";
    case 0x0401: return "GL_FRONT_RIGHT";
    case 0x0402: return "GL_BACK_LEFT";
    case 0x0403: return "GL_BACK_RIGHT";
    case 0x0404: return "GL_FRONT";
    case 0x0405: return "GL_BACK";
    case 0x0406: return "GL_LEFT";
    case 0x0407: return "GL_RIGHT";
    case 0x0408: return "GL_FRONT_AND_BACK";
    case 0x0409: return "GL_AUX0";
    case 0x040A: return "GL_AUX1";
    case 0x040B: return "GL_AUX2";
    case 0x040C: return "GL_AUX3";
    case 0x0500: return "GL_INVALID_ENUM";
    case 0x0501: return "GL_INVALID_VALUE";
    case 0x0502: return "GL_INVALID_OPERATION";
    case 0x0503: return "GL_STACK_OVERFLOW";
    case 0x0504: return "GL_STACK_UNDERFLOW";
    case 0x0505: return "GL_OUT_OF_MEMORY";
    case 0x0600: return "GL_2D";
    case 0x0601: return "GL_3D";
    case 0x0602: return "GL_3D_COLOR";
    case 0x0603: return "GL_3D_COLOR_TEXTURE";
    case 0x0604: return "GL_4D_COLOR_TEXTURE";
    case 0x0700: return "GL_PASS_THROUGH_TOKEN";
    case 0x0701: return "GL_POINT_TOKEN";
    case 0x0702: return "GL_LINE_TOKEN";
    case 0x0703: return "GL_POLYGON_TOKEN";
    case 0x0704: return "GL_BITMAP_TOKEN";
    case 0x0705: return "GL_DRAW_PIXEL_TOKEN";
    case 0x0706: return "GL_COPY_PIXEL_TOKEN";
    case 0x0707: return "GL_LINE_RESET_TOKEN";
    case 0x0800: return "GL_EXP";
    case 0x0801: return "GL_EXP2";
    case 0x0900: return "GL_CW";
    case 0x0901: return "GL_CCW";
    case 0x0A00: return "GL_COEFF";
    case 0x0A01: return "GL_ORDER";
    case 0x0A02: return "GL_DOMAIN";
    case 0x0B00: return "GL_CURRENT_COLOR";
    case 0x0B01: return "GL_CURRENT_INDEX";
    case 0x0B02: return "GL_CURRENT_NORMAL";
    case 0x0B03: return "GL_CURRENT_TEXTURE_COORDS";
    case 0x0B04: return "GL_CURRENT_RASTER_COLOR";
    case 0x0B05: return "GL_CURRENT_RASTER_INDEX";
    case 0x0B06: return "GL_CURRENT_RASTER_TEXTURE_COORDS";
    case 0x0B07: return "GL_CURRENT_RASTER_POSITION";
    case 0x0B08: return "GL_CURRENT_RASTER_POSITION_VALID";
    case 0x0B09: return "GL_CURRENT_RASTER_DISTANCE";
    case 0x0B10: return "GL_POINT_SMOOTH";
    case 0x0B11: return "GL_POINT_SIZE";
    case 0x0B12: return "GL_POINT_SIZE_RANGE";
    case 0x0B13: return "GL_POINT_SIZE_GRANULARITY";
    case 0x0B20: return "GL_LINE_SMOOTH";
    case 0x0B21: return "GL_LINE_WIDTH";
    case 0x0B22: return "GL_LINE_WIDTH_RANGE";
    case 0x0B23: return "GL_LINE_WIDTH_GRANULARITY";
    case 0x0B24: return "GL_LINE_STIPPLE";
    case 0x0B25: return "GL_LINE_STIPPLE_PATTERN";
    case 0x0B26: return "GL_LINE_STIPPLE_REPEAT";
    case 0x0B30: return "GL_LIST_MODE";
    case 0x0B31: return "GL_MAX_LIST_NESTING";
    case 0x0B32: return "GL_LIST_BASE";
    case 0x0B33: return "GL_LIST_INDEX";
    case 0x0B40: return "GL_POLYGON_MODE";
    case 0x0B41: return "GL_POLYGON_SMOOTH";
    case 0x0B42: return "GL_POLYGON_STIPPLE";
    case 0x0B43: return "GL_EDGE_FLAG";
    case 0x0B44: return "GL_CULL_FACE";
    case 0x0B45: return "GL_CULL_FACE_MODE";
    case 0x0B46: return "GL_FRONT_FACE";
    case 0x0B50: return "GL_LIGHTING";
    case 0x0B51: return "GL_LIGHT_MODEL_LOCAL_VIEWER";
    case 0x0B52: return "GL_LIGHT_MODEL_TWO_SIDE";
    case 0x0B53: return "GL_LIGHT_MODEL_AMBIENT";
    case 0x0B54: return "GL_SHADE_MODEL";
    case 0x0B55: return "GL_COLOR_MATERIAL_FACE";
    case 0x0B56: return "GL_COLOR_MATERIAL_PARAMETER";
    case 0x0B57: return "GL_COLOR_MATERIAL";
    case 0x0B60: return "GL_FOG";
    case 0x0B61: return "GL_FOG_INDEX";
    case 0x0B62: return "GL_FOG_DENSITY";
    case 0x0B63: return "GL_FOG_START";
    case 0x0B64: return "GL_FOG_END";
    case 0x0B65: return "GL_FOG_MODE";
    case 0x0B66: return "GL_FOG_COLOR";
    case 0x0B70: return "GL_DEPTH_RANGE";
    case 0x0B71: return "GL_DEPTH_TEST";
    case 0x0B72: return "GL_DEPTH_WRITEMASK";
    case 0x0B73: return "GL_DEPTH_CLEAR_VALUE";
    case 0x0B74: return "GL_DEPTH_FUNC";
    case 0x0B80: return "GL_ACCUM_CLEAR_VALUE";
    case 0x0B90: return "GL_STENCIL_TEST";
    case 0x0B91: return "GL_STENCIL_CLEAR_VALUE";
    case 0x0B92: return "GL_STENCIL_FUNC";
    case 0x0B93: return "GL_STENCIL_VALUE_MASK";
    case 0x0B94: return "GL_STENCIL_FAIL";
    case 0x0B95: return "GL_STENCIL_PASS_DEPTH_FAIL";
    case 0x0B96: return "GL_STENCIL_PASS_DEPTH_PASS";
    case 0x0B97: return "GL_STENCIL_REF";
    case 0x0B98: return "GL_STENCIL_WRITEMASK";
    case 0x0BA0: return "GL_MATRIX_MODE";
    case 0x0BA1: return "GL_NORMALIZE";
    case 0x0BA2: return "GL_VIEWPORT";
    case 0x0BA3: return "GL_MODELVIEW_STACK_DEPTH";
    case 0x0BA4: return "GL_PROJECTION_STACK_DEPTH";
    case 0x0BA5: return "GL_TEXTURE_STACK_DEPTH";
    case 0x0BA6: return "GL_MODELVIEW_MATRIX";
    case 0x0BA7: return "GL_PROJECTION_MATRIX";
    case 0x0BA8: return "GL_TEXTURE_MATRIX";
    case 0x0BB0: return "GL_ATTRIB_STACK_DEPTH";
    case 0x0BB1: return "GL_CLIENT_ATTRIB_STACK_DEPTH";
    case 0x0BC0: return "GL_ALPHA_TEST";
    case 0x0BC1: return "GL_ALPHA_TEST_FUNC";
    case 0x0BC2: return "GL_ALPHA_TEST_REF";
    case 0x0BD0: return "GL_DITHER";
    case 0x0BE0: return "GL_BLEND_DST";
    case 0x0BE1: return "GL_BLEND_SRC";
    case 0x0BE2: return "GL_BLEND";
    case 0x0BF0: return "GL_LOGIC_OP_MODE";
    case 0x0BF1: return "GL_LOGIC_OP";
    case 0x0BF2: return "GL_COLOR_LOGIC_OP";
    case 0x0C00: return "GL_AUX_BUFFERS";
    case 0x0C01: return "GL_DRAW_BUFFER";
    case 0x0C02: return "GL_READ_BUFFER";
    case 0x0C10: return "GL_SCISSOR_BOX";
    case 0x0C11: return "GL_SCISSOR_TEST";
    case 0x0C20: return "GL_INDEX_CLEAR_VALUE";
    case 0x0C21: return "GL_INDEX_WRITEMASK";
    case 0x0C22: return "GL_COLOR_CLEAR_VALUE";
    case 0x0C23: return "GL_COLOR_WRITEMASK";
    case 0x0C30: return "GL_INDEX_MODE";
    case 0x0C31: return "GL_RGBA_MODE";
    case 0x0C32: return "GL_DOUBLEBUFFER";
    case 0x0C33: return "GL_STEREO";
    case 0x0C40: return "GL_RENDER_MODE";
    case 0x0C50: return "GL_PERSPECTIVE_CORRECTION_HINT";
    case 0x0C51: return "GL_POINT_SMOOTH_HINT";
    case 0x0C52: return "GL_LINE_SMOOTH_HINT";
    case 0x0C53: return "GL_POLYGON_SMOOTH_HINT";
    case 0x0C54: return "GL_FOG_HINT";
    case 0x0C60: return "GL_TEXTURE_GEN_S";
    case 0x0C61: return "GL_TEXTURE_GEN_T";
    case 0x0C62: return "GL_TEXTURE_GEN_R";
    case 0x0C63: return "GL_TEXTURE_GEN_Q";
    case 0x0C70: return "GL_PIXEL_MAP_I_TO_I";
    case 0x0C71: return "GL_PIXEL_MAP_S_TO_S";
    case 0x0C72: return "GL_PIXEL_MAP_I_TO_R";
    case 0x0C73: return "GL_PIXEL_MAP_I_TO_G";
    case 0x0C74: return "GL_PIXEL_MAP_I_TO_B";
    case 0x0C75: return "GL_PIXEL_MAP_I_TO_A";
    case 0x0C76: return "GL_PIXEL_MAP_R_TO_R";
    case 0x0C77: return "GL_PIXEL_MAP_G_TO_G";
    case 0x0C78: return "GL_PIXEL_MAP_B_TO_B";
    case 0x0C79: return "GL_PIXEL_MAP_A_TO_A";
    case 0x0CB0: return "GL_PIXEL_MAP_I_TO_I_SIZE";
    case 0x0CB1: return "GL_PIXEL_MAP_S_TO_S_SIZE";
    case 0x0CB2: return "GL_PIXEL_MAP_I_TO_R_SIZE";
    case 0x0CB3: return "GL_PIXEL_MAP_I_TO_G_SIZE";
    case 0x0CB4: return "GL_PIXEL_MAP_I_TO_B_SIZE";
    case 0x0CB5: return "GL_PIXEL_MAP_I_TO_A_SIZE";
    case 0x0CB6: return "GL_PIXEL_MAP_R_TO_R_SIZE";
    case 0x0CB7: return "GL_PIXEL_MAP_G_TO_G_SIZE";
    case 0x0CB8: return "GL_PIXEL_MAP_B_TO_B_SIZE";
    case 0x0CB9: return "GL_PIXEL_MAP_A_TO_A_SIZE";
    case 0x0CF0: return "GL_UNPACK_SWAP_BYTES";
    case 0x0CF1: return "GL_UNPACK_LSB_FIRST";
    case 0x0CF2: return "GL_UNPACK_ROW_LENGTH";
    case 0x0CF3: return "GL_UNPACK_SKIP_ROWS";
    case 0x0CF4: return "GL_UNPACK_SKIP_PIXELS";
    case 0x0CF5: return "GL_UNPACK_ALIGNMENT";
    case 0x0D00: return "GL_PACK_SWAP_BYTES";
    case 0x0D01: return "GL_PACK_LSB_FIRST";
    case 0x0D02: return "GL_PACK_ROW_LENGTH";
    case 0x0D03: return "GL_PACK_SKIP_ROWS";
    case 0x0D04: return "GL_PACK_SKIP_PIXELS";
    case 0x0D05: return "GL_PACK_ALIGNMENT";
    case 0x0D10: return "GL_MAP_COLOR";
    case 0x0D11: return "GL_MAP_STENCIL";
    case 0x0D12: return "GL_INDEX_SHIFT";
    case 0x0D13: return "GL_INDEX_OFFSET";
    case 0x0D14: return "GL_RED_SCALE";
    case 0x0D15: return "GL_RED_BIAS";
    case 0x0D16: return "GL_ZOOM_X";
    case 0x0D17: return "GL_ZOOM_Y";
    case 0x0D18: return "GL_GREEN_SCALE";
    case 0x0D19: return "GL_GREEN_BIAS";
    case 0x0D1A: return "GL_BLUE_SCALE";
    case 0x0D1B: return "GL_BLUE_BIAS";
    case 0x0D1C: return "GL_ALPHA_SCALE";
    case 0x0D1D: return "GL_ALPHA_BIAS";
    case 0x0D1E: return "GL_DEPTH_SCALE";
    case 0x0D1F: return "GL_DEPTH_BIAS";
    case 0x0D30: return "GL_MAX_EVAL_ORDER";
    case 0x0D31: return "GL_MAX_LIGHTS";
    case 0x0D32: return "GL_MAX_CLIP_PLANES";
    case 0x0D33: return "GL_MAX_TEXTURE_SIZE";
    case 0x0D34: return "GL_MAX_PIXEL_MAP_TABLE";
    case 0x0D35: return "GL_MAX_ATTRIB_STACK_DEPTH";
    case 0x0D36: return "GL_MAX_MODELVIEW_STACK_DEPTH";
    case 0x0D37: return "GL_MAX_NAME_STACK_DEPTH";
    case 0x0D38: return "GL_MAX_PROJECTION_STACK_DEPTH";
    case 0x0D39: return "GL_MAX_TEXTURE_STACK_DEPTH";
    case 0x0D3A: return "GL_MAX_VIEWPORT_DIMS";
    case 0x0D3B: return "GL_MAX_CLIENT_ATTRIB_STACK_DEPTH";
    case 0x0D50: return "GL_SUBPIXEL_BITS";
    case 0x0D51: return "GL_INDEX_BITS";
    case 0x0D52: return "GL_RED_BITS";
    case 0x0D53: return "GL_GREEN_BITS";
    case 0x0D54: return "GL_BLUE_BITS";
    case 0x0D55: return "GL_ALPHA_BITS";
    case 0x0D56: return "GL_DEPTH_BITS";
    case 0x0D57: return "GL_STENCIL_BITS";
    case 0x0D58: return "GL_ACCUM_RED_BITS";
    case 0x0D59: return "GL_ACCUM_GREEN_BITS";
    case 0x0D5A: return "GL_ACCUM_BLUE_BITS";
    case 0x0D5B: return "GL_ACCUM_ALPHA_BITS";
    case 0x0D70: return "GL_NAME_STACK_DEPTH";
    case 0x0D80: return "GL_AUTO_NORMAL";
    case 0x0D90: return "GL_MAP1_COLOR_4";
    case 0x0D91: return "GL_MAP1_INDEX";
    case 0x0D92: return "GL_MAP1_NORMAL";
    case 0x0D93: return "GL_MAP1_TEXTURE_COORD_1";
    case 0x0D94: return "GL_MAP1_TEXTURE_COORD_2";
    case 0x0D95: return "GL_MAP1_TEXTURE_COORD_3";
    case 0x0D96: return "GL_MAP1_TEXTURE_COORD_4";
    case 0x0D97: return "GL_MAP1_VERTEX_3";
    case 0x0D98: return "GL_MAP1_VERTEX_4";
    case 0x0DB0: return "GL_MAP2_COLOR_4";
    case 0x0DB1: return "GL_MAP2_INDEX";
    case 0x0DB2: return "GL_MAP2_NORMAL";
    case 0x0DB3: return "GL_MAP2_TEXTURE_COORD_1";
    case 0x0DB4: return "GL_MAP2_TEXTURE_COORD_2";
    case 0x0DB5: return "GL_MAP2_TEXTURE_COORD_3";
    case 0x0DB6: return "GL_MAP2_TEXTURE_COORD_4";
    case 0x0DB7: return "GL_MAP2_VERTEX_3";
    case 0x0DB8: return "GL_MAP2_VERTEX_4";
    case 0x0DD0: return "GL_MAP1_GRID_DOMAIN";
    case 0x0DD1: return "GL_MAP1_GRID_SEGMENTS";
    case 0x0DD2: return "GL_MAP2_GRID_DOMAIN";
    case 0x0DD3: return "GL_MAP2_GRID_SEGMENTS";
    case 0x0DE0: return "GL_TEXTURE_1D";
    case 0x0DE1: return "GL_TEXTURE_2D";
    case 0x0DF0: return "GL_FEEDBACK_BUFFER_POINTER";
    case 0x0DF1: return "GL_FEEDBACK_BUFFER_SIZE";
    case 0x0DF2: return "GL_FEEDBACK_BUFFER_TYPE";
    case 0x0DF3: return "GL_SELECTION_BUFFER_POINTER";
    case 0x0DF4: return "GL_SELECTION_BUFFER_SIZE";
    case 0x1000: return "GL_TEXTURE_WIDTH";
    case 0x1001: return "GL_TEXTURE_HEIGHT";
    case 0x1003: return "GL_TEXTURE_COMPONENTS";
    case 0x1004: return "GL_TEXTURE_BORDER_COLOR";
    case 0x1005: return "GL_TEXTURE_BORDER";
    case 0x1100: return "GL_DONT_CARE";
    case 0x1101: return "GL_FASTEST";
    case 0x1102: return "GL_NICEST";
    case 0x1200: return "GL_AMBIENT";
    case 0x1201: return "GL_DIFFUSE";
    case 0x1202: return "GL_SPECULAR";
    case 0x1203: return "GL_POSITION";
    case 0x1204: return "GL_SPOT_DIRECTION";
    case 0x1205: return "GL_SPOT_EXPONENT";
    case 0x1206: return "GL_SPOT_CUTOFF";
    case 0x1207: return "GL_CONSTANT_ATTENUATION";
    case 0x1208: return "GL_LINEAR_ATTENUATION";
    case 0x1209: return "GL_QUADRATIC_ATTENUATION";
    case 0x1300: return "GL_COMPILE";
    case 0x1301: return "GL_COMPILE_AND_EXECUTE";
    case 0x1400: return "GL_BYTE";
    case 0x1401: return "GL_UNSIGNED_BYTE";
    case 0x1402: return "GL_SHORT";
    case 0x1403: return "GL_UNSIGNED_SHORT";
    case 0x1404: return "GL_INT";
    case 0x1405: return "GL_UNSIGNED_INT";
    case 0x1406: return "GL_FLOAT";
    case 0x1407: return "GL_2_BYTES";
    case 0x1408: return "GL_3_BYTES";
    case 0x1409: return "GL_4_BYTES";
    case 0x140A: return "GL_DOUBLE";
    case 0x1500: return "GL_CLEAR";
    case 0x1501: return "GL_AND";
    case 0x1502: return "GL_AND_REVERSE";
    case 0x1503: return "GL_COPY";
    case 0x1504: return "GL_AND_INVERTED";
    case 0x1505: return "GL_NOOP";
    case 0x1506: return "GL_XOR";
    case 0x1507: return "GL_OR";
    case 0x1508: return "GL_NOR";
    case 0x1509: return "GL_EQUIV";
    case 0x150A: return "GL_INVERT";
    case 0x150B: return "GL_OR_REVERSE";
    case 0x150C: return "GL_COPY_INVERTED";
    case 0x150D: return "GL_OR_INVERTED";
    case 0x150E: return "GL_NAND";
    case 0x150F: return "GL_SET";
    case 0x1600: return "GL_EMISSION";
    case 0x1601: return "GL_SHININESS";
    case 0x1602: return "GL_AMBIENT_AND_DIFFUSE";
    case 0x1603: return "GL_COLOR_INDEXES";
    case 0x1700: return "GL_MODELVIEW";
    case 0x1701: return "GL_PROJECTION";
    case 0x1702: return "GL_TEXTURE";
    case 0x1800: return "GL_COLOR";
    case 0x1801: return "GL_DEPTH";
    case 0x1802: return "GL_STENCIL";
    case 0x1900: return "GL_COLOR_INDEX";
    case 0x1901: return "GL_STENCIL_INDEX";
    case 0x1902: return "GL_DEPTH_COMPONENT";
    case 0x1903: return "GL_RED";
    case 0x1904: return "GL_GREEN";
    case 0x1905: return "GL_BLUE";
    case 0x1906: return "GL_ALPHA";
    case 0x1907: return "GL_RGB";
    case 0x1908: return "GL_RGBA";
    case 0x1909: return "GL_LUMINANCE";
    case 0x190A: return "GL_LUMINANCE_ALPHA";
    case 0x1A00: return "GL_BITMAP";
    case 0x1B00: return "GL_POINT";
    case 0x1B01: return "GL_LINE";
    case 0x1B02: return "GL_FILL";
    case 0x1C00: return "GL_RENDER";
    case 0x1C01: return "GL_FEEDBACK";
    case 0x1C02: return "GL_SELECT";
    case 0x1D00: return "GL_FLAT";
    case 0x1D01: return "GL_SMOOTH";
    case 0x1E00: return "GL_KEEP";
    case 0x1E01: return "GL_REPLACE";
    case 0x1E02: return "GL_INCR";
    case 0x1E03: return "GL_DECR";
    case 0x1F00: return "GL_VENDOR";
    case 0x1F01: return "GL_RENDERER";
    case 0x1F02: return "GL_VERSION";
    case 0x1F03: return "GL_EXTENSIONS";
    case 0x2000: return "GL_S";
    case 0x2001: return "GL_T";
    case 0x2002: return "GL_R";
    case 0x2003: return "GL_Q";
    case 0x2100: return "GL_MODULATE";
    case 0x2101: return "GL_DECAL";
    case 0x2200: return "GL_TEXTURE_ENV_MODE";
    case 0x2201: return "GL_TEXTURE_ENV_COLOR";
    case 0x2300: return "GL_TEXTURE_ENV";
    case 0x2400: return "GL_EYE_LINEAR";
    case 0x2401: return "GL_OBJECT_LINEAR";
    case 0x2402: return "GL_SPHERE_MAP";
    case 0x2500: return "GL_TEXTURE_GEN_MODE";
    case 0x2501: return "GL_OBJECT_PLANE";
    case 0x2502: return "GL_EYE_PLANE";
    case 0x2600: return "GL_NEAREST";
    case 0x2601: return "GL_LINEAR";
    case 0x2700: return "GL_NEAREST_MIPMAP_NEAREST";
    case 0x2701: return "GL_LINEAR_MIPMAP_NEAREST";
    case 0x2702: return "GL_NEAREST_MIPMAP_LINEAR";
    case 0x2703: return "GL_LINEAR_MIPMAP_LINEAR";
    case 0x2800: return "GL_TEXTURE_MAG_FILTER";
    case 0x2801: return "GL_TEXTURE_MIN_FILTER";
    case 0x2802: return "GL_TEXTURE_WRAP_S";
    case 0x2803: return "GL_TEXTURE_WRAP_T";
    case 0x2900: return "GL_CLAMP";
    case 0x2901: return "GL_REPEAT";
    case 0x2A00: return "GL_POLYGON_OFFSET_UNITS";
    case 0x2A01: return "GL_POLYGON_OFFSET_POINT";
    case 0x2A02: return "GL_POLYGON_OFFSET_LINE";
    case 0x2A10: return "GL_R3_G3_B2";
    case 0x2A20: return "GL_V2F";
    case 0x2A21: return "GL_V3F";
    case 0x2A22: return "GL_C4UB_V2F";
    case 0x2A23: return "GL_C4UB_V3F";
    case 0x2A24: return "GL_C3F_V3F";
    case 0x2A25: return "GL_N3F_V3F";
    case 0x2A26: return "GL_C4F_N3F_V3F";
    case 0x2A27: return "GL_T2F_V3F";
    case 0x2A28: return "GL_T4F_V4F";
    case 0x2A29: return "GL_T2F_C4UB_V3F";
    case 0x2A2A: return "GL_T2F_C3F_V3F";
    case 0x2A2B: return "GL_T2F_N3F_V3F";
    case 0x2A2C: return "GL_T2F_C4F_N3F_V3F";
    case 0x2A2D: return "GL_T4F_C4F_N3F_V4F";
    case 0x3000: return "GL_CLIP_PLANE0";
    case 0x3001: return "GL_CLIP_PLANE1";
    case 0x3002: return "GL_CLIP_PLANE2";
    case 0x3003: return "GL_CLIP_PLANE3";
    case 0x3004: return "GL_CLIP_PLANE4";
    case 0x3005: return "GL_CLIP_PLANE5";
    case 0x4000: return "GL_LIGHT0";
    case 0x4001: return "GL_LIGHT1";
    case 0x4002: return "GL_LIGHT2";
    case 0x4003: return "GL_LIGHT3";
    case 0x4004: return "GL_LIGHT4";
    case 0x4005: return "GL_LIGHT5";
    case 0x4006: return "GL_LIGHT6";
    case 0x4007: return "GL_LIGHT7";
    case 0x8001: return "GL_CONSTANT_COLOR";
    case 0x8002: return "GL_ONE_MINUS_CONSTANT_COLOR";
    case 0x8003: return "GL_CONSTANT_ALPHA";
    case 0x8004: return "GL_ONE_MINUS_CONSTANT_ALPHA";
    case 0x8005: return "GL_BLEND_COLOR";
    case 0x8006: return "GL_FUNC_ADD";
    case 0x8007: return "GL_MIN";
    case 0x8008: return "GL_MAX";
    case 0x8009: return "GL_BLEND_EQUATION";
    case 0x800A: return "GL_FUNC_SUBTRACT";
    case 0x800B: return "GL_FUNC_REVERSE_SUBTRACT";
    case 0x8010: return "GL_CONVOLUTION_1D";
    case 0x8011: return "GL_CONVOLUTION_2D";
    case 0x8012: return "GL_SEPARABLE_2D";
    case 0x8013: return "GL_CONVOLUTION_BORDER_MODE";
    case 0x8014: return "GL_CONVOLUTION_FILTER_SCALE";
    case 0x8015: return "GL_CONVOLUTION_FILTER_BIAS";
    case 0x8016: return "GL_REDUCE";
    case 0x8017: return "GL_CONVOLUTION_FORMAT";
    case 0x8018: return "GL_CONVOLUTION_WIDTH";
    case 0x8019: return "GL_CONVOLUTION_HEIGHT";
    case 0x801A: return "GL_MAX_CONVOLUTION_WIDTH";
    case 0x801B: return "GL_MAX_CONVOLUTION_HEIGHT";
    case 0x801C: return "GL_POST_CONVOLUTION_RED_SCALE";
    case 0x801D: return "GL_POST_CONVOLUTION_GREEN_SCALE";
    case 0x801E: return "GL_POST_CONVOLUTION_BLUE_SCALE";
    case 0x801F: return "GL_POST_CONVOLUTION_ALPHA_SCALE";
    case 0x8020: return "GL_POST_CONVOLUTION_RED_BIAS";
    case 0x8021: return "GL_POST_CONVOLUTION_GREEN_BIAS";
    case 0x8022: return "GL_POST_CONVOLUTION_BLUE_BIAS";
    case 0x8023: return "GL_POST_CONVOLUTION_ALPHA_BIAS";
    case 0x8024: return "GL_HISTOGRAM";
    case 0x8025: return "GL_PROXY_HISTOGRAM";
    case 0x8026: return "GL_HISTOGRAM_WIDTH";
    case 0x8027: return "GL_HISTOGRAM_FORMAT";
    case 0x8028: return "GL_HISTOGRAM_RED_SIZE";
    case 0x8029: return "GL_HISTOGRAM_GREEN_SIZE";
    case 0x802A: return "GL_HISTOGRAM_BLUE_SIZE";
    case 0x802B: return "GL_HISTOGRAM_ALPHA_SIZE";
    case 0x802C: return "GL_HISTOGRAM_LUMINANCE_SIZE";
    case 0x802D: return "GL_HISTOGRAM_SINK";
    case 0x802E: return "GL_MINMAX";
    case 0x802F: return "GL_MINMAX_FORMAT";
    case 0x8030: return "GL_MINMAX_SINK";
    case 0x8031: return "GL_TABLE_TOO_LARGE";
    case 0x8032: return "GL_UNSIGNED_BYTE_3_3_2";
    case 0x8033: return "GL_UNSIGNED_SHORT_4_4_4_4";
    case 0x8034: return "GL_UNSIGNED_SHORT_5_5_5_1";
    case 0x8035: return "GL_UNSIGNED_INT_8_8_8_8";
    case 0x8036: return "GL_UNSIGNED_INT_10_10_10_2";
    case 0x8037: return "GL_POLYGON_OFFSET_FILL";
    case 0x8038: return "GL_POLYGON_OFFSET_FACTOR";
    case 0x803A: return "GL_RESCALE_NORMAL";
    case 0x803B: return "GL_ALPHA4";
    case 0x803C: return "GL_ALPHA8";
    case 0x803D: return "GL_ALPHA12";
    case 0x803E: return "GL_ALPHA16";
    case 0x803F: return "GL_LUMINANCE4";
    case 0x8040: return "GL_LUMINANCE8";
    case 0x8041: return "GL_LUMINANCE12";
    case 0x8042: return "GL_LUMINANCE16";
    case 0x8043: return "GL_LUMINANCE4_ALPHA4";
    case 0x8044: return "GL_LUMINANCE6_ALPHA2";
    case 0x8045: return "GL_LUMINANCE8_ALPHA8";
    case 0x8046: return "GL_LUMINANCE12_ALPHA4";
    case 0x8047: return "GL_LUMINANCE12_ALPHA12";
    case 0x8048: return "GL_LUMINANCE16_ALPHA16";
    case 0x8049: return "GL_INTENSITY";
    case 0x804A: return "GL_INTENSITY4";
    case 0x804B: return "GL_INTENSITY8";
    case 0x804C: return "GL_INTENSITY12";
    case 0x804D: return "GL_INTENSITY16";
    case 0x804F: return "GL_RGB4";
    case 0x8050: return "GL_RGB5";
    case 0x8051: return "GL_RGB8";
    case 0x8052: return "GL_RGB10";
    case 0x8053: return "GL_RGB12";
    case 0x8054: return "GL_RGB16";
    case 0x8055: return "GL_RGBA2";
    case 0x8056: return "GL_RGBA4";
    case 0x8057: return "GL_RGB5_A1";
    case 0x8058: return "GL_RGBA8";
    case 0x8059: return "GL_RGB10_A2";
    case 0x805A: return "GL_RGBA12";
    case 0x805B: return "GL_RGBA16";
    case 0x805C: return "GL_TEXTURE_RED_SIZE";
    case 0x805D: return "GL_TEXTURE_GREEN_SIZE";
    case 0x805E: return "GL_TEXTURE_BLUE_SIZE";
    case 0x805F: return "GL_TEXTURE_ALPHA_SIZE";
    case 0x8060: return "GL_TEXTURE_LUMINANCE_SIZE";
    case 0x8061: return "GL_TEXTURE_INTENSITY_SIZE";
    case 0x8063: return "GL_PROXY_TEXTURE_1D";
    case 0x8064: return "GL_PROXY_TEXTURE_2D";
    case 0x8066: return "GL_TEXTURE_PRIORITY";
    case 0x8067: return "GL_TEXTURE_RESIDENT";
    case 0x8068: return "GL_TEXTURE_BINDING_1D";
    case 0x8069: return "GL_TEXTURE_BINDING_2D";
    case 0x806A: return "GL_TEXTURE_BINDING_3D";
    case 0x806B: return "GL_PACK_SKIP_IMAGES";
    case 0x806C: return "GL_PACK_IMAGE_HEIGHT";
    case 0x806D: return "GL_UNPACK_SKIP_IMAGES";
    case 0x806E: return "GL_UNPACK_IMAGE_HEIGHT";
    case 0x806F: return "GL_TEXTURE_3D";
    case 0x8070: return "GL_PROXY_TEXTURE_3D";
    case 0x8071: return "GL_TEXTURE_DEPTH";
    case 0x8072: return "GL_TEXTURE_WRAP_R";
    case 0x8073: return "GL_MAX_3D_TEXTURE_SIZE";
    case 0x8074: return "GL_VERTEX_ARRAY";
    case 0x8075: return "GL_NORMAL_ARRAY";
    case 0x8076: return "GL_COLOR_ARRAY";
    case 0x8077: return "GL_INDEX_ARRAY";
    case 0x8078: return "GL_TEXTURE_COORD_ARRAY";
    case 0x8079: return "GL_EDGE_FLAG_ARRAY";
    case 0x807A: return "GL_VERTEX_ARRAY_SIZE";
    case 0x807B: return "GL_VERTEX_ARRAY_TYPE";
    case 0x807C: return "GL_VERTEX_ARRAY_STRIDE";
    case 0x807E: return "GL_NORMAL_ARRAY_TYPE";
    case 0x807F: return "GL_NORMAL_ARRAY_STRIDE";
    case 0x8081: return "GL_COLOR_ARRAY_SIZE";
    case 0x8082: return "GL_COLOR_ARRAY_TYPE";
    case 0x8083: return "GL_COLOR_ARRAY_STRIDE";
    case 0x8085: return "GL_INDEX_ARRAY_TYPE";
    case 0x8086: return "GL_INDEX_ARRAY_STRIDE";
    case 0x8088: return "GL_TEXTURE_COORD_ARRAY_SIZE";
    case 0x8089: return "GL_TEXTURE_COORD_ARRAY_TYPE";
    case 0x808A: return "GL_TEXTURE_COORD_ARRAY_STRIDE";
    case 0x808C: return "GL_EDGE_FLAG_ARRAY_STRIDE";
    case 0x808E: return "GL_VERTEX_ARRAY_POINTER";
    case 0x808F: return "GL_NORMAL_ARRAY_POINTER";
    case 0x8090: return "GL_COLOR_ARRAY_POINTER";
    case 0x8091: return "GL_INDEX_ARRAY_POINTER";
    case 0x8092: return "GL_TEXTURE_COORD_ARRAY_POINTER";
    case 0x8093: return "GL_EDGE_FLAG_ARRAY_POINTER";
    case 0x809D: return "GL_MULTISAMPLE";
    case 0x809E: return "GL_SAMPLE_ALPHA_TO_COVERAGE";
    case 0x809F: return "GL_SAMPLE_ALPHA_TO_ONE";
    case 0x80A0: return "GL_SAMPLE_COVERAGE";
    case 0x80A8: return "GL_SAMPLE_BUFFERS";
    case 0x80A9: return "GL_SAMPLES";
    case 0x80AA: return "GL_SAMPLE_COVERAGE_VALUE";
    case 0x80AB: return "GL_SAMPLE_COVERAGE_INVERT";
    case 0x80B1: return "GL_COLOR_MATRIX";
    case 0x80B2: return "GL_COLOR_MATRIX_STACK_DEPTH";
    case 0x80B3: return "GL_MAX_COLOR_MATRIX_STACK_DEPTH";
    case 0x80B4: return "GL_POST_COLOR_MATRIX_RED_SCALE";
    case 0x80B5: return "GL_POST_COLOR_MATRIX_GREEN_SCALE";
    case 0x80B6: return "GL_POST_COLOR_MATRIX_BLUE_SCALE";
    case 0x80B7: return "GL_POST_COLOR_MATRIX_ALPHA_SCALE";
    case 0x80B8: return "GL_POST_COLOR_MATRIX_RED_BIAS";
    case 0x80B9: return "GL_POST_COLOR_MATRIX_GREEN_BIAS";
    case 0x80BA: return "GL_POST_COLOR_MATRIX_BLUE_BIAS";
    case 0x80BB: return "GL_POST_COLOR_MATRIX_ALPHA_BIAS";
    case 0x80D0: return "GL_COLOR_TABLE";
    case 0x80D1: return "GL_POST_CONVOLUTION_COLOR_TABLE";
    case 0x80D2: return "GL_POST_COLOR_MATRIX_COLOR_TABLE";
    case 0x80D3: return "GL_PROXY_COLOR_TABLE";
    case 0x80D4: return "GL_PROXY_POST_CONVOLUTION_COLOR_TABLE";
    case 0x80D5: return "GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE";
    case 0x80D6: return "GL_COLOR_TABLE_SCALE";
    case 0x80D7: return "GL_COLOR_TABLE_BIAS";
    case 0x80D8: return "GL_COLOR_TABLE_FORMAT";
    case 0x80D9: return "GL_COLOR_TABLE_WIDTH";
    case 0x80DA: return "GL_COLOR_TABLE_RED_SIZE";
    case 0x80DB: return "GL_COLOR_TABLE_GREEN_SIZE";
    case 0x80DC: return "GL_COLOR_TABLE_BLUE_SIZE";
    case 0x80DD: return "GL_COLOR_TABLE_ALPHA_SIZE";
    case 0x80DE: return "GL_COLOR_TABLE_LUMINANCE_SIZE";
    case 0x80DF: return "GL_COLOR_TABLE_INTENSITY_SIZE";
    case 0x80E0: return "GL_BGR";
    case 0x80E1: return "GL_BGRA";
    case 0x80E8: return "GL_MAX_ELEMENTS_VERTICES";
    case 0x80E9: return "GL_MAX_ELEMENTS_INDICES";
    case 0x812D: return "GL_CLAMP_TO_BORDER";
    case 0x812F: return "GL_CLAMP_TO_EDGE";
    case 0x813A: return "GL_TEXTURE_MIN_LOD";
    case 0x813B: return "GL_TEXTURE_MAX_LOD";
    case 0x813C: return "GL_TEXTURE_BASE_LEVEL";
    case 0x813D: return "GL_TEXTURE_MAX_LEVEL";
    case 0x8151: return "GL_CONSTANT_BORDER";
    case 0x8153: return "GL_REPLICATE_BORDER";
    case 0x8154: return "GL_CONVOLUTION_BORDER_COLOR";
    case 0x81F8: return "GL_LIGHT_MODEL_COLOR_CONTROL";
    case 0x81F9: return "GL_SINGLE_COLOR";
    case 0x81FA: return "GL_SEPARATE_SPECULAR_COLOR";
    case 0x8362: return "GL_UNSIGNED_BYTE_2_3_3_REV";
    case 0x8363: return "GL_UNSIGNED_SHORT_5_6_5";
    case 0x8364: return "GL_UNSIGNED_SHORT_5_6_5_REV";
    case 0x8365: return "GL_UNSIGNED_SHORT_4_4_4_4_REV";
    case 0x8366: return "GL_UNSIGNED_SHORT_1_5_5_5_REV";
    case 0x8367: return "GL_UNSIGNED_INT_8_8_8_8_REV";
    case 0x8368: return "GL_UNSIGNED_INT_2_10_10_10_REV";
    case 0x846D: return "GL_ALIASED_POINT_SIZE_RANGE";
    case 0x846E: return "GL_ALIASED_LINE_WIDTH_RANGE";
    case 0x84C0: return "GL_TEXTURE0";
    case 0x84C1: return "GL_TEXTURE1";
    case 0x84C2: return "GL_TEXTURE2";
    case 0x84C3: return "GL_TEXTURE3";
    case 0x84C4: return "GL_TEXTURE4";
    case 0x84C5: return "GL_TEXTURE5";
    case 0x84C6: return "GL_TEXTURE6";
    case 0x84C7: return "GL_TEXTURE7";
    case 0x84C8: return "GL_TEXTURE8";
    case 0x84C9: return "GL_TEXTURE9";
    case 0x84CA: return "GL_TEXTURE10";
    case 0x84CB: return "GL_TEXTURE11";
    case 0x84CC: return "GL_TEXTURE12";
    case 0x84CD: return "GL_TEXTURE13";
    case 0x84CE: return "GL_TEXTURE14";
    case 0x84CF: return "GL_TEXTURE15";
    case 0x84D0: return "GL_TEXTURE16";
    case 0x84D1: return "GL_TEXTURE17";
    case 0x84D2: return "GL_TEXTURE18";
    case 0x84D3: return "GL_TEXTURE19";
    case 0x84D4: return "GL_TEXTURE20";
    case 0x84D5: return "GL_TEXTURE21";
    case 0x84D6: return "GL_TEXTURE22";
    case 0x84D7: return "GL_TEXTURE23";
    case 0x84D8: return "GL_TEXTURE24";
    case 0x84D9: return "GL_TEXTURE25";
    case 0x84DA: return "GL_TEXTURE26";
    case 0x84DB: return "GL_TEXTURE27";
    case 0x84DC: return "GL_TEXTURE28";
    case 0x84DD: return "GL_TEXTURE29";
    case 0x84DE: return "GL_TEXTURE30";
    case 0x84DF: return "GL_TEXTURE31";
    case 0x84E0: return "GL_ACTIVE_TEXTURE";
    case 0x84E1: return "GL_CLIENT_ACTIVE_TEXTURE";
    case 0x84E2: return "GL_MAX_TEXTURE_UNITS";
    case 0x84E3: return "GL_TRANSPOSE_MODELVIEW_MATRIX";
    case 0x84E4: return "GL_TRANSPOSE_PROJECTION_MATRIX";
    case 0x84E5: return "GL_TRANSPOSE_TEXTURE_MATRIX";
    case 0x84E6: return "GL_TRANSPOSE_COLOR_MATRIX";
    case 0x84E7: return "GL_SUBTRACT";
    case 0x84E9: return "GL_COMPRESSED_ALPHA";
    case 0x84EA: return "GL_COMPRESSED_LUMINANCE";
    case 0x84EB: return "GL_COMPRESSED_LUMINANCE_ALPHA";
    case 0x84EC: return "GL_COMPRESSED_INTENSITY";
    case 0x84ED: return "GL_COMPRESSED_RGB";
    case 0x84EE: return "GL_COMPRESSED_RGBA";
    case 0x84EF: return "GL_TEXTURE_COMPRESSION_HINT";
    case 0x8511: return "GL_NORMAL_MAP";
    case 0x8512: return "GL_REFLECTION_MAP";
    case 0x8513: return "GL_TEXTURE_CUBE_MAP";
    case 0x8514: return "GL_TEXTURE_BINDING_CUBE_MAP";
    case 0x8515: return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
    case 0x8516: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
    case 0x8517: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
    case 0x8518: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
    case 0x8519: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
    case 0x851A: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
    case 0x851B: return "GL_PROXY_TEXTURE_CUBE_MAP";
    case 0x851C: return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
    case 0x8570: return "GL_COMBINE";
    case 0x8571: return "GL_COMBINE_RGB";
    case 0x8572: return "GL_COMBINE_ALPHA";
    case 0x8573: return "GL_RGB_SCALE";
    case 0x8574: return "GL_ADD_SIGNED";
    case 0x8575: return "GL_INTERPOLATE";
    case 0x8576: return "GL_CONSTANT";
    case 0x8577: return "GL_PRIMARY_COLOR";
    case 0x8578: return "GL_PREVIOUS";
    case 0x8580: return "GL_SOURCE0_RGB";
    case 0x8581: return "GL_SOURCE1_RGB";
    case 0x8582: return "GL_SOURCE2_RGB";
    case 0x8588: return "GL_SOURCE0_ALPHA";
    case 0x8589: return "GL_SOURCE1_ALPHA";
    case 0x858A: return "GL_SOURCE2_ALPHA";
    case 0x8590: return "GL_OPERAND0_RGB";
    case 0x8591: return "GL_OPERAND1_RGB";
    case 0x8592: return "GL_OPERAND2_RGB";
    case 0x8598: return "GL_OPERAND0_ALPHA";
    case 0x8599: return "GL_OPERAND1_ALPHA";
    case 0x859A: return "GL_OPERAND2_ALPHA";
    case 0x86A0: return "GL_TEXTURE_COMPRESSED_IMAGE_SIZE";
    case 0x86A1: return "GL_TEXTURE_COMPRESSED";
    case 0x86A2: return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
    case 0x86A3: return "GL_COMPRESSED_TEXTURE_FORMATS";
    case 0x86AE: return "GL_DOT3_RGB";
    case 0x86AF: return "GL_DOT3_RGBA";
    case 0x8750: return "GL_DEPTH_STENCIL_MESA";
    case 0x8751: return "GL_UNSIGNED_INT_24_8_MESA";
    case 0x8752: return "GL_UNSIGNED_INT_8_24_REV_MESA";
    case 0x8753: return "GL_UNSIGNED_SHORT_15_1_MESA";
    case 0x8754: return "GL_UNSIGNED_SHORT_1_15_REV_MESA";
    case 0x8622: return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
    case 0x8623: return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
    case 0x8624: return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
    case 0x8625: return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
    case 0x8626: return "GL_CURRENT_VERTEX_ATTRIB";
    case 0x8642: return "GL_VERTEX_PROGRAM_POINT_SIZE";
    case 0x8645: return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
    case 0x8800: return "GL_STENCIL_BACK_FUNC";
    case 0x8801: return "GL_STENCIL_BACK_FAIL";
    case 0x8802: return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
    case 0x8803: return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
    case 0x8824: return "GL_MAX_DRAW_BUFFERS";
    case 0x8825: return "GL_DRAW_BUFFER0";
    case 0x8826: return "GL_DRAW_BUFFER1";
    case 0x8827: return "GL_DRAW_BUFFER2";
    case 0x8828: return "GL_DRAW_BUFFER3";
    case 0x8829: return "GL_DRAW_BUFFER4";
    case 0x882A: return "GL_DRAW_BUFFER5";
    case 0x882B: return "GL_DRAW_BUFFER6";
    case 0x882C: return "GL_DRAW_BUFFER7";
    case 0x882D: return "GL_DRAW_BUFFER8";
    case 0x882E: return "GL_DRAW_BUFFER9";
    case 0x882F: return "GL_DRAW_BUFFER10";
    case 0x8830: return "GL_DRAW_BUFFER11";
    case 0x8831: return "GL_DRAW_BUFFER12";
    case 0x8832: return "GL_DRAW_BUFFER13";
    case 0x8833: return "GL_DRAW_BUFFER14";
    case 0x8834: return "GL_DRAW_BUFFER15";
    case 0x883D: return "GL_BLEND_EQUATION_ALPHA";
    case 0x8869: return "GL_MAX_VERTEX_ATTRIBS";
    case 0x886A: return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
    case 0x8872: return "GL_MAX_TEXTURE_IMAGE_UNITS";
    case 0x8B30: return "GL_FRAGMENT_SHADER";
    case 0x8B31: return "GL_VERTEX_SHADER";
    case 0x8B49: return "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS";
    case 0x8B4A: return "GL_MAX_VERTEX_UNIFORM_COMPONENTS";
    case 0x8B4B: return "GL_MAX_VARYING_FLOATS";
    case 0x8B4C: return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
    case 0x8B4D: return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
    case 0x8B4F: return "GL_SHADER_TYPE";
    case 0x8B50: return "GL_FLOAT_VEC2";
    case 0x8B51: return "GL_FLOAT_VEC3";
    case 0x8B52: return "GL_FLOAT_VEC4";
    case 0x8B53: return "GL_INT_VEC2";
    case 0x8B54: return "GL_INT_VEC3";
    case 0x8B55: return "GL_INT_VEC4";
    case 0x8B56: return "GL_BOOL";
    case 0x8B57: return "GL_BOOL_VEC2";
    case 0x8B58: return "GL_BOOL_VEC3";
    case 0x8B59: return "GL_BOOL_VEC4";
    case 0x8B5A: return "GL_FLOAT_MAT2";
    case 0x8B5B: return "GL_FLOAT_MAT3";
    case 0x8B5C: return "GL_FLOAT_MAT4";
    case 0x8B5D: return "GL_SAMPLER_1D";
    case 0x8B5E: return "GL_SAMPLER_2D";
    case 0x8B5F: return "GL_SAMPLER_3D";
    case 0x8B60: return "GL_SAMPLER_CUBE";
    case 0x8B61: return "GL_SAMPLER_1D_SHADOW";
    case 0x8B62: return "GL_SAMPLER_2D_SHADOW";
    case 0x8B80: return "GL_DELETE_STATUS";
    case 0x8B81: return "GL_COMPILE_STATUS";
    case 0x8B82: return "GL_LINK_STATUS";
    case 0x8B83: return "GL_VALIDATE_STATUS";
    case 0x8B84: return "GL_INFO_LOG_LENGTH";
    case 0x8B85: return "GL_ATTACHED_SHADERS";
    case 0x8B86: return "GL_ACTIVE_UNIFORMS";
    case 0x8B87: return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
    case 0x8B88: return "GL_SHADER_SOURCE_LENGTH";
    case 0x8B89: return "GL_ACTIVE_ATTRIBUTES";
    case 0x8B8A: return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
    case 0x8B8B: return "GL_FRAGMENT_SHADER_DERIVATIVE_HINT";
    case 0x8B8C: return "GL_SHADING_LANGUAGE_VERSION";
    case 0x8B8D: return "GL_CURRENT_PROGRAM";
    case 0x8CA0: return "GL_POINT_SPRITE_COORD_ORIGIN";
    case 0x8CA1: return "GL_LOWER_LEFT";
    case 0x8CA2: return "GL_UPPER_LEFT";
    case 0x8CA3: return "GL_STENCIL_BACK_REF";
    case 0x8CA4: return "GL_STENCIL_BACK_VALUE_MASK";
    case 0x8CA5: return "GL_STENCIL_BACK_WRITEMASK";
    case 0x8643: return "GL_VERTEX_PROGRAM_TWO_SIDE";
    case 0x8861: return "GL_POINT_SPRITE";
    case 0x8862: return "GL_COORD_REPLACE";
    case 0x8871: return "GL_MAX_TEXTURE_COORDS";
    case 0x8764: return "GL_BUFFER_SIZE";
    case 0x8765: return "GL_BUFFER_USAGE";
    case 0x8864: return "GL_QUERY_COUNTER_BITS";
    case 0x8865: return "GL_CURRENT_QUERY";
    case 0x8866: return "GL_QUERY_RESULT";
    case 0x8867: return "GL_QUERY_RESULT_AVAILABLE";
    case 0x8892: return "GL_ARRAY_BUFFER";
    case 0x8893: return "GL_ELEMENT_ARRAY_BUFFER";
    case 0x8894: return "GL_ARRAY_BUFFER_BINDING";
    case 0x8895: return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
    case 0x889F: return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
    case 0x88B8: return "GL_READ_ONLY";
    case 0x88B9: return "GL_WRITE_ONLY";
    case 0x88BA: return "GL_READ_WRITE";
    case 0x88BB: return "GL_BUFFER_ACCESS";
    case 0x88BC: return "GL_BUFFER_MAPPED";
    case 0x88BD: return "GL_BUFFER_MAP_POINTER";
    case 0x88E0: return "GL_STREAM_DRAW";
    case 0x88E1: return "GL_STREAM_READ";
    case 0x88E2: return "GL_STREAM_COPY";
    case 0x88E4: return "GL_STATIC_DRAW";
    case 0x88E5: return "GL_STATIC_READ";
    case 0x88E6: return "GL_STATIC_COPY";
    case 0x88E8: return "GL_DYNAMIC_DRAW";
    case 0x88E9: return "GL_DYNAMIC_READ";
    case 0x88EA: return "GL_DYNAMIC_COPY";
    case 0x8914: return "GL_SAMPLES_PASSED";
    case 0x8896: return "GL_VERTEX_ARRAY_BUFFER_BINDING";
    case 0x8897: return "GL_NORMAL_ARRAY_BUFFER_BINDING";
    case 0x8898: return "GL_COLOR_ARRAY_BUFFER_BINDING";
    case 0x8899: return "GL_INDEX_ARRAY_BUFFER_BINDING";
    case 0x889A: return "GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING";
    case 0x889B: return "GL_EDGE_FLAG_ARRAY_BUFFER_BINDING";
    case 0x889C: return "GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING";
    case 0x889D: return "GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING";
    case 0x889E: return "GL_WEIGHT_ARRAY_BUFFER_BINDING";
    case 0x8450: return "GL_FOG_COORD_SRC";
    case 0x8451: return "GL_FOG_COORD";
    case 0x8453: return "GL_CURRENT_FOG_COORD";
    case 0x8454: return "GL_FOG_COORD_ARRAY_TYPE";
    case 0x8455: return "GL_FOG_COORD_ARRAY_STRIDE";
    case 0x8456: return "GL_FOG_COORD_ARRAY_POINTER";
    case 0x8457: return "GL_FOG_COORD_ARRAY";

    default:
        return "UNKNOWN_GL_ENUM";
    }
}

const char *
reverse_clipboard_type(PP_Flash_Clipboard_Type ct)
{
    switch (ct) {
    CASE(PP_FLASH_CLIPBOARD_TYPE_STANDARD);
    CASE(PP_FLASH_CLIPBOARD_TYPE_SELECTION);
    default:
        return "UNKNOWNCLIPBOARDTYPE";
    }
}

const char *
reverse_clipboard_format(uint32_t cf)
{
    switch (cf) {
    CASE(PP_FLASH_CLIPBOARD_FORMAT_INVALID);
    CASE(PP_FLASH_CLIPBOARD_FORMAT_PLAINTEXT);
    CASE(PP_FLASH_CLIPBOARD_FORMAT_HTML);
    CASE(PP_FLASH_CLIPBOARD_FORMAT_RTF);
    default:
        return "UNKNOWNCLIPBOARDFORMAT";
    }
}

const char *
reverse_video_decoder_profile(PP_VideoDecoder_Profile profile)
{
    switch (profile) {
    CASE(PP_VIDEODECODER_PROFILE_UNKNOWN);
    CASE(PP_VIDEODECODER_H264PROFILE_NONE);
    CASE(PP_VIDEODECODER_H264PROFILE_BASELINE);
    CASE(PP_VIDEODECODER_H264PROFILE_MAIN);
    CASE(PP_VIDEODECODER_H264PROFILE_EXTENDED);
    CASE(PP_VIDEODECODER_H264PROFILE_HIGH);
    CASE(PP_VIDEODECODER_H264PROFILE_HIGH10PROFILE);
    CASE(PP_VIDEODECODER_H264PROFILE_HIGH422PROFILE);
    CASE(PP_VIDEODECODER_H264PROFILE_HIGH444PREDICTIVEPROFILE);
    CASE(PP_VIDEODECODER_H264PROFILE_SCALABLEBASELINE);
    CASE(PP_VIDEODECODER_H264PROFILE_SCALABLEHIGH);
    CASE(PP_VIDEODECODER_H264PROFILE_STEREOHIGH);
    CASE(PP_VIDEODECODER_H264PROFILE_MULTIVIEWHIGH);
    CASE(PP_VIDEODECODER_VP8PROFILE_ANY);
    default:
        return "UNKNOWNVIDEODECODERPROFILE";
    }
}

const char *
reverse_file_chooser_mode(PP_FileChooserMode_Dev mode)
{
    switch (mode) {
    CASE(PP_FILECHOOSERMODE_OPEN);
    CASE(PP_FILECHOOSERMODE_OPENMULTIPLE);
    default:
        return "UNKNOWNFILECHOOSERMODE";
    }
}

const char *
reverse_udp_socket_feature(PP_UDPSocketFeature_Private feature)
{
    switch (feature) {
    CASE(PP_UDPSOCKETFEATURE_PRIVATE_ADDRESS_REUSE);
    CASE(PP_UDPSOCKETFEATURE_PRIVATE_BROADCAST);
    default:
        return "UNKNOWNUDPSOCKETFEATURE";
    }
}

const char *
reverse_x509_certificate_field(PP_X509Certificate_Private_Field field)
{
    switch (field) {
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_COMMON_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_LOCALITY_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_STATE_OR_PROVINCE_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_COUNTRY_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_ORGANIZATION_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_ORGANIZATION_UNIT_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_UNIQUE_ID);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_COMMON_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_LOCALITY_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_STATE_OR_PROVINCE_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_COUNTRY_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_ORGANIZATION_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_ORGANIZATION_UNIT_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_UNIQUE_ID);
    CASE(PP_X509CERTIFICATE_PRIVATE_VERSION);
    CASE(PP_X509CERTIFICATE_PRIVATE_SERIAL_NUMBER);
    CASE(PP_X509CERTIFICATE_PRIVATE_SIGNATURE_ALGORITHM_OID);
    CASE(PP_X509CERTIFICATE_PRIVATE_SIGNATURE_ALGORITHM_PARAMATERS_RAW);
    CASE(PP_X509CERTIFICATE_PRIVATE_VALIDITY_NOT_BEFORE);
    CASE(PP_X509CERTIFICATE_PRIVATE_VALIDITY_NOT_AFTER);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_PUBLIC_KEY_ALGORITHM_OID);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_PUBLIC_KEY);
    CASE(PP_X509CERTIFICATE_PRIVATE_RAW);
    CASE(PP_X509CERTIFICATE_PRIVATE_ISSUER_DISTINGUISHED_NAME);
    CASE(PP_X509CERTIFICATE_PRIVATE_SUBJECT_DISTINGUISHED_NAME);
    default:
        return "UNKNOWNX509CERTIFICATEFIELD";
    }
}

const char *
reverse_resource_image(PP_ResourceImage resource_image)
{
    switch (resource_image) {
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTP);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTP_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTP_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTW);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTW_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_FTW_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMOUT_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_0);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_1);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_2);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_3);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_4);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_5);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_6);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_7);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_8);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_9);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_THUMBNAIL_NUM_BACKGROUND);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_0);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_1);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_2);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_3);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_4);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_5);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_6);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_7);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_8);
    CASE(PP_RESOURCEIMAGE_PDF_PROGRESS_BAR_BACKGROUND);
    CASE(PP_RESOURCEIMAGE_PDF_PAGE_DROPSHADOW);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_SAVE);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_SAVE_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_PRINT);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_HOVER);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_ZOOMIN_END_PRESSED);
    CASE(PP_RESOURCEIMAGE_PDF_PAN_SCROLL_ICON);
    CASE(PP_RESOURCEIMAGE_PDF_PAGE_INDICATOR_BACKGROUND);
    CASE(PP_RESOURCEIMAGE_PDF_BUTTON_PRINT_DISABLED);
    default:
        return "UNKNOWNRESOURCEIMAGE";
    }
}

const char *
reverse_pdf_feature(PP_PDFFeature feature)
{
    switch (feature) {
    CASE(PP_PDFFEATURE_HIDPI);
    CASE(PP_PDFFEATURE_PRINTING);
    default:
        return "UNKNOWNPDFFEATURE";
    }
}

const char *
reverse_resource_string(PP_ResourceString resource_string)
{
    switch (resource_string) {
    CASE(PP_RESOURCESTRING_PDFGETPASSWORD);
    CASE(PP_RESOURCESTRING_PDFLOADING);
    CASE(PP_RESOURCESTRING_PDFLOAD_FAILED);
    CASE(PP_RESOURCESTRING_PDFPROGRESSLOADING);
    default:
        return "UNKNOWNRESOURCESTRING";
    }
}

const char *
reverse_private_font_charset(PP_PrivateFontCharset charset)
{
    switch (charset) {
    CASE(PP_PRIVATEFONTCHARSET_ANSI);
    CASE(PP_PRIVATEFONTCHARSET_DEFAULT);
    CASE(PP_PRIVATEFONTCHARSET_SYMBOL);
    CASE(PP_PRIVATEFONTCHARSET_MAC);
    CASE(PP_PRIVATEFONTCHARSET_SHIFTJIS);
    CASE(PP_PRIVATEFONTCHARSET_HANGUL);
    CASE(PP_PRIVATEFONTCHARSET_JOHAB);
    CASE(PP_PRIVATEFONTCHARSET_GB2312);
    CASE(PP_PRIVATEFONTCHARSET_CHINESEBIG5);
    CASE(PP_PRIVATEFONTCHARSET_GREEK);
    CASE(PP_PRIVATEFONTCHARSET_TURKISH);
    CASE(PP_PRIVATEFONTCHARSET_VIETNAMESE);
    CASE(PP_PRIVATEFONTCHARSET_HEBREW);
    CASE(PP_PRIVATEFONTCHARSET_ARABIC);
    CASE(PP_PRIVATEFONTCHARSET_BALTIC);
    CASE(PP_PRIVATEFONTCHARSET_RUSSIAN);
    CASE(PP_PRIVATEFONTCHARSET_THAI);
    CASE(PP_PRIVATEFONTCHARSET_EASTEUROPE);
    CASE(PP_PRIVATEFONTCHARSET_OEM);
    default:
        return "UNKNOWNPRIVATEFONTCHARSET";
    }
}

const char *
reverse_scrollby(PP_ScrollBy_Dev unit)
{
    switch (unit) {
    CASE(PP_SCROLLBY_PIXEL);
    CASE(PP_SCROLLBY_LINE);
    CASE(PP_SCROLLBY_PAGE);
    CASE(PP_SCROLLBY_DOCUMENT);
    default:
        return "UNKNONWSCROLLBY";
    }
}

const char *
reverse_graphics3d_attribute(int32_t attr)
{
    switch (attr) {
    CASE(PP_GRAPHICS3DATTRIB_ALPHA_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_BLUE_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_GREEN_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_RED_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_DEPTH_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_STENCIL_SIZE);
    CASE(PP_GRAPHICS3DATTRIB_SAMPLES);
    CASE(PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS);
    CASE(PP_GRAPHICS3DATTRIB_NONE);
    CASE(PP_GRAPHICS3DATTRIB_HEIGHT);
    CASE(PP_GRAPHICS3DATTRIB_WIDTH);
    CASE(PP_GRAPHICS3DATTRIB_SWAP_BEHAVIOR);
    CASE(PP_GRAPHICS3DATTRIB_GPU_PREFERENCE);
    default:
        return "UNKNOWNATTRIBUTE";
    }
}
