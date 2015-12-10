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

#include "config.h"
#include "config_parser/config_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <glib.h>
#include "trace_core.h"


static struct fpp_config_s default_config = {
    .audio_buffer_min_ms =      20,
    .audio_buffer_max_ms =      500,
    .audio_use_jack      =      0,
    .jack_autoconnect_ports =   1,
    .jack_server_name =         NULL,
    .jack_autostart_server =    1,
    .pepperflash_path    =      NULL,
    .flash_command_line  =      "",
    .enable_3d           =      1,
    .enable_hwdec        =      0,
    .quiet =                    0,
    .fullscreen_width    =      0,
    .fullscreen_height   =      0,
    .randomize_dns_case =       0,
    .device_scale        =      1.0,
    .enable_windowed_mode   =   1,
    .enable_xembed          =   1,
    .enable_vaapi =             1,
    .enable_vdpau =             1,
    .tie_fullscreen_window_to_browser = 1,
    .vsync_afterwait_us =       0,
    .fs_delay_ms =              300,
    .enable_vsync =             1,
    .double_click_delay_ms =    400,
    .show_version_info =        0,
    .probe_video_capture_devices = 1,
    .enable_xrender =           1,
    .quirks = {
        .connect_first_loader_to_unrequested_stream = 0,
        .dump_resource_histogram    = 0,
        .dump_variables             = 0,
        .plasma5_screensaver        = 0,
        .plugin_missing             = 0,
        .incompatible_npapi_version = 0,
        .x_synchronize              = 0,
        .avoid_stdout               = 0,
    },
};

static cfg_opt_t opts[] = {
    CFG_SIMPLE_INT("audio_buffer_min_ms",    &config.audio_buffer_min_ms),
    CFG_SIMPLE_INT("audio_buffer_max_ms",    &config.audio_buffer_max_ms),
    CFG_SIMPLE_INT("audio_use_jack",         &config.audio_use_jack),
    CFG_SIMPLE_INT("jack_autoconnect_ports", &config.jack_autoconnect_ports),
    CFG_SIMPLE_STR("jack_server_name",       &config.jack_server_name),
    CFG_SIMPLE_INT("jack_autostart_server",  &config.jack_autostart_server),
    CFG_SIMPLE_STR("pepperflash_path",       &config.pepperflash_path),
    CFG_SIMPLE_STR("flash_command_line",     &config.flash_command_line),
    CFG_SIMPLE_INT("enable_3d",              &config.enable_3d),
    CFG_SIMPLE_INT("enable_hwdec",           &config.enable_hwdec),
    CFG_SIMPLE_INT("quiet",                  &config.quiet),
    CFG_SIMPLE_INT("fullscreen_width",       &config.fullscreen_width),
    CFG_SIMPLE_INT("fullscreen_height",      &config.fullscreen_height),
    CFG_SIMPLE_INT("randomize_dns_case",     &config.randomize_dns_case),
    CFG_SIMPLE_FLOAT("device_scale",         &config.device_scale),
    CFG_SIMPLE_INT("enable_windowed_mode",   &config.enable_windowed_mode),
    CFG_SIMPLE_INT("enable_xembed",          &config.enable_xembed),
    CFG_SIMPLE_INT("enable_vaapi",           &config.enable_vaapi),
    CFG_SIMPLE_INT("enable_vdpau",           &config.enable_vdpau),
    CFG_SIMPLE_INT("vsync_afterwait_us",     &config.vsync_afterwait_us),
    CFG_SIMPLE_INT("fs_delay_ms",            &config.fs_delay_ms),
    CFG_SIMPLE_INT("enable_vsync",           &config.enable_vsync),
    CFG_SIMPLE_INT("tie_fullscreen_window_to_browser", &config.tie_fullscreen_window_to_browser),
    CFG_SIMPLE_INT("quirk_plasma5_screensaver",        &config.quirks.plasma5_screensaver),
    CFG_SIMPLE_INT("double_click_delay_ms",  &config.double_click_delay_ms),
    CFG_SIMPLE_INT("show_version_info",      &config.show_version_info),
    CFG_SIMPLE_INT("probe_video_capture_devices", &config.probe_video_capture_devices),
    CFG_SIMPLE_INT("enable_xrender",         &config.enable_xrender),
    CFG_END()
};

struct fpp_config_s config = {};
static const char *salt_file_name = "salt.dat";
static const char *config_file_name = "freshwrapper.conf";
static const char *config_dir_name = "freshwrapper-data";
static int initialized = 0;
static char *pepper_data_dir;
static char *pepper_salt_file_name;


static
void
initialize_quirks(void)
{
    fpp_config_detect_plugin_specific_quirks();

    // Webkit2gtk uses stdout to pass plugin name from plugin container to web renderer process.
    // So stdout should be avoided if possible.
    FILE *fp = fopen("/proc/self/cmdline", "r");
    if (fp) {
        char buf[2048];
        size_t read_bytes = fread(buf, 1, sizeof(buf) - 1, fp);

        if (read_bytes > 0)
            if (strstr(buf, "WebKitPluginProcess"))
                config.quirks.avoid_stdout = 1;
        fclose(fp);
    }
}

static
char *
get_local_config_path(const char *file_name)
{
    char       *res = NULL;
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");

    if (xdg_config_home) {
        res = g_strdup_printf("%s/%s", xdg_config_home, file_name);
    } else {
        const char *home = getenv("HOME");
        res = g_strdup_printf("%s/.config/%s", home ? home : "", file_name);
    }

    return res;
}

static
char *
get_global_config_path(const char *file_name)
{
    return g_strdup_printf("/etc/%s", file_name);
}

static
void
error_report_func(const char *s)
{
    trace_error("config: %s\n", s);
}

void
fpp_config_initialize(void)
{
    if (initialized)
        return;

    cfg_t  *cfg;
    char   *local_config = get_local_config_path(config_file_name);
    char   *global_config = get_global_config_path(config_file_name);

    config = default_config;

#define DUP_CFG_STRING(field) do { if (field) field = strdup(field); } while (0)

    // all non-NULL strings values need to be duplicated, since config parser free()'s
    // their previous versions
    DUP_CFG_STRING(config.jack_server_name);
    DUP_CFG_STRING(config.pepperflash_path);
    DUP_CFG_STRING(config.flash_command_line);

    // Setting locale to "C" changes decimal mark to point for floating point values
    setlocale(LC_ALL, "C");

    cfg = cfg_init(opts, 0);
    cfg_set_error_func(cfg, error_report_func);
    if (cfg_parse(cfg, local_config) != CFG_SUCCESS) {
        trace_info_f("can't open configuration file %s, trying %s\n", local_config, global_config);

        if (cfg_parse(cfg, global_config) != CFG_SUCCESS) {
            trace_info_f("can't open configuration file %s, using default values\n", global_config);
            config = default_config;
            goto quit;
        }
    }

    cfg_free(cfg);

quit:

    // restore locale
    setlocale(LC_ALL, "");

    g_free(local_config);
    g_free(global_config);

    initialize_quirks();

    // calculate plugin data directory
    local_config = get_local_config_path(config_dir_name);
    pepper_data_dir = g_strdup_printf("%s/%s", local_config, fpp_config_get_plugin_name());
    pepper_salt_file_name = g_strdup_printf("%s/%s", local_config, salt_file_name);
    g_free(local_config);

    initialized = 1;
}

void
fpp_config_destroy(void)
{
#define FREE_IF_CHANGED(field)                          \
    do {                                                \
        if (config.field != default_config.field)       \
            free(config.field);                         \
    } while (0)

    if (!initialized)
        return;

    FREE_IF_CHANGED(pepperflash_path);
    FREE_IF_CHANGED(flash_command_line);
    FREE_IF_CHANGED(jack_server_name);
    g_free(pepper_data_dir);
    g_free(pepper_salt_file_name);
    initialized = 0;
}

const char *
fpp_config_get_pepper_data_dir(void)
{
    return pepper_data_dir;
}

const char *
fpp_config_get_pepper_salt_file_name(void)
{
    return pepper_salt_file_name;
}
