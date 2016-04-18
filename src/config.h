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

#ifndef FPP_CONFIG_H
#define FPP_CONFIG_H

#include <stdint.h>


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

// plugin-specific
/**
 * Tell a plugin to initialize itself */
void
fpp_config_plugin_initialize(void);

void
fpp_config_destroy(void);

// plugin-specific
/**
 * Find .so to use and write its location, description, and version info
 * to the suuplied pointers. Memory of these details are solely the
 * responsibility of the plugin; plugin must be careful to free them
 * upon subsequent calls. Check out config_pepperflash.c for reference. */
void
fpp_config_get_plugin(char **, char **, char **);

/** name of binary (without path) */
const char *
fpp_config_get_plugin_file_name(void);

/** default name of plugin to display on plugin list page */
const char *
fpp_config_get_plugin_name(void);

/** list of mime types in NPAPI format */
const char *
fpp_config_get_plugin_mime_type(void);

/** list of mime types in NPAPI format */
const char *
fpp_config_get_default_plugin_version(void);

/** default plugin description to display on plugin list page */
const char *
fpp_config_get_default_plugin_descr(void);

const char *
fpp_config_get_pepper_data_dir(void);

const char *
fpp_config_get_pepper_salt_file_name(void);

void
fpp_config_detect_plugin_specific_quirks(void);

/**
 * Given a filename and a list of paths, look for a file with the supplied
 * filename in the list of paths.
 *
 * Returns the fullpath as a new heap-allocated string if found,
 * NULL otherwise */
char *
fpp_config_find_file_among_paths(const char *, const char **);

#endif // FPP_CONFIG_H
