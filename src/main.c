#include <dlfcn.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ppapi/c/ppb.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/ppp_instance.h>
#include "interface_list.h"
#include "trace.h"

#define PPFP_PATH "/opt/google/chrome/PepperFlash/libpepflashplayer.so"

static int32_t (*ppp_initialize_module)(PP_Module module_id,
                                        PPB_GetInterface get_browser_interface);

static const void *(*ppp_get_interface)(const char *interface_name);

const void *
browser_get_interface(const char *interface_name)
{
    trace_info("called PPB_GetInterface(\"%s\")\n", interface_name);
    if (!strcmp(interface_name, PPB_CORE_INTERFACE_1_0)) {
        return &ppb_core_interface_1_0;
    } else if (!strcmp(interface_name, PPB_AUDIO_CONFIG_INTERFACE_1_1)) {
        return &ppb_audio_config_interface_1_1;
    } else if (!strcmp(interface_name, PPB_AUDIO_INPUT_DEV_INTERFACE_0_3)) {
        return &ppb_audio_input_dev_interface_0_3;
    } else if (!strcmp(interface_name, PPB_AUDIO_INTERFACE_1_0)) {
        return &ppb_audio_interface_1_0;
    } else if (!strcmp(interface_name, PPB_BROWSERFONT_TRUSTED_INTERFACE_1_0)) {
        return &ppb_browser_font_trusted_interface_1_0;
    } else if (!strcmp(interface_name, PPB_BUFFER_DEV_INTERFACE_0_4)) {
        return &ppb_buffer_dev_interface_0_4;
    } else if (!strcmp(interface_name, PPB_CHAR_SET_DEV_INTERFACE_0_4)) {
        return &ppb_char_set_dev_interface_0_4;
    } else if (!strcmp(interface_name, PPB_CRYPTO_DEV_INTERFACE_0_1)) {
        return &ppb_crypto_dev_interface_0_1;
    } else if (!strcmp(interface_name, PPB_CURSOR_CONTROL_DEV_INTERFACE_0_4)) {
        return &ppb_cursor_control_dev_interface_0_4;
    } else if (!strcmp(interface_name, PPB_FILECHOOSER_DEV_INTERFACE_0_6)) {
        return &ppb_file_chooser_dev_interface_0_6;
    } else if (!strcmp(interface_name, PPB_FILECHOOSER_TRUSTED_INTERFACE_0_6)) {
        return &ppb_file_chooser_trusted_interface_0_6;
    } else if (!strcmp(interface_name, PPB_FILEREF_INTERFACE_1_0)) {
        return &ppb_file_ref_interface_1_0;
    } else if (!strcmp(interface_name, PPB_FLASH_CLIPBOARD_INTERFACE_5_0)) {
        return &ppb_flash_clipboard_interface_5_0;
    } else if (!strcmp(interface_name, PPB_FLASH_FILE_FILEREF_INTERFACE)) {
        return &ppb_flash_file_file_ref_interface_2_0;
    } else if (!strcmp(interface_name, PPB_FLASH_FILE_MODULELOCAL_INTERFACE_3_0)) {
        return &ppb_flash_file_modulelocal_interface_3_0;
    } else if (!strcmp(interface_name, PPB_FLASH_FONTFILE_INTERFACE_0_1)) {
        return &ppb_flash_font_file_interface_0_1;
    } else if (!strcmp(interface_name, PPB_FLASHFULLSCREEN_INTERFACE_1_0)) {
        return &ppb_flash_fullscreen_interface_1_0;
    } else if (!strcmp(interface_name, PPB_FLASH_INTERFACE_13_0)) {
        return &ppb_flash_interface_13_0;
    } else if (!strcmp(interface_name, PPB_FLASH_MENU_INTERFACE_0_2)) {
        return &ppb_flash_menu_interface_0_2;
    } else if (!strcmp(interface_name, PPB_GRAPHICS_2D_INTERFACE_1_0)) {
        return &ppb_graphics2d_interface_1_0;
    } else if (!strcmp(interface_name, PPB_GRAPHICS_3D_INTERFACE_1_0)) {
        return &ppb_graphics3d_interface_1_0;
    } else if (!strcmp(interface_name, PPB_IMAGEDATA_INTERFACE_1_0)) {
        return &ppb_image_data_interface_1_0;
    } else if (!strcmp(interface_name, PPB_IME_INPUT_EVENT_DEV_INTERFACE_0_2)) {
        return &ppb_ime_input_event_dev_interface_0_2;
    } else if (!strcmp(interface_name, PPB_INPUT_EVENT_INTERFACE_1_0)) {
        return &ppb_input_event_interface_1_0;
    } else if (!strcmp(interface_name, PPB_INSTANCE_INTERFACE_1_0)) {
        return &ppb_instance_interface_1_0;
    } else if (!strcmp(interface_name, PPB_MEMORY_DEV_INTERFACE_0_1)) {
        return &ppb_memory_dev_interface_0_1;
    } else if (!strcmp(interface_name, PPB_NETADDRESS_PRIVATE_INTERFACE_1_1)) {
        return &ppb_net_address_private_interface_1_1;
    } else if (!strcmp(interface_name, PPB_OPENGLES2_CHROMIUMMAPSUB_INTERFACE_1_0)) {
        return &ppb_opengles2_chromium_map_sub_interface_1_0;
    } else if (!strcmp(interface_name, PPB_OPENGLES2_INTERFACE_1_0)) {
        return &ppb_opengles2_interface_1_0;
    } else if (!strcmp(interface_name, PPB_TCPSOCKET_PRIVATE_INTERFACE_0_4)) {
        return &ppb_tcp_socket_private_interface_0_4;
    } else if (!strcmp(interface_name, PPB_TEXTINPUT_DEV_INTERFACE_0_2)) {
        return &ppb_text_input_dev_interface_0_2;
    } else if (!strcmp(interface_name, PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4)) {
        return &ppb_udp_socket_private_interface_0_4;
    } else if (!strcmp(interface_name, PPB_URLLOADER_INTERFACE_1_0)) {
        return &ppb_url_loader_interface_1_0;
    } else if (!strcmp(interface_name, PPB_URLLOADERTRUSTED_INTERFACE_0_3)) {
        return &ppb_url_loader_trusted_interface_0_3;
    } else if (!strcmp(interface_name, PPB_URLREQUESTINFO_INTERFACE_1_0)) {
        return &ppb_url_request_info_interface_1_0;
    } else if (!strcmp(interface_name, PPB_URLRESPONSEINFO_INTERFACE_1_0)) {
        return &ppb_url_response_info_interface_1_0;
    } else if (!strcmp(interface_name, PPB_URLUTIL_DEV_INTERFACE_0_6)) {
        return &ppb_url_util_dev_interface_0_6;
    } else if (!strcmp(interface_name, PPB_VAR_INTERFACE_1_1)) {
        return &ppb_var_interface_1_1;
    } else if (!strcmp(interface_name, PPB_VIDEOCAPTURE_DEV_INTERFACE_0_3)) {
        return &ppb_video_capture_dev_interface_0_3;
    } else if (!strcmp(interface_name, PPB_VIEW_INTERFACE_1_0)) {
        return &ppb_view_interface_1_0;
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
    const struct PPP_Instance_1_1 *ppp_instance_1_1 = ppp_get_interface(PPP_INSTANCE_INTERFACE_1_1);
    trace_info("p = %p\n", ppp_instance_1_1);
    trace_info("DidCreate = %p\n", ppp_instance_1_1->DidCreate);
    trace_info("DidDestroy = %p\n", ppp_instance_1_1->DidDestroy);
    trace_info("DidChangeView = %p\n", ppp_instance_1_1->DidChangeView);
    trace_info("DidChangeFocus = %p\n", ppp_instance_1_1->DidChangeFocus);
    trace_info("HandleDocumentLoad = %p\n", ppp_instance_1_1->HandleDocumentLoad);

    const char *argn[] = {"id"};
    const char *argv[] = {"hello"};
    ppp_instance_1_1->DidCreate(1, 1, argn, argv);


    return 0;
}
