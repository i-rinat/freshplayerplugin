/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#pragma once

struct fpp_config_s {
    int     audio_buffer_min_ms;
    int     audio_buffer_max_ms;
    int     audio_use_jack;
    int     jack_autoconnect_ports;
    char   *jack_server_name;
    int     jack_autostart_server;
    char   *pepperflash_path;
    char   *flash_command_line;
    int     enable_3d;
    int     enable_hwdec;
    int     quiet;
    int     fullscreen_width;
    int     fullscreen_height;
    int     fullscreen_horz_maximize_atom;
    int     fullscreen_vert_maximize_atom;
    int     randomize_dns_case;
    double  device_scale;
    int     enable_windowed_mode;
    int     enable_xembed;
    int     enable_vaapi;
    int     enable_vdpau;
    int     tie_fullscreen_window_to_browser;
    int     vsync_afterwait_us;
    int     fs_delay_ms;
    int     enable_vsync;
    int     double_click_delay_ms;
    int     show_version_info;
    int     probe_video_capture_devices;
    int     enable_xrender;
    struct {
        int   connect_first_loader_to_unrequested_stream;
        int   dump_resource_histogram;
        int   dump_variables;
        int   plasma5_screensaver;
        int   plugin_missing;
        int   incompatible_npapi_version;
        int   x_synchronize;
        int   avoid_stdout;
    } quirks;
};

extern struct fpp_config_s config;

void
fpp_config_initialize(void);

void
fpp_config_destroy(void);

const char *
fpp_config_get_pepper_data_dir(void);

const char *
fpp_config_get_pepper_salt_file_name(void);
