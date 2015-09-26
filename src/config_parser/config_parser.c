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

#include "config_parser.h"
#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct cfg_s {
    GHashTable         *entries_ht;
    cfg_report_error_f *error_func;
};

struct entry {
    int   type;
    void *ptr;
};


cfg_t *
cfg_init(cfg_opt_t *opts, unsigned int flags)
{
    cfg_t *cfg = calloc(sizeof(*cfg), 1);
    if (!cfg)
        goto point_1;

    cfg->entries_ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    if (!cfg->entries_ht)
        goto point_2;

    cfg_opt_t *cur = opts;
    while (cur->name) {
        cfg_opt_t *e = g_malloc0(sizeof(*e));

        if (!e)
            goto point_3;

        e->type = cur->type;
        e->ptr = cur->ptr;

        g_hash_table_insert(cfg->entries_ht, g_strdup(cur->name), e);

        cur++;
    }

    return cfg;

point_3:
    g_hash_table_destroy(cfg->entries_ht);
point_2:
    free(cfg);
point_1:
    return NULL;

}

int
cfg_parse(cfg_t *cfg, const char *filename)
{
    GError *err = NULL;
    gchar *s;

    if (g_file_get_contents(filename, &s, NULL, &err) == FALSE)
        return CFG_FAILURE;

    cfg_parse_string(cfg, s);
    g_free(s);

    return CFG_SUCCESS;
}

static
void
handle_param(cfg_t *cfg, const char *ns, const char *ne, const char *vs, const char *ve)
{
    char   *name = g_strndup(ns, ne - ns);
    char   *value = g_strndup(vs, ve - vs);
    char  **str_ptr;
    int    *int_ptr;
    double *double_ptr;

    cfg_opt_t *e = g_hash_table_lookup(cfg->entries_ht, name);

    if (!e) {
        if (cfg->error_func) {
            gchar *s = g_strdup_printf("unknown parameter: %s = %s", name, value);
            cfg->error_func(s);
            g_free(s);
        }
        goto done;
    }

    switch (e->type) {
    case CFG_TYPE_INT:
        int_ptr = e->ptr;
        *int_ptr = atoi(value);
        break;

    case CFG_TYPE_FLOAT:
        double_ptr = e->ptr;
        *double_ptr = atof(value);
        break;

    case CFG_TYPE_STRING:
        str_ptr = e->ptr;
        free(*str_ptr);
        *str_ptr = g_strdup(value);
        break;

    default:
        if (cfg->error_func) {
            gchar *s = g_strdup_printf("parameter of unknown type %d: %s = %s", e->type, name,
                                       value);
            cfg->error_func(s);
            g_free(s);
        }
        break;
    }

done:
    g_free(name);
    g_free(value);
}

void
cfg_parse_string(cfg_t *cfg, const char *s)
{
    int cs;
    const char *p = s;
    const char *pe = p + strlen(s);
    const char *eof = pe;

    struct {
        const char *name_start;
        const char *name_end;
        const char *val_start;
        const char *val_end;
    } param;

#include <config_parser_fsm.c>

    (void)config_parser_en_main;
    (void)config_parser_error;
    (void)config_parser_first_final;
}

void
cfg_free(cfg_t *cfg)
{
    g_hash_table_destroy(cfg->entries_ht);
    free(cfg);
}

void
cfg_set_error_func(cfg_t *cfg, cfg_report_error_f func)
{
    cfg->error_func = func;
}
