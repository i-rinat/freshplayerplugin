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

/** name of binary (without path) */
const char *
fpp_config_get_plugin_file_name(void);

/** default name of plugin to display on plugin list page */
const char *
fpp_config_get_plugin_name(void);

/** list of mime types in NPAPI format */
const char *
fpp_config_get_plugin_mime_type(void);

const char *
fpp_config_get_plugin_version(void);

const char *
fpp_config_get_plugin_descr(void);

/** return full path to a plugin if set */
char *
fpp_config_get_plugin_path(void);

void
fpp_config_detect_plugin_specific_quirks(void);

void
fpp_config_find_backend_plugin(void);
