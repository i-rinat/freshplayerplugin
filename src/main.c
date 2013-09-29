#include <dlfcn.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/pp_module.h>
#include "interface_list.h"
#include "trace.h"

#define PPFP_PATH "/opt/google/chrome/PepperFlash/libpepflashplayer.so"

static int32_t (*ppp_initialize_module)(PP_Module module_id,
                                        PPB_GetInterface get_browser_interface);

static const void* (*ppp_get_interface)(const char* interface_name);

const void *
browser_get_interface(const char* interface_name)
{
    trace_info("called PPB_GetInterface(\"%s\")\n", interface_name);
    if (!strcmp(interface_name, PPB_CORE_INTERFACE)) {
        return &ppb_core;
    } else if (!strcmp(interface_name, PPB_AUDIO_CONFIG_INTERFACE)) {
        return &ppb_audio_config;
    } else if (!strcmp(interface_name, PPB_AUDIO_INPUT_DEV_INTERFACE)) {
        return &ppb_audio_input_dev;
    } else if (!strcmp(interface_name, PPB_AUDIO_INTERFACE)) {
        return &ppb_audio;
    } else if (!strcmp(interface_name, PPB_BROWSERFONT_TRUSTED_INTERFACE)) {
        return &ppb_browser_font_trusted;
    } else if (!strcmp(interface_name, PPB_BUFFER_DEV_INTERFACE)) {
        return &ppb_buffer_dev;
    } else if (!strcmp(interface_name, PPB_CHAR_SET_DEV_INTERFACE)) {
        return &ppb_char_set_dev;
    } else if (!strcmp(interface_name, PPB_CRYPTO_DEV_INTERFACE)) {
        return &ppb_crypto_dev;
    } else if (!strcmp(interface_name, PPB_CURSOR_CONTROL_DEV_INTERFACE)) {
        return &ppb_cursor_control_dev;
    } else if (!strcmp(interface_name, PPB_FILECHOOSER_DEV_INTERFACE)) {
        return &ppb_file_chooser_dev;
    } else if (!strcmp(interface_name, PPB_FILECHOOSER_TRUSTED_INTERFACE)) {
        return &ppb_file_chooser_trusted;
    } else if (!strcmp(interface_name, PPB_FILEREF_INTERFACE_1_0)) {
        return &ppb_file_ref_1_0;
    } else if (!strcmp(interface_name, PPB_FLASH_CLIPBOARD_INTERFACE)) {
        return &ppb_flash_clipboard;
    } else if (!strcmp(interface_name, PPB_FLASH_FILE_FILEREF_INTERFACE)) {
        return &ppb_flash_file_file_ref;
    } else if (!strcmp(interface_name, PPB_FLASH_FILE_MODULELOCAL_INTERFACE)) {
        return &ppb_flash_file_modulelocal;
    } else if (!strcmp(interface_name, PPB_FLASH_FONTFILE_INTERFACE)) {
        return &ppb_flash_font_file;
    } else if (!strcmp(interface_name, PPB_FLASHFULLSCREEN_INTERFACE)) {
        return &ppb_flash_fullscreen;
    } else if (!strcmp(interface_name, PPB_FLASH_INTERFACE)) {
        return &ppb_flash;
    } else if (!strcmp(interface_name, PPB_FLASH_MENU_INTERFACE)) {
        return &ppb_flash_menu;
    } else if (!strcmp(interface_name, PPB_GRAPHICS_2D_INTERFACE_1_0)) {
        return &ppb_graphics2d_1_0;
    } else if (!strcmp(interface_name, PPB_GRAPHICS_3D_INTERFACE)) {
        return &ppb_graphics3d;
    } else if (!strcmp(interface_name, PPB_IMAGEDATA_INTERFACE)) {
        return &ppb_image_data;
    } else if (!strcmp(interface_name, PPB_IME_INPUT_EVENT_DEV_INTERFACE)) {
        return &ppb_ime_input_event_dev;
    } else if (!strcmp(interface_name, PPB_INPUT_EVENT_INTERFACE)) {
        return &ppb_input_event;
    } else if (!strcmp(interface_name, PPB_INSTANCE_INTERFACE)) {
        return &ppb_instance;
    } else if (!strcmp(interface_name, PPB_MEMORY_DEV_INTERFACE)) {
        return &ppb_memory_dev;
    } else if (!strcmp(interface_name, PPB_NETADDRESS_PRIVATE_INTERFACE)) {
        return &ppb_net_address_private;
    } else if (!strcmp(interface_name, PPB_OPENGLES2_CHROMIUMMAPSUB_INTERFACE)) {
        return &ppb_opengles2_chromium_map_sub;
    } else if (!strcmp(interface_name, PPB_OPENGLES2_INTERFACE)) {
        return &ppb_opengles2;
    } else if (!strcmp(interface_name, PPB_TCPSOCKET_PRIVATE_INTERFACE_0_4)) {
        return &ppb_tcp_socket_private_0_4;
    } else if (!strcmp(interface_name, PPB_TEXTINPUT_DEV_INTERFACE)) {
        return &ppb_text_input_dev;
    } else if (!strcmp(interface_name, PPB_UDPSOCKET_PRIVATE_INTERFACE)) {
        return &ppb_udp_socket_private;
    } else if (!strcmp(interface_name, PPB_URLLOADER_INTERFACE)) {
        return &ppb_url_loader;
    } else if (!strcmp(interface_name, PPB_URLLOADERTRUSTED_INTERFACE)) {
        return &ppb_url_loader_trusted;
    } else if (!strcmp(interface_name, PPB_URLREQUESTINFO_INTERFACE)) {
        return &ppb_url_request_info;
    } else if (!strcmp(interface_name, PPB_URLRESPONSEINFO_INTERFACE)) {
        return &ppb_url_response_info;
    } else if (!strcmp(interface_name, PPB_URLUTIL_DEV_INTERFACE)) {
        return &ppb_url_util_dev;
    } else if (!strcmp(interface_name, PPB_VAR_INTERFACE)) {
        return &ppb_var;
    } else if (!strcmp(interface_name, PPB_VIDEOCAPTURE_DEV_INTERFACE)) {
        return &ppb_video_capture_dev;
    } else if (!strcmp(interface_name, PPB_VIEW_INTERFACE_1_0)) {
        return &ppb_view_1_0;
    } else {
        trace_info("not implemented: %s\n", interface_name);
        return NULL;
    }
}

int
load_pepper_plugin(void)
{
    void *h = dlopen(PPFP_PATH, RTLD_LAZY);
    if (!h) {
        trace_info("can't open " PPFP_PATH "\n");
        return 1;
    }
    ppp_initialize_module = dlsym(h, "PPP_InitializeModule");
    ppp_get_interface = dlsym(h, "PPP_GetInterface");
    if (!ppp_initialize_module || !ppp_get_interface) {
        trace_info("one of required PPP_* is missing\n");
        return 1;
    }

    return 0;
}

int
main(void)
{
    if (load_pepper_plugin() != 0) {
        trace_info("error loading PepperFlash plugin\n");
        return 1;
    }

    int32_t res = ppp_initialize_module(42, browser_get_interface);
    trace_info("res = %d\n", res);

    return 0;
}
