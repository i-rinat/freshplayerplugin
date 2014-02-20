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

#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "tables.h"


// URL/urlloader resource mapping
struct url_pair_s {
    const char *url;
    PP_Resource resource;
};

static GHashTable  *var_ht;
static GHashTable  *pp_to_np_ht;
static GList       *url_pair_list;

static
void
__attribute__((constructor))
constructor_tables(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pp_to_np_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    url_pair_list = NULL;
}

static
void
__attribute__((destructor))
destructor_tables(void)
{
    g_hash_table_unref(var_ht);
    g_hash_table_unref(pp_to_np_ht);
    g_list_free(url_pair_list);
}

int
tables_ref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt + 1));
    return refcnt + 1;
}

int
tables_unref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt - 1));
    return refcnt - 1;
}

struct np_priv_s *
tables_pp_instance_to_np_priv(PP_Instance instance)
{
    return g_hash_table_lookup(pp_to_np_ht, GINT_TO_POINTER(instance));
}

void
tables_add_pp_np_mapping(PP_Instance instance, struct np_priv_s *priv)
{
    g_hash_table_replace(pp_to_np_ht, GINT_TO_POINTER(instance), priv);
}

void               
tables_push_url_pair(const char *url, PP_Resource resource)
{
    struct url_pair_s *pair = malloc(sizeof(*pair));
    pair->url = url;
    pair->resource = resource;
    url_pair_list = g_list_append(url_pair_list, pair);
}

PP_Resource
tables_pop_url_pair(const char *url)
{
    GList *ptr = g_list_first(url_pair_list);
    while (ptr) {
        struct url_pair_s *pair = ptr->data;

        if (!strcmp(pair->url, url)) {
            PP_Resource ret = pair->resource;
            url_pair_list = g_list_remove_link(url_pair_list, ptr);
            g_list_free(ptr);
            return ret;
        }
        ptr = g_list_next(ptr);
    }

    return 0;
}
