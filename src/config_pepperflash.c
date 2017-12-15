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

#define _XOPEN_SOURCE 500
#include "config.h"
#include "trace_core.h"
#include <glib.h>
#include <libgen.h>
#include <parson/parson.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <wordexp.h>

struct flapper_version {
    unsigned int v[4];
};

static char *flapper_path = NULL;
static struct flapper_version flapper_version;
static char *flapper_descr = NULL;
static char *flapper_str_version = NULL;

static const char *pepperflash_path_list[] = {
    // Chrome
    "/opt/google/chrome/PepperFlash/libpepflashplayer.so",

    // Adobe package
    "/usr/lib/flash-plugin/libpepflashplayer.so",
    "/usr/lib64/flash-plugin/libpepflashplayer.so",

    // adobe-flashplugin (Ubuntu)
    "/usr/lib/adobe-flashplugin/libpepflashplayer.so",

    // pepperflashplugin-nonfree (Debian)
    "/usr/lib/pepperflashplugin-nonfree/libpepflashplayer.so",

    // chromium-pepperflash-plugin (Slackware)
    "/usr/lib/PepperFlash/libpepflashplayer.so",
    "/usr/lib64/PepperFlash/libpepflashplayer.so",

    // chrome-binary-plugins (Gentoo/Sabayon)
    "/usr/lib/chromium-browser/PepperFlash/libpepflashplayer.so",
    "/usr/lib64/chromium-browser/PepperFlash/libpepflashplayer.so",

    // chromium-pepper-flash (Old Build) (CentOS/Read Hat/Fedora)
    "/usr/lib/chromium/PepperFlash/libpepflashplayer.so",
    "/usr/lib64/chromium/PepperFlash/libpepflashplayer.so",

    // chromium-pepper-flash (New Build) (CentOS/Read Hat/Fedora)
    "/opt/chromium/PepperFlash/libpepflashplayer.so",

    // pepflashplugin-installer (Ubuntu)
    "/usr/lib/pepflashplugin-installer/libpepflashplayer.so",

    // Chrome component update
    "~/.config/google-chrome/PepperFlash/*/libpepflashplayer.so",

    NULL,
};

__attribute__((destructor))
static void
destructor_fpp_config_pepper_flash(void)
{
    free(flapper_path);
    g_free(flapper_descr);
    g_free(flapper_str_version);
}

static int
file_exists_and_is_regular_and_readable(const char *fname)
{
    struct stat sb;
    int ret = lstat(fname, &sb);

    // should exist
    if (ret != 0)
        return 0;

    // should be a regular file
    if (!S_ISREG(sb.st_mode))
        return 0;

    // should be readable
    if (!(sb.st_mode & 0444))
        return 0;

    return 1;
}

static struct flapper_version
get_flapper_version(const char *fname)
{
    struct flapper_version v = {};

    // Try manifest.json file.
    char *manifest_dir = strdup(fname);
    gchar *manifest_path = g_strdup_printf("%s/manifest.json", dirname(manifest_dir));
    free(manifest_dir);

    JSON_Value *root_val = json_parse_file(manifest_path);
    g_free(manifest_path);

    if (!root_val) {
        // There are either no such file, or it's not in JSON format.
        goto done;
    }

    JSON_Object *root_obj = json_value_get_object(root_val);
    const char *version = json_object_get_string(root_obj, "version");
    if (version) {
        (void)sscanf(version, "%9u.%9u.%9u.%9u", &v.v[0], &v.v[1], &v.v[2], &v.v[3]);
    }

    json_value_free(root_val);

done:
    return v;
}

static int
compare_flapper_version(const struct flapper_version *a, const struct flapper_version *b)
{
    for (int k = 0; k < 4; k++) {
        if (a->v[k] < b->v[k])
            return -1;

        if (a->v[k] > b->v[k])
            return 1;
    }

    return 0;
}

static uintptr_t
probe_flapper(const char *fname)
{
    if (!file_exists_and_is_regular_and_readable(fname))
        return 1;

    struct flapper_version v = get_flapper_version(fname);

    trace_info_f("found %s, version %u.%u.%u.%u\n", fname, v.v[0], v.v[1], v.v[2], v.v[3]);

    if (!flapper_path || compare_flapper_version(&v, &flapper_version) > 0) {
        trace_info_f("%s has newer version\n", fname);
        free(flapper_path);
        flapper_path = strdup(fname);
        flapper_version = v;
        return 0;
    }

    return 0;
}

void
fpp_config_find_backend_plugin(void)
{
    if (config.pepperflash_path) {
        // First, search in user supplied paths.
        gchar **paths = g_strsplit(config.pepperflash_path, ":", -1);
        for (gchar **it = paths; *it != NULL; it++) {
            probe_flapper(*it);
        }
        g_strfreev(paths);

    } else {
        for (const char **it = pepperflash_path_list; *it != NULL; it++) {
            wordexp_t p;

            if (wordexp(*it, &p, WRDE_NOCMD) == 0) {
                const char *fname = p.we_wordv[0];
                trace_info_f("probing %s\n", fname);
                probe_flapper(fname);
            }
        }
    }

    flapper_descr = g_strdup_printf("Shockwave Flash %u.%u r%u", flapper_version.v[0],
                                    flapper_version.v[1], flapper_version.v[2]);

    flapper_str_version = g_strdup_printf("%u.%u.%u.%u", flapper_version.v[0], flapper_version.v[1],
                                          flapper_version.v[2], flapper_version.v[3]);
}

const char *
fpp_config_get_plugin_version(void)
{
    return flapper_str_version;
}

const char *
fpp_config_get_plugin_name(void)
{
    return "Shockwave Flash";
}

const char *
fpp_config_get_plugin_descr(void)
{
    return flapper_descr;
}

const char *
fpp_config_get_plugin_mime_type(void)
{
    return "application/x-shockwave-flash:swf:Shockwave Flash;"
           "application/futuresplash:spl:FutureSplash Player";
}

char *
fpp_config_get_plugin_path(void)
{
    return flapper_path;
}

const char *
fpp_config_get_plugin_file_name(void)
{
    return "libpepflashplayer.so";
}

void
fpp_config_detect_plugin_specific_quirks(void)
{
    config.quirks.connect_first_loader_to_unrequested_stream = 1;
}
