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
#include <libconfig.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>


static struct fpp_config_s default_config = {
    .audio_buffer_min_ms =      20,
    .audio_buffer_max_ms =      500,
    .audio_use_jack      =      0,
    .jack_autoconnect_ports =   1,
    .jack_server_name =         NULL,
    .jack_autostart_server =    1,
    .pepperflash_path    =      NULL,
    .flash_command_line  =      "", // "enable_hw_video_decode=1,enable_stagevideo_auto=1",
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
    .quirks = {
        .connect_first_loader_to_unrequested_stream = 0,
        .dump_resource_histogram    = 0,
        .dump_variables             = 0,
        .plasma5_screensaver        = 0,
        .plugin_missing             = 0,
        .incompatible_npapi_version = 0,
        .x_synchronize              = 0,
    },
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
get_int(const config_t *cfg, const char *name, int *result)
{
    long long intval;
    if (config_lookup_int64(cfg, name, &intval))
        *result = intval;
}

static
void
get_string(const config_t *cfg, const char *name, char **result)
{
    const char *stringval;
    if (config_lookup_string(cfg, name, &stringval))
        *result = strdup(stringval);
}

static
void
get_double(const config_t *cfg, const char *name, double *result)
{
    double doubleval;
    if (config_lookup_float(cfg, name, &doubleval))
        *result = doubleval;
}

void
fpp_config_initialize(void)
{
    if (initialized)
        return;

    config_t    cfg;
    char       *local_config = get_local_config_path(config_file_name);
    char       *global_config = get_global_config_path(config_file_name);

    config = default_config;

    config_init(&cfg);
    config_set_auto_convert(&cfg, 1);

    if (!config_read_file(&cfg, local_config)) {
        if (!config_read_file(&cfg, global_config)) {
            goto quit;
        }
    }

    get_int(&cfg, "audio_buffer_min_ms", &config.audio_buffer_min_ms);
    get_int(&cfg, "audio_buffer_max_ms", &config.audio_buffer_max_ms);
    get_int(&cfg, "audio_use_jack", &config.audio_use_jack);
    get_int(&cfg, "jack_autoconnect_ports", &config.jack_autoconnect_ports);
    get_int(&cfg, "jack_autostart_server", &config.jack_autostart_server);
    get_int(&cfg, "enable_3d", &config.enable_3d);
    get_int(&cfg, "enable_hwdec", &config.enable_hwdec);
    get_int(&cfg, "quiet", &config.quiet);
    get_int(&cfg, "fullscreen_width", &config.fullscreen_width);
    get_int(&cfg, "fullscreen_height", &config.fullscreen_height);
    get_int(&cfg, "randomize_dns_case", &config.randomize_dns_case);
    get_int(&cfg, "quirk_plasma5_screensaver", &config.quirks.plasma5_screensaver);
    get_int(&cfg, "enable_windowed_mode", &config.enable_windowed_mode);
    get_int(&cfg, "enable_xembed", &config.enable_xembed);
    get_int(&cfg, "enable_vaapi", &config.enable_vaapi);
    get_int(&cfg, "enable_vdpau", &config.enable_vdpau);
    get_int(&cfg, "tie_fullscreen_window_to_browser", &config.tie_fullscreen_window_to_browser);
    get_int(&cfg, "vsync_afterwait_us", &config.vsync_afterwait_us);
    get_int(&cfg, "fs_delay_ms", &config.fs_delay_ms);
    get_int(&cfg, "enable_vsync", &config.enable_vsync);

    get_string(&cfg, "pepperflash_path", &config.pepperflash_path);
    get_string(&cfg, "flash_command_line", &config.flash_command_line);
    get_string(&cfg, "jack_server_name", &config.jack_server_name);

    get_double(&cfg, "device_scale", &config.device_scale);

    config_destroy(&cfg);

quit:
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
