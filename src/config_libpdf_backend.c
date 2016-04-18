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
#include <glib.h>
#include <stdlib.h>

static
const char *
plugin_path_list[] = {
    "/opt/google/chrome",           // Chrome
    "/opt/google/chrome-beta",      // Chrome beta
    "/opt/google/chrome-unstable",  // Chrome unstable
    "/usr/lib64/chromium",          // Chromium (Slackware)
    "/usr/lib/chromium",            // Chromium (Slackware)
    NULL
};

void
fpp_config_get_plugin(char **path, char **desc, char **version)
{
    g_free(*path);
    *path = fpp_config_find_file_among_paths(fpp_config_get_plugin_file_name(), plugin_path_list);

    *desc = (char *)fpp_config_get_default_plugin_descr();
    *version = (char *)fpp_config_get_default_plugin_version();
}


const char *
fpp_config_get_default_plugin_version(void)
{
    return "1.0.0.0";
}

const char *
fpp_config_get_plugin_name(void)
{
    return "libpdf.so renderer backend";
}

const char *
fpp_config_get_default_plugin_descr(void)
{
    return "libpdf.so renderer backend";
}

const char *
fpp_config_get_plugin_mime_type(void)
{
    return "application/x-freshwrapper-libpdf-so::libpdf renderer";
}

const char *
fpp_config_get_plugin_file_name(void)
{
    return "libpdf.so";
}

void
fpp_config_detect_plugin_specific_quirks(void)
{
}
