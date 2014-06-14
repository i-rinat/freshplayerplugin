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
    .plugin_path         = "/opt/google/chrome/PepperFlash/libpepflashplayer.so",
    .flash_command_line  = "enable_hw_video_decode=1,enable_stagevideo_auto=1",
    .exp_enable_3d       = 0,
    .quirks = {
        .switch_buttons_2_3      = 0,
        .dump_resource_histogram = 0,
        .plugin_missing          = 0,
    },
};

struct fpp_config_s config = {};
static const char *config_file_name = "freshwrapper.conf";


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
get_local_config_path(void)
{
    char       *res = NULL;
    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");

    if (xdg_config_home) {
        res = g_strdup_printf("%s/%s", xdg_config_home, config_file_name);
    } else {
        const char *home = getenv("HOME");
        res = g_strdup_printf("%s/.config/%s", home ? home : "", config_file_name);
    }

    return res;
}

static
char *
get_global_config_path(void)
{
    return g_strdup_printf("/etc/%s", config_file_name);
}

void
fpp_config_initialize(void)
{
    config_t    cfg;
    char       *local_config = get_local_config_path();
    char       *global_config = get_global_config_path();

    config = default_config;

    config_init(&cfg);

    if (!config_read_file(&cfg, local_config)) {
        if (!config_read_file(&cfg, global_config)) {
            goto quit;
        }
    }

    int intval;
    if (config_lookup_int(&cfg, "audio_buffer_min_ms", &intval)) {
        config.audio_buffer_min_ms = intval;
    }

    if (config_lookup_int(&cfg, "audio_buffer_max_ms", &intval)) {
        config.audio_buffer_max_ms = intval;
    }

    if (config_lookup_int(&cfg, "xinerama_screen", &intval)) {
        config.xinerama_screen = intval;
    }

    const char *stringval;
    if (config_lookup_string(&cfg, "plugin_path", &stringval)) {
        config.plugin_path = strdup(stringval);
    }

    if (config_lookup_string(&cfg, "flash_command_line", &stringval)) {
        config.flash_command_line = strdup(stringval);
    }

    if (config_lookup_int(&cfg, "exp_enable_3d", &intval)) {
        config.exp_enable_3d = intval;
    }

quit:
    config_destroy(&cfg);
    g_free(local_config);
    g_free(global_config);

    initialize_quirks();
}

void
fpp_config_destroy(void)
{
#define FREE_IF_CHANGED(field)                          \
    do {                                                \
        if (config.field != default_config.field)       \
            free(config.field);                         \
    } while (0)

    FREE_IF_CHANGED(plugin_path);
    FREE_IF_CHANGED(flash_command_line);
}
