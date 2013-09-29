#ifndef FPP__INTERFACE_LIST_H
#define FPP__INTERFACE_LIST_H

#include <ppapi/c/ppb_audio_config.h>
#include <ppapi/c/ppb_audio.h>
#include <ppapi/c/ppb_core.h>
#include <ppapi/c/ppb_file_ref.h>
#include <ppapi/c/ppb_graphics_2d.h>
#include <ppapi/c/ppb_graphics_3d.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/ppb_image_data.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/ppb_opengles2.h>
#include <ppapi/c/ppb_url_loader.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_view.h>
#include <ppapi/c/pp_module.h>

#include <ppapi/c/dev/ppb_audio_input_dev.h>
#include <ppapi/c/dev/ppb_buffer_dev.h>
#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <ppapi/c/dev/ppb_crypto_dev.h>
#include <ppapi/c/dev/ppb_cursor_control_dev.h>
#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <ppapi/c/dev/ppb_memory_dev.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/dev/ppb_url_util_dev.h>
#include <ppapi/c/dev/ppb_video_capture_dev.h>

#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <ppapi/c/private/ppb_flash_file.h>
#include <ppapi/c/private/ppb_flash_font_file.h>
#include <ppapi/c/private/ppb_flash_fullscreen.h>
#include <ppapi/c/private/ppb_flash.h>
#include <ppapi/c/private/ppb_flash_menu.h>
#include <ppapi/c/private/ppb_tcp_socket_private.h>
#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <ppapi/c/trusted/ppb_browser_font_trusted.h>
#include <ppapi/c/trusted/ppb_file_chooser_trusted.h>
#include <ppapi/c/trusted/ppb_url_loader_trusted.h>

extern const PPB_Core ppb_core;
extern const PPB_AudioConfig ppb_audio_config;
extern const PPB_AudioInput_Dev ppb_audio_input_dev;
extern const PPB_Audio ppb_audio;
extern const PPB_BrowserFont_Trusted ppb_browser_font_trusted;
extern const PPB_Buffer_Dev ppb_buffer_dev;
extern const PPB_CharSet_Dev ppb_char_set_dev;
extern const PPB_Crypto_Dev ppb_crypto_dev;
extern const PPB_CursorControl_Dev ppb_cursor_control_dev;
extern const PPB_FileChooser_Dev ppb_file_chooser_dev;
extern const PPB_FileChooserTrusted ppb_file_chooser_trusted;
extern const struct PPB_FileRef_1_0 ppb_file_ref_1_0;
extern const PPB_Flash_Clipboard ppb_flash_clipboard;
extern const struct PPB_Flash_File_FileRef ppb_flash_file_file_ref;
extern const PPB_Flash_File_ModuleLocal ppb_flash_file_modulelocal;
extern const PPB_Flash_FontFile ppb_flash_font_file;
extern const PPB_FlashFullscreen ppb_flash_fullscreen;
extern const PPB_Flash ppb_flash;
extern const PPB_Flash_Menu ppb_flash_menu;
extern const struct PPB_Graphics2D_1_0 ppb_graphics2d_1_0;
extern const PPB_Graphics3D ppb_graphics3d;
extern const PPB_ImageData ppb_image_data;
extern const PPB_IMEInputEvent_Dev ppb_ime_input_event_dev;
extern const PPB_InputEvent ppb_input_event;
extern const PPB_Instance ppb_instance;
extern const PPB_Memory_Dev ppb_memory_dev;
extern const PPB_NetAddress_Private ppb_net_address_private;
extern const struct PPB_OpenGLES2ChromiumMapSub ppb_opengles2_chromium_map_sub;
extern const struct PPB_OpenGLES2 ppb_opengles2;
extern const struct PPB_TCPSocket_Private_0_4 ppb_tcp_socket_private_0_4;
extern const PPB_TextInput_Dev ppb_text_input_dev;
extern const PPB_UDPSocket_Private ppb_udp_socket_private;
extern const PPB_URLLoader ppb_url_loader;
extern const PPB_URLLoaderTrusted ppb_url_loader_trusted;
extern const PPB_URLRequestInfo ppb_url_request_info;
extern const PPB_URLResponseInfo ppb_url_response_info;
extern const PPB_URLUtil_Dev ppb_url_util_dev;
extern const PPB_Var ppb_var;
extern const PPB_VideoCapture_Dev ppb_video_capture_dev;
extern const struct PPB_View_1_0 ppb_view_1_0;

#endif // FPP__INTERFACE_LIST_H
