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

enum {
    CFG_TYPE_UNKNOWN,
    CFG_TYPE_INT,
    CFG_TYPE_STRING,
    CFG_TYPE_FLOAT,
};

enum {
    CFG_SUCCESS,
    CFG_FAILURE,
};

#define CFG_SIMPLE_INT(a_name, a_ptr)   { .name = a_name, .ptr = a_ptr, .type = CFG_TYPE_INT}
#define CFG_SIMPLE_STR(a_name, a_ptr)   { .name = a_name, .ptr = a_ptr, .type = CFG_TYPE_STRING}
#define CFG_SIMPLE_FLOAT(a_name, a_ptr) { .name = a_name, .ptr = a_ptr, .type = CFG_TYPE_FLOAT}
#define CFG_END()                       { .name = NULL, .ptr = NULL, .type = CFG_TYPE_UNKNOWN}

typedef struct {
    const char *name;
    void       *ptr;
    int         type;
} cfg_opt_t;

typedef struct cfg_s cfg_t;

typedef void (cfg_report_error_f)(const char *error_string);

cfg_t *
cfg_init(cfg_opt_t *opts, unsigned int flags);

int
cfg_parse(cfg_t *cfg, const char *filename);

void
cfg_parse_string(cfg_t *cfg, const char *s);

void
cfg_free(cfg_t *cfg);

void
cfg_set_error_func(cfg_t *cfg, cfg_report_error_f func);
