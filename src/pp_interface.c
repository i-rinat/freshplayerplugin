#include <string.h>
#include "pp_interface.h"
#include "interface_list.h"
#include "trace.h"
#include <ppapi/c/ppb.h>


const void *(*ppp_get_interface)(const char *interface_name);

const void *
ppb_get_interface(const char *interface_name)
{
    trace_info("[PPB] {full} %s interface_name=%s\n", __func__, interface_name);
    if (!strcmp(interface_name, PPB_CORE_INTERFACE_1_0)) {
        return &ppb_core_interface_1_0;
    } else if (!strcmp(interface_name, PPB_AUDIO_CONFIG_INTERFACE_1_1)) {
        return &ppb_audio_config_interface_1_1;
    } else if (!strcmp(interface_name, PPB_AUDIO_INPUT_DEV_INTERFACE_0_3)) {
        return &ppb_audio_input_dev_interface_0_3;
    } else if (!strcmp(interface_name, PPB_AUDIO_INPUT_DEV_INTERFACE_0_4)) {
        return &ppb_audio_input_dev_interface_0_4;
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
    } else if (!strcmp(interface_name, PPB_INSTANCE_PRIVATE_INTERFACE_0_1)) {
        return &ppb_instance_private_interface_0_1;
    } else if (!strcmp(interface_name, PPB_MEMORY_DEV_INTERFACE_0_1)) {
        return &ppb_memory_dev_interface_0_1;
    } else if (!strcmp(interface_name, PPB_NETADDRESS_PRIVATE_INTERFACE_1_1)) {
        return &ppb_net_address_private_interface_1_1;
    } else if (!strcmp(interface_name, PPB_NETWORKMONITOR_INTERFACE_1_0)) {
        return &ppb_network_monitor_interface_1_0;
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
    } else if (!strcmp(interface_name, PPB_VAR_INTERFACE_1_0)) {
        return &ppb_var_interface_1_0;
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
