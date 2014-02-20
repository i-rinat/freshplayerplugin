/*
 * Copyright © 2013-2014  Rinat Ibragimov
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
#include "interface_list.h"
#include "trace.h"
#include <ppapi/c/ppb.h>


const void *(*ppp_get_interface)(const char *interface_name);

const void *
ppb_get_interface(const char *interface_name)
{
#define ELSEIFBLOCK(DEF, name)                      \
    } else if (strcmp(interface_name, DEF) == 0) {  \
        return &name;

    trace_info("[PPB] {full} %s interface_name=%s\n", __func__, interface_name);

    if (0) {
        // placeholder

    ELSEIFBLOCK(PPB_CORE_INTERFACE_1_0,                 ppb_core_interface_1_0)
    ELSEIFBLOCK(PPB_AUDIO_CONFIG_INTERFACE_1_1,         ppb_audio_config_interface_1_1)
    ELSEIFBLOCK(PPB_AUDIO_INPUT_DEV_INTERFACE_0_3,      ppb_audio_input_dev_interface_0_3)
    ELSEIFBLOCK(PPB_AUDIO_INPUT_DEV_INTERFACE_0_4,      ppb_audio_input_dev_interface_0_4)
    ELSEIFBLOCK(PPB_AUDIO_INTERFACE_1_0,                ppb_audio_interface_1_0)
    ELSEIFBLOCK(PPB_BROWSERFONT_TRUSTED_INTERFACE_1_0,  ppb_browser_font_trusted_interface_1_0)
    ELSEIFBLOCK(PPB_BUFFER_DEV_INTERFACE_0_4,           ppb_buffer_dev_interface_0_4)
    ELSEIFBLOCK(PPB_CHAR_SET_DEV_INTERFACE_0_4,         ppb_char_set_dev_interface_0_4)
    ELSEIFBLOCK(PPB_CRYPTO_DEV_INTERFACE_0_1,           ppb_crypto_dev_interface_0_1)
    ELSEIFBLOCK(PPB_CURSOR_CONTROL_DEV_INTERFACE_0_4,   ppb_cursor_control_dev_interface_0_4)
    ELSEIFBLOCK(PPB_FILECHOOSER_DEV_INTERFACE_0_6,      ppb_file_chooser_dev_interface_0_6)
    ELSEIFBLOCK(PPB_FILECHOOSER_TRUSTED_INTERFACE_0_6,  ppb_file_chooser_trusted_interface_0_6)
    ELSEIFBLOCK(PPB_FILEREF_INTERFACE_1_0,              ppb_file_ref_interface_1_0)
    ELSEIFBLOCK(PPB_FLASH_CLIPBOARD_INTERFACE_5_0,      ppb_flash_clipboard_interface_5_0)
    ELSEIFBLOCK(PPB_FLASH_FILE_FILEREF_INTERFACE,       ppb_flash_file_file_ref_interface_2_0)
    ELSEIFBLOCK(PPB_FLASH_FILE_MODULELOCAL_INTERFACE_3_0, ppb_flash_file_modulelocal_interface_3_0)
    ELSEIFBLOCK(PPB_FLASH_FONTFILE_INTERFACE_0_1,       ppb_flash_font_file_interface_0_1)
    ELSEIFBLOCK(PPB_FLASHFULLSCREEN_INTERFACE_1_0,      ppb_flash_fullscreen_interface_1_0)
    ELSEIFBLOCK(PPB_FLASH_INTERFACE_13_0,               ppb_flash_interface_13_0)
    ELSEIFBLOCK(PPB_FLASH_MENU_INTERFACE_0_2,           ppb_flash_menu_interface_0_2)
    ELSEIFBLOCK(PPB_GRAPHICS_2D_INTERFACE_1_0,          ppb_graphics2d_interface_1_0)
    ELSEIFBLOCK(PPB_GRAPHICS_2D_INTERFACE_1_1,          ppb_graphics2d_interface_1_1)
    ELSEIFBLOCK(PPB_GRAPHICS_3D_INTERFACE_1_0,          ppb_graphics3d_interface_1_0)
    ELSEIFBLOCK(PPB_IMAGEDATA_INTERFACE_1_0,            ppb_image_data_interface_1_0)
    ELSEIFBLOCK(PPB_IME_INPUT_EVENT_DEV_INTERFACE_0_2,  ppb_ime_input_event_dev_interface_0_2)
    ELSEIFBLOCK(PPB_INPUT_EVENT_INTERFACE_1_0,          ppb_input_event_interface_1_0)
    ELSEIFBLOCK(PPB_INSTANCE_INTERFACE_1_0,             ppb_instance_interface_1_0)
    ELSEIFBLOCK(PPB_INSTANCE_PRIVATE_INTERFACE_0_1,     ppb_instance_private_interface_0_1)
    ELSEIFBLOCK(PPB_MEMORY_DEV_INTERFACE_0_1,           ppb_memory_dev_interface_0_1)
    ELSEIFBLOCK(PPB_NETADDRESS_PRIVATE_INTERFACE_1_1,   ppb_net_address_private_interface_1_1)
    ELSEIFBLOCK(PPB_NETWORKMONITOR_INTERFACE_1_0,       ppb_network_monitor_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_CHROMIUMMAPSUB_INTERFACE_1_0, ppb_opengles2_chromium_map_sub_interface_1_0)
    ELSEIFBLOCK(PPB_OPENGLES2_INTERFACE_1_0,            ppb_opengles2_interface_1_0)
    ELSEIFBLOCK(PPB_TCPSOCKET_PRIVATE_INTERFACE_0_4,    ppb_tcp_socket_private_interface_0_4)
    ELSEIFBLOCK(PPB_TEXTINPUT_DEV_INTERFACE_0_2,        ppb_text_input_dev_interface_0_2)
    ELSEIFBLOCK(PPB_UDPSOCKET_PRIVATE_INTERFACE_0_4,    ppb_udp_socket_private_interface_0_4)
    ELSEIFBLOCK(PPB_URLLOADER_INTERFACE_1_0,            ppb_url_loader_interface_1_0)
    ELSEIFBLOCK(PPB_URLLOADERTRUSTED_INTERFACE_0_3,     ppb_url_loader_trusted_interface_0_3)
    ELSEIFBLOCK(PPB_URLREQUESTINFO_INTERFACE_1_0,       ppb_url_request_info_interface_1_0)
    ELSEIFBLOCK(PPB_URLRESPONSEINFO_INTERFACE_1_0,      ppb_url_response_info_interface_1_0)
    ELSEIFBLOCK(PPB_URLUTIL_DEV_INTERFACE_0_6,          ppb_url_util_dev_interface_0_6)
    ELSEIFBLOCK(PPB_VAR_INTERFACE_1_0,                  ppb_var_interface_1_0)
    ELSEIFBLOCK(PPB_VAR_INTERFACE_1_1,                  ppb_var_interface_1_1)
    ELSEIFBLOCK(PPB_VIDEOCAPTURE_DEV_INTERFACE_0_3,     ppb_video_capture_dev_interface_0_3)
    ELSEIFBLOCK(PPB_VIEW_INTERFACE_1_0,                 ppb_view_interface_1_0)
    ELSEIFBLOCK(PPB_VIEW_INTERFACE_1_1,                 ppb_view_interface_1_1)
    } else {
        trace_info("not implemented: %s\n", interface_name);
        return NULL;
    }
#undef ELSEIFBLOCK
}
