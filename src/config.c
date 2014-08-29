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

#include "config.h"
#include <libconfig.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>


static struct fpp_config_s default_config = {
    .audio_buffer_min_ms = 20,
    .audio_buffer_max_ms = 500,
    .xinerama_screen     = 0,
    .pepperflash_path    = NULL,
    .flash_command_line  = "enable_hw_video_decode=1,enable_stagevideo_auto=1",
    .exp_enable_3d       = 0,
    .quirks = {
        .switch_buttons_2_3         = 0,
        .dump_resource_histogram    = 0,
        .dump_variables             = 0,
        .plugin_missing             = 0,
        .incompatible_npapi_version = 0,
        .x_synchronize              = 0,
    },
};

struct fpp_config_s config = {};
static const char *config_file_name = "freshwrapper.conf";
static const char *config_dir_name = "freshwrapper-data";
static int initialized = 0;
static char *pepper_data_dir;


static
void
initialize_quirks(void)
{
    FILE *fp = fopen("/proc/self/cmdline", "r");
    if (fp) {
        char cmdline[2048];
        size_t len = fread(cmdline, 1, sizeof(cmdline) - 1, fp);
        cmdline[len] = 0;
        if (strstr(cmdline, "operapluginwrapper")) {
            // Opera calls right mouse button "2" instead of correct "3"
            config.quirks.switch_buttons_2_3 = 1;
        }

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

    if (!config_read_file(&cfg, local_config)) {
        if (!config_read_file(&cfg, global_config)) {
            goto quit;
        }
    }

    long long intval;
    const char *stringval;

    if (config_lookup_int64(&cfg, "audio_buffer_min_ms", &intval)) {
        config.audio_buffer_min_ms = intval;
    }

    if (config_lookup_int64(&cfg, "audio_buffer_max_ms", &intval)) {
        config.audio_buffer_max_ms = intval;
    }

    if (config_lookup_int64(&cfg, "xinerama_screen", &intval)) {
        config.xinerama_screen = intval;
    }


    if (config_lookup_string(&cfg, "pepperflash_path", &stringval)) {
        config.pepperflash_path = strdup(stringval);
    }

    if (config_lookup_string(&cfg, "flash_command_line", &stringval)) {
        config.flash_command_line = strdup(stringval);
    }

    if (config_lookup_int64(&cfg, "exp_enable_3d", &intval)) {
        config.exp_enable_3d = intval;
    }

    config_destroy(&cfg);

quit:
    g_free(local_config);
    g_free(global_config);

    initialize_quirks();

    // calculate plugin data directory
    local_config = get_local_config_path(config_dir_name);
    pepper_data_dir = g_strdup_printf("%s/%s", local_config, fpp_config_get_plugin_name());
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
    g_free(pepper_data_dir);
    initialized = 0;
}

const char *
fpp_config_get_pepper_data_dir(void)
{
    return pepper_data_dir;
}
