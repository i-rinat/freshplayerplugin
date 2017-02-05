/*
 * Copyright © 2013-2016  Rinat Ibragimov
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
#include "config_pepperflash.h"
#include <glib.h>
#include <parson/parson.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>

gchar **pepperflash_path_list = NULL; // Will be set from config.pepperflash_path

const char *pepperflash_system_path_list[] = {
    "/opt/google/chrome/PepperFlash",           // Chrome
    "/opt/google/chrome-beta/PepperFlash",      // Chrome beta
    "/opt/google/chrome-unstable/PepperFlash",  // Chrome unstable
    "/usr/lib/flash-plugin",                    // Adobe package
    "/usr/lib64/flash-plugin",                  // Adobe package
    "/usr/lib/adobe-flashplugin",               // adobe-flashplugin (Ubuntu)
    "/usr/lib/pepperflashplugin-nonfree",       // pepperflashplugin-nonfree (Debian)
    "/usr/lib/PepperFlash",                     // chromium-pepperflash-plugin (Slackware)
    "/usr/lib64/PepperFlash",                   // chromium-pepperflash-plugin (Slackware)
    "/usr/lib/chromium-browser/PepperFlash",    // chrome-binary-plugins (Gentoo/Sabayon)
    "/usr/lib64/chromium-browser/PepperFlash",  // chrome-binary-plugins (Gentoo/Sabayon)
    "/usr/lib/chromium/PepperFlash",            // chromium-pepper-flash (Old Build) (CentOS/Read Hat/Fedora)
    "/usr/lib64/chromium/PepperFlash",          // chromium-pepper-flash (Old Build) (CentOS/Read Hat/Fedora)
    "/opt/chromium/PepperFlash",                // chromium-pepper-flash (New Build) (CentOS/Read Hat/Fedora)
    "/usr/lib/pepflashplugin-installer",        // pepflashplugin-installer (Ubuntu)
    NULL,
};

const char *pepperflash_user_path_list[] = {
    "$XDG_CONFIG_DIR/google-chrome/PepperFlash",
    "$XDG_CONFIG_DIR/google-chrome-beta/PepperFlash",
    "$XDG_CONFIG_DIR/google-chrome-unstable/PepperFlash",
    NULL
};

void
fpp_config_plugin_initialize()
{
  if (!config.pepperflash_path) return;
  pepperflash_path_list = g_strsplit(config.pepperflash_path, ":", 0);
  // Convert file paths to directory paths
  char **path = pepperflash_path_list;
  char *temp_path;
  int len = 0;
  while (*path) {
    if (!g_file_test(*path, G_FILE_TEST_IS_DIR)) {
      temp_path = g_strdup(dirname(*path));
      g_free(*path);
      *path = temp_path;
    }

    len++;
    path++;
  }

  // Flip order of paths, for backwards-compatibility
  path = pepperflash_path_list;
  for (int i = 0; i < len/2; i++) {
    temp_path = path[i];
    path[i] = path[len - i -1];
    path[len -i -1] = temp_path;
  }
}

void
fpp_config_get_plugin(char **path, char **desc, char **version)
{
    struct version latest_version = {0,0,0,0};
    const char *latest_version_path = NULL;

    if (config.pepperflash_path) {
      latest_version_path = fpp_config_find_file_among_paths(
                                fpp_config_get_plugin_file_name(),
                                (const char **) pepperflash_path_list);
      is_newer_version(latest_version_path, &latest_version); //Grab version
      g_free(*path);
      *path = (char *)latest_version_path;
    } else {
      for(const char **path = pepperflash_system_path_list; *path; path++) {
        if (is_newer_version(*path, &latest_version) > 0) {
          latest_version_path = *path;
        }
      }

      g_free(*path);
      *path = g_strdup_printf("%s/%s", latest_version_path, fpp_config_get_plugin_file_name());
      fprintf(stderr, "fresh: using path: %s\n", *path);
    }

    if (latest_version_path == NULL) return;

    g_free(*desc);
    *desc = g_strdup_printf("%s %d.%d r%d.%d", fpp_config_get_plugin_name(),
            latest_version.v1, latest_version.v2, latest_version.v3, latest_version.v4);

    g_free(*version);
    *version = g_strdup_printf("%d.%d.%d.%d", latest_version.v1, latest_version.v2,
            latest_version.v3, latest_version.v4);
}

const char *
fpp_config_get_default_plugin_version(void)
{
    return "24.0.0.186";
}

const char *
fpp_config_get_plugin_name(void)
{
    return "Shockwave Flash";
}

const char *
fpp_config_get_default_plugin_descr(void)
{
    return "Shockwave Flash 24.0 r0";
}

const char *
fpp_config_get_plugin_mime_type(void)
{
    return "application/x-shockwave-flash:swf:Shockwave Flash;"
           "application/futuresplash:spl:FutureSplash Player";
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

/**
 * Takes a pointer to a path, and a pointer to a version. Looks for a manifest.json at path,
 * and sets the version to version from path if the latter is found to be newer.
 *
 * Returns positive if newer version is found, -1 if no valid manifest.json file is found at
 * path or an older version is found
 */
int
is_newer_version(const char *path, version *old) {
    gchar *manifest_path = g_strdup_printf("%s/manifest.json", path);
    JSON_Value *root_val = json_parse_file(manifest_path);
    g_free(manifest_path);
    if (!root_val) return -1;

    JSON_Object *root_obj = json_value_get_object(root_val);
    const char *version = json_object_get_string(root_obj, "version");
    if (!version) return -1;

    int *new = (int *) malloc(sizeof(struct version));
    sscanf(version, "%d.%d.%d.%d", new, new+1, new+2, new+3);

    for (int i = 0; i < 4; i++) {
        if (new[i] > ((int *)old)[i]) goto newer;
    }

    return -1;

newer:
    *old = *(struct version *)new;
    free(new);
    return 1;
}
