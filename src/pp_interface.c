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

#include <string.h>
#include "pp_interface.h"
#include "trace.h"
#include <ppapi/c/dev/ppb_audio_input_dev.h>
#include <ppapi/c/dev/ppb_buffer_dev.h>
#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <ppapi/c/dev/ppb_crypto_dev.h>
#include <ppapi/c/dev/ppb_cursor_control_dev.h>
#include <ppapi/c/dev/ppb_device_ref_dev.h>
#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <ppapi/c/dev/ppb_font_dev.h>
#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <ppapi/c/dev/ppb_memory_dev.h>
#include <ppapi/c/dev/ppb_printing_dev.h>
#include <ppapi/c/dev/ppb_scrollbar_dev.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <ppapi/c/dev/ppb_var_deprecated.h>
#include <ppapi/c/dev/ppb_video_capture_dev.h>
#include <ppapi/c/dev/ppb_video_decoder_dev.h>
#include <ppapi/c/dev/ppb_view_dev.h>
#include <ppapi/c/dev/ppb_widget_dev.h>
#include <ppapi/c/dev/ppb_zoom_dev.h>
#include <ppapi/c/ppb_audio_config.h>
#include <ppapi/c/ppb_audio.h>
#include <ppapi/c/ppb_core.h>
#include <ppapi/c/ppb_file_io.h>
#include <ppapi/c/ppb_file_ref.h>
#include <ppapi/c/ppb_graphics_2d.h>
#include <ppapi/c/ppb_graphics_3d.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/ppb_image_data.h>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/ppb_messaging.h>
#include <ppapi/c/ppb_network_monitor.h>
#include <ppapi/c/ppb_opengles2.h>
#include <ppapi/c/ppb_text_input_controller.h>
#include <ppapi/c/ppb_url_loader.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_var_array.h>
#include <ppapi/c/ppb_var_array_buffer.h>
#include <ppapi/c/ppb_var_dictionary.h>
#include <ppapi/c/ppb_view.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/private/ppb_file_io_private.h>
#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <ppapi/c/private/ppb_flash_drm.h>
#include <ppapi/c/private/ppb_flash_file.h>
#include <ppapi/c/private/ppb_flash_font_file.h>
#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <ppapi/c/private/ppb_flash.h>
#include <ppapi/c/private/ppb_flash_menu.h>
#include <ppapi/c/private/ppb_flash_message_loop.h>
#include <ppapi/c/private/ppb_host_resolver_private.h>
#include <ppapi/c/private/ppb_instance_private.h>
#include <ppapi/c/private/ppb_pdf.h>
#include <ppapi/c/private/ppb_tcp_socket_private.h>
#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <ppapi/c/private/ppb_uma_private.h>
#include <ppapi/c/private/ppb_x509_certificate_private.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <ppapi/c/trusted/ppb_file_chooser_trusted.h>
#include <ppapi/c/trusted/ppb_url_loader_trusted.h>


extern const struct PPB_Core_1_0                    ppb_core_interface_1_0;
extern const struct PPB_AudioConfig_1_1             ppb_audio_config_interface_1_1;
extern const struct PPB_AudioInput_Dev_0_3          ppb_audio_input_dev_interface_0_3;
extern const struct PPB_AudioInput_Dev_0_4          ppb_audio_input_dev_interface_0_4;
extern const struct PPB_Audio_1_0                   ppb_audio_interface_1_0;
extern const struct PPB_Audio_1_1                   ppb_audio_interface_1_1;
extern const struct PPB_BrowserFont_Trusted_1_0     ppb_browser_font_trusted_interface_1_0;
extern const struct PPB_Buffer_Dev_0_4              ppb_buffer_dev_interface_0_4;
extern const struct PPB_CharSet_Dev_0_4             ppb_char_set_dev_interface_0_4;
extern const struct PPB_Crypto_Dev_0_1              ppb_crypto_dev_interface_0_1;
extern const struct PPB_CursorControl_Dev_0_4       ppb_cursor_control_dev_interface_0_4;
extern const struct PPB_DeviceRef_Dev_0_1           ppb_device_ref_dev_interface_0_1;
extern const struct PPB_FileChooser_Dev_0_6         ppb_file_chooser_dev_interface_0_6;
extern const struct PPB_FileChooserTrusted_0_6      ppb_file_chooser_trusted_interface_0_6;
extern const struct PPB_FileIO_1_0                  ppb_file_io_interface_1_0;
extern const struct PPB_FileIO_1_1                  ppb_file_io_interface_1_1;
extern const struct PPB_FileIO_Private_0_1          ppb_file_io_private_interface_0_1;
extern const struct PPB_FileRef_1_0                 ppb_file_ref_interface_1_0;
extern const struct PPB_FileRef_1_1                 ppb_file_ref_interface_1_1;
extern const struct PPB_Flash_Clipboard_5_0         ppb_flash_clipboard_interface_5_0;
extern const struct PPB_Flash_Clipboard_5_1         ppb_flash_clipboard_interface_5_1;
extern const struct PPB_Flash_DRM_1_0               ppb_flash_drm_interface_1_0;
extern const struct PPB_Flash_DRM_1_1               ppb_flash_drm_interface_1_1;
extern const struct PPB_Flash_File_FileRef          ppb_flash_file_file_ref_interface_2_0;
extern const struct PPB_Flash_File_ModuleLocal_3_0  ppb_flash_file_modulelocal_interface_3_0;
extern const struct PPB_Flash_FontFile_0_1          ppb_flash_font_file_interface_0_1;
extern const struct PPB_FlashFullscreen_1_0         ppb_flash_fullscreen_interface_1_0;
extern const struct PPB_Flash_12_6                  ppb_flash_interface_12_6;
extern const struct PPB_Flash_13_0                  ppb_flash_interface_13_0;
extern const struct PPB_Flash_Menu_0_2              ppb_flash_menu_interface_0_2;
extern const struct PPB_Flash_MessageLoop_0_1       ppb_flash_message_loop_interface_0_1;
extern const struct PPB_Font_Dev_0_6                ppb_font_dev_interface_0_6;
extern const struct PPB_Graphics2D_1_0              ppb_graphics2d_interface_1_0;
extern const struct PPB_Graphics2D_1_1              ppb_graphics2d_interface_1_1;
extern const struct PPB_Graphics3D_1_0              ppb_graphics3d_interface_1_0;
extern const struct PPB_HostResolver_Private_0_1    ppb_host_resolver_private_interface_0_1;
extern const struct PPB_ImageData_1_0               ppb_image_data_interface_1_0;
extern const struct PPB_IMEInputEvent_1_0           ppb_ime_input_event_interface_1_0;
extern const struct PPB_IMEInputEvent_Dev_0_2       ppb_ime_input_event_dev_interface_0_2;
extern const struct PPB_InputEvent_1_0              ppb_input_event_interface_1_0;
extern const struct PPB_Instance_1_0                ppb_instance_interface_1_0;
extern const struct PPB_Instance_Private_0_1        ppb_instance_private_interface_0_1;
extern const struct PPB_KeyboardInputEvent_1_0      ppb_keyboard_input_event_interface_1_0;
extern const struct PPB_KeyboardInputEvent_1_2      ppb_keyboard_input_event_interface_1_2;
extern const struct PPB_Memory_Dev_0_1              ppb_memory_dev_interface_0_1;
extern const struct PPB_Messaging_1_0               ppb_messaging_interface_1_0;
extern const struct PPB_Messaging_1_2               ppb_messaging_interface_1_2;
extern const struct PPB_MouseInputEvent_1_1         ppb_mouse_input_event_interface_1_1;
extern const struct PPB_NetAddress_Private_1_1      ppb_net_address_private_interface_1_1;
extern const struct PPB_NetworkMonitor_1_0          ppb_network_monitor_interface_1_0;
extern const struct PPB_OpenGLES2ChromiumEnableFeature ppb_opengles2_chromium_enable_feature_interface_1_0;
extern const struct PPB_OpenGLES2ChromiumMapSub     ppb_opengles2_chromium_map_sub_interface_1_0;
extern const struct PPB_OpenGLES2FramebufferBlit    ppb_opengles2_framebuffer_blit_interface_1_0;
extern const struct PPB_OpenGLES2FramebufferMultisample ppb_opengles2_framebuffer_multisample_interface_1_0;
extern const struct PPB_OpenGLES2InstancedArrays    ppb_opengles2_instanced_arrays_interface_1_0;
extern const struct PPB_OpenGLES2Query              ppb_opengles2_query_interface_1_0;
extern const struct PPB_OpenGLES2                   ppb_opengles2_interface_1_0;
extern const struct PPB_PDF                         ppb_pdf_interface;
extern const struct PPB_Printing_Dev_0_7            ppb_printing_dev_interface_0_7;
extern const struct PPB_Scrollbar_Dev_0_5           ppb_scrollbar_dev_interface_0_5;
extern const struct PPB_TCPSocket_Private_0_4       ppb_tcp_socket_private_interface_0_4;
extern const struct PPB_TCPSocket_Private_0_5       ppb_tcp_socket_private_interface_0_5;
extern const struct PPB_TextInputController_1_0     ppb_text_input_controller_interface_1_0;
extern const struct PPB_TextInput_Dev_0_2           ppb_text_input_dev_interface_0_2;
extern const struct PPB_TouchInputEvent_1_0         ppb_touch_input_event_interface_1_0;
extern const struct PPB_UDPSocket_Private_0_4       ppb_udp_socket_private_interface_0_4;
extern const struct PPB_URLLoader_1_0               ppb_url_loader_interface_1_0;
extern const struct PPB_URLLoaderTrusted_0_3        ppb_url_loader_trusted_interface_0_3;
extern const struct PPB_URLRequestInfo_1_0          ppb_url_request_info_interface_1_0;
extern const struct PPB_URLResponseInfo_1_0         ppb_url_response_info_interface_1_0;
extern const struct PPB_URLUtil_Dev_0_6             ppb_url_util_dev_interface_0_6;
extern const struct PPB_URLUtil_Dev_0_7             ppb_url_util_dev_interface_0_7;
extern const struct PPB_UMA_Private_0_3             ppb_uma_private_interface_0_3;
extern const struct PPB_Var_1_0                     ppb_var_interface_1_0;
extern const struct PPB_Var_1_1                     ppb_var_interface_1_1;
extern const struct PPB_Var_1_2                     ppb_var_interface_1_2;
extern const struct PPB_VarArrayBuffer_1_0          ppb_var_array_buffer_interface_1_0;
extern const struct PPB_VarArray_1_0                ppb_var_array_interface_1_0;
extern const struct PPB_VarDictionary_1_0           ppb_var_dictionary_interface_1_0;
extern const struct PPB_Var_Deprecated              ppb_var_deprecated_interface_0_3;
extern const struct PPB_VideoCapture_Dev_0_3        ppb_video_capture_dev_interface_0_3;
extern const struct PPB_VideoDecoder_Dev_0_16       ppb_video_decoder_dev_interface_0_16;
extern const struct PPB_View_Dev_0_1                ppb_view_dev_interface_0_1;
extern const struct PPB_View_1_0                    ppb_view_interface_1_0;
extern const struct PPB_View_1_1                    ppb_view_interface_1_1;
extern const struct PPB_View_1_2                    ppb_view_interface_1_2;
extern const struct PPB_WheelInputEvent_1_0         ppb_wheel_input_event_interface_1_0;
extern const struct PPB_Widget_Dev_0_3              ppb_widget_dev_interface_0_3;
extern const struct PPB_Widget_Dev_0_4              ppb_widget_dev_interface_0_4;
extern const struct PPB_X509Certificate_Private_0_1 ppb_x509_certificate_interface_0_1;
extern const struct PPB_Zoom_Dev_0_2                ppb_zoom_dev_interface_0_2;

const void *(*ppp_get_interface)(const char *interface_name);

const void *
ppb_get_interface(const char *interface_name)
{
#define ELSEIFBLOCK(DEF, name)                      \
    } else if (strcmp(interface_name, DEF) == 0) {  \
        return &name;

    trace_info_f("[PPB] {full} %s interface_name=%s\n", __func__, interface_name);

    if (0) {
        // placeholder

    ELSEIFBLOCK(PPB_CORE_INTERFACE_1_0,                 ppb_core_interface_1_0)
    ELSEIFBLOCK(PPB_AUDIO_CONFIG_INTERFACE_1_1,         ppb_audio_config_interface_1_1)
    ELSEIFBLOCK(PPB_AUDIO_INPUT_DEV_INTERFACE_0_3,      ppb_audio_input_dev_interface_0_3)
    ELSEIFBLOCK(PPB_AUDIO_INPUT_DEV_INTERFACE_0_4,      ppb_audio_input_dev_interface_0_4)
    ELSEIFBLOCK(PPB_AUDIO_INTERFACE_1_0,                ppb_audio_interface_1_0)
    ELSEIFBLOCK(PPB_AUDIO_INTERFACE_1_1,                ppb_audio_interface_1_1)
    ELSEIFBLOCK(PPB_BROWSERFONT_TRUSTED_INTERFACE_1_0,  ppb_browser_font_trusted_interface_1_0)
    ELSEIFBLOCK(PPB_BUFFER_DEV_INTERFACE_0_4,           ppb_buffer_dev_interface_0_4)
    ELSEIFBLOCK(PPB_CHAR_SET_DEV_INTERFACE_0_4,         ppb_char_set_dev_interface_0_4)
    ELSEIFBLOCK(PPB_CRYPTO_DEV_INTERFACE_0_1,           ppb_crypto_dev_interface_0_1)
    ELSEIFBLOCK(PPB_CURSOR_CONTROL_DEV_INTERFACE_0_4,   ppb_cursor_control_dev_interface_0_4)
    ELSEIFBLOCK(PPB_DEVICEREF_DEV_INTERFACE_0_1,        ppb_device_ref_dev_interface_0_1)
    ELSEIFBLOCK(PPB_FILECHOOSER_DEV_INTERFACE_0_6,      ppb_file_chooser_dev_interface_0_6)
    ELSEIFBLOCK(PPB_FILECHOOSER_TRUSTED_INTERFACE_0_6,  ppb_file_chooser_trusted_interface_0_6)
    ELSEIFBLOCK(PPB_FILEIO_PRIVATE_INTERFACE_0_1,       ppb_file_io_private_interface_0_1)
    ELSEIFBLOCK(PPB_FILEIO_INTERFACE_1_0,               ppb_file_io_interface_1_0)
    ELSEIFBLOCK(PPB_FILEIO_INTERFACE_1_1,               ppb_file_io_interface_1_1)
    ELSEIFBLOCK(PPB_FILEREF_INTERFACE_1_0,              ppb_file_ref_interface_1_0)
    ELSEIFBLOCK(PPB_FILEREF_INTERFACE_1_1,              ppb_file_ref_interface_1_1)
    ELSEIFBLOCK(PPB_FLASH_CLIPBOARD_INTERFACE_5_0,      ppb_flash_clipboard_interface_5_0)
    ELSEIFBLOCK(PPB_FLASH_CLIPBOARD_INTERFACE_5_1,      ppb_flash_clipboard_interface_5_1)
    ELSEIFBLOCK(PPB_FLASH_DRM_INTERFACE_1_0,            ppb_flash_drm_interface_1_0)
    ELSEIFBLOCK(PPB_FLASH_DRM_INTERFACE_1_1,            ppb_flash_drm_interface_1_1)
    ELSEIFBLOCK(PPB_FLASH_FILE_FILEREF_INTERFACE,       ppb_flash_file_file_ref_interface_2_0)
    ELSEIFBLOCK(PPB_FLASH_FILE_MODULELOCAL_INTERFACE_3_0, ppb_flash_file_modulelocal_interface_3_0)
    ELSEIFBLOCK(PPB_FLASH_FONTFILE_INTERFACE_0_1,       ppb_flash_font_file_interface_0_1)
    ELSEIFBLOCK(PPB_FLASHFULLSCREEN_INTERFACE_1_0,      ppb_flash_fullscreen_interface_1_0)
    ELSEIFBLOCK(PPB_FLASH_INTERFACE_12_6,               ppb_flash_interface_12_6)
    ELSEIFBLOCK(PPB_FLASH_INTERFACE_13_0,               ppb_flash_interface_13_0)
    ELSEIFBLOCK(PPB_FLASH_MENU_INTERFACE_0_2,           ppb_flash_menu_interface_0_2)
    ELSEIFBLOCK(PPB_FLASH_MESSAGELOOP_INTERFACE_0_1,    ppb_flash_message_loop_interface_0_1)
    ELSEIFBLOCK(PPB_FONT_DEV_INTERFACE_0_6,             ppb_font_dev_interface_0_6)
    ELSEIFBLOCK(PPB_GRAPHICS_2D_INTERFACE_1_0,          ppb_graphics2d_interface_1_0)
    ELSEIFBLOCK(PPB_GRAPHICS_2D_INTERFACE_1_1,          ppb_graphics2d_interface_1_1)
    ELSEIFBLOCK(PPB_GRAPHICS_3D_INTERFACE_1_0,          ppb_graphics3d_interface_1_0)
    ELSEIFBLOCK(PPB_HOSTRESOLVER_PRIVATE_INTERFACE_0_1, ppb_host_resolver_private_interface_0_1)
    ELSEIFBLOCK(PPB_IMAGEDATA_INTERFACE_1_0,            ppb_image_data_interface_1_0)
    ELSEIFBLOCK(PPB_IME_INPUT_EVENT_DEV_INTERFACE_0_2,  ppb_ime_input_event_dev_interface_0_2)
    ELSEIFBLOCK(PPB_INPUT_EVENT_INTERFACE_1_0,          ppb_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_MOUSE_INPUT_EVENT_INTERFACE_1_1,    ppb_mouse_input_event_interface_1_1)
    ELSEIFBLOCK(PPB_WHEEL_INPUT_EVENT_INTERFACE_1_0,    ppb_wheel_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_KEYBOARD_INPUT_EVENT_INTERFACE_1_0, ppb_keyboard_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_KEYBOARD_INPUT_EVENT_INTERFACE_1_2, ppb_keyboard_input_event_interface_1_2)
    ELSEIFBLOCK(PPB_TOUCH_INPUT_EVENT_INTERFACE_1_0,    ppb_touch_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_IME_INPUT_EVENT_INTERFACE_1_0,      ppb_ime_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_INSTANCE_INTERFACE_1_0,             ppb_instance_interface_1_0)
    ELSEIFBLOCK(PPB_INSTANCE_PRIVATE_INTERFACE_0_1,     ppb_instance_private_interface_0_1)
    ELSEIFBLOCK(PPB_MEMORY_DEV_INTERFACE_0_1,           ppb_memory_dev_interface_0_1)
    ELSEIFBLOCK(PPB_MESSAGING_INTERFACE_1_0,            ppb_messaging_interface_1_0)
    ELSEIFBLOCK(PPB_MESSAGING_INTERFACE_1_2,            ppb_messaging_interface_1_2)
    ELSEIFBLOCK(PPB_NETADDRESS_PRIVATE_INTERFACE_1_1,   ppb_net_address_private_interface_1_1)
    ELSEIFBLOCK(PPB_NETWORKMONITOR_INTERFACE_1_0,       ppb_network_monitor_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_CHROMIUMENABLEFEATURE_INTERFACE_1_0, ppb_opengles2_chromium_enable_feature_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_CHROMIUMMAPSUB_INTERFACE_1_0, ppb_opengles2_chromium_map_sub_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_FRAMEBUFFERBLIT_INTERFACE_1_0, ppb_opengles2_framebuffer_blit_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_FRAMEBUFFERMULTISAMPLE_INTERFACE_1_0, ppb_opengles2_framebuffer_multisample_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_INSTANCEDARRAYS_INTERFACE_1_0, ppb_opengles2_instanced_arrays_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_INTERFACE_1_0,            ppb_opengles2_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_QUERY_INTERFACE_1_0,      ppb_opengles2_query_interface_1_0)
    ELSEIFBLOCK(PPB_PDF_INTERFACE,                      ppb_pdf_interface)
    ELSEIFBLOCK(PPB_PRINTING_DEV_INTERFACE_0_7,         ppb_printing_dev_interface_0_7)
    ELSEIFBLOCK(PPB_SCROLLBAR_DEV_INTERFACE_0_5,        ppb_scrollbar_dev_interface_0_5)
    ELSEIFBLOCK(PPB_TCPSOCKET_PRIVATE_INTERFACE_0_4,    ppb_tcp_socket_private_interface_0_4)
    ELSEIFBLOCK(PPB_TCPSOCKET_PRIVATE_INTERFACE_0_5,    ppb_tcp_socket_private_interface_0_5)
    ELSEIFBLOCK(PPB_TEXTINPUTCONTROLLER_INTERFACE_1_0,  ppb_text_input_controller_interface_1_0)
    ELSEIFBLOCK(PPB_TEXTINPUT_DEV_INTERFACE_0_2,        ppb_text_input_dev_interface_0_2)
    ELSEIFBLOCK(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4,    ppb_udp_socket_private_interface_0_4)
    ELSEIFBLOCK(PPB_URLLOADER_INTERFACE_1_0,            ppb_url_loader_interface_1_0)
    ELSEIFBLOCK(PPB_URLLOADERTRUSTED_INTERFACE_0_3,     ppb_url_loader_trusted_interface_0_3)
    ELSEIFBLOCK(PPB_URLREQUESTINFO_INTERFACE_1_0,       ppb_url_request_info_interface_1_0)
    ELSEIFBLOCK(PPB_URLRESPONSEINFO_INTERFACE_1_0,      ppb_url_response_info_interface_1_0)
    ELSEIFBLOCK(PPB_URLUTIL_DEV_INTERFACE_0_6,          ppb_url_util_dev_interface_0_6)
    ELSEIFBLOCK(PPB_URLUTIL_DEV_INTERFACE_0_7,          ppb_url_util_dev_interface_0_7)
    ELSEIFBLOCK(PPB_UMA_PRIVATE_INTERFACE_0_3,          ppb_uma_private_interface_0_3)
    ELSEIFBLOCK(PPB_VAR_INTERFACE_1_0,                  ppb_var_interface_1_0)
    ELSEIFBLOCK(PPB_VAR_INTERFACE_1_1,                  ppb_var_interface_1_1)
    ELSEIFBLOCK(PPB_VAR_INTERFACE_1_2,                  ppb_var_interface_1_2)
    ELSEIFBLOCK(PPB_VAR_ARRAY_BUFFER_INTERFACE_1_0,     ppb_var_array_buffer_interface_1_0)
    ELSEIFBLOCK(PPB_VAR_ARRAY_INTERFACE_1_0,            ppb_var_array_interface_1_0)
    ELSEIFBLOCK(PPB_VAR_DICTIONARY_INTERFACE_1_0,       ppb_var_dictionary_interface_1_0)
    ELSEIFBLOCK(PPB_VAR_DEPRECATED_INTERFACE_0_3,       ppb_var_deprecated_interface_0_3)
    ELSEIFBLOCK(PPB_VIDEOCAPTURE_DEV_INTERFACE_0_3,     ppb_video_capture_dev_interface_0_3)
    ELSEIFBLOCK(PPB_VIDEODECODER_DEV_INTERFACE_0_16,    ppb_video_decoder_dev_interface_0_16)
    ELSEIFBLOCK(PPB_VIEW_DEV_INTERFACE_0_1,             ppb_view_dev_interface_0_1)
    ELSEIFBLOCK(PPB_VIEW_INTERFACE_1_0,                 ppb_view_interface_1_0)
    ELSEIFBLOCK(PPB_VIEW_INTERFACE_1_1,                 ppb_view_interface_1_1)
    ELSEIFBLOCK(PPB_VIEW_INTERFACE_1_2,                 ppb_view_interface_1_2)
    ELSEIFBLOCK(PPB_WIDGET_DEV_INTERFACE_0_3,           ppb_widget_dev_interface_0_3)
    ELSEIFBLOCK(PPB_WIDGET_DEV_INTERFACE_0_4,           ppb_widget_dev_interface_0_4)
    ELSEIFBLOCK(PPB_X509CERTIFICATE_PRIVATE_INTERFACE_0_1, ppb_x509_certificate_interface_0_1)
    ELSEIFBLOCK(PPB_ZOOM_DEV_INTERFACE_0_2,             ppb_zoom_dev_interface_0_2)
    } else {
        trace_info("not implemented: %s\n", interface_name);
        return NULL;
    }
#undef ELSEIFBLOCK
}
