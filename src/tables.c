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
#include "trace.h"


NPNetscapeFuncs npn;

// URL/urlloader resource mapping
struct url_pair_s {
    const char *url;
    PP_Resource resource;
};

static GHashTable  *var_ht;
static GHashTable  *pp_to_np_ht;
static GList       *url_pair_list;
static GHashTable  *npp_ht;

static
void
__attribute__((constructor))
constructor_tables(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pp_to_np_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    npp_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    url_pair_list = NULL;
}

static
void
__attribute__((destructor))
destructor_tables(void)
{
    g_hash_table_unref(var_ht);
    g_hash_table_unref(pp_to_np_ht);
    g_hash_table_unref(npp_ht);
    g_list_free(url_pair_list);
}

int
tables_ref_var(struct PP_Var var)
{
    void *key = (void*)(size_t)var.value.as_id;
    int ref_cnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, key));

    if (ref_cnt > 0) {
        ref_cnt ++;
        g_hash_table_replace(var_ht, key, GINT_TO_POINTER(ref_cnt));
    } else {
        ref_cnt = 1;
        g_hash_table_insert(var_ht, key, GINT_TO_POINTER(1));
    }

    return ref_cnt;
}

int
tables_unref_var(struct PP_Var var)
{
    void *key = (void*)(size_t)var.value.as_id;
    int ref_cnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, key));

    if (ref_cnt > 1) {
        ref_cnt --;
        g_hash_table_replace(var_ht, key, GINT_TO_POINTER(ref_cnt));
    } else {
        ref_cnt = 0;
        g_hash_table_remove(var_ht, key);
    }
    return ref_cnt;
}

struct pp_instance_s *
tables_get_pp_instance(PP_Instance instance)
{
    return g_hash_table_lookup(pp_to_np_ht, GINT_TO_POINTER(instance));
}

void
tables_add_pp_instance(PP_Instance instance, struct pp_instance_s *pp_i)
{
    g_hash_table_replace(pp_to_np_ht, GINT_TO_POINTER(instance), pp_i);
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
            url_pair_list = g_list_delete_link(url_pair_list, ptr);
            return ret;
        }
        ptr = g_list_next(ptr);
    }

    trace_warning("%s, no corresponding resource in tables for url=\"%s\"\n", __func__, url);
    return 0;
}

struct PP_Var
PP_MakeString(const char *s)
{
    return PP_MakeStringN(s, strlen(s));
}

struct PP_Var
PP_MakeStringN(const char *s, unsigned int len)
{
    // strings are stored in format   [size]    [data...]
    //                              (uint32_t)   (char)...
    // where (size) is uint32_t string length, and (data) is actual
    // string contents with appended '\0'

    char *ptr = malloc(sizeof(uint32_t) + len + 1);
    *(uint32_t *)ptr = len;
    memcpy(ptr + sizeof(uint32_t), s, len);
    ptr[len + sizeof(uint32_t)] = 0;

    struct PP_Var var;
    var.type = PP_VARTYPE_STRING;
    var.value.as_id = (size_t)(void *)ptr;
    tables_ref_var(var);
    return var;
}


struct PP_Var
PP_MakeBrowserObject(void *data, const struct pp_var_object_s *reference_obj)
{
    struct PP_Var var;
    struct pp_var_object_s *obj;

    obj = malloc(sizeof(*obj));
    obj->klass = reference_obj->klass;
    obj->npp = reference_obj->npp;
    obj->data = data;
    var.type = PP_VARTYPE_OBJECT;
    var.value.as_id = (size_t)(void *)obj;
    tables_ref_var(var);
    return var;
}

struct PP_Var
np_variant_to_pp_var(NPVariant v, const struct pp_var_object_s *reference_obj)
{
    switch (v.type) {
    case NPVariantType_Void:    return PP_MakeUndefined();
    case NPVariantType_Null:    return PP_MakeNull();
    case NPVariantType_Bool:    return PP_MakeBool(v.value.boolValue);
    case NPVariantType_Int32:   return PP_MakeInt32(v.value.intValue);
    case NPVariantType_Double:  return PP_MakeDouble(v.value.doubleValue);
    case NPVariantType_String:  return PP_MakeStringN(v.value.stringValue.UTF8Characters,
                                                      v.value.stringValue.UTF8Length);
    case NPVariantType_Object:  return PP_MakeBrowserObject(v.value.objectValue, reference_obj);
    default:                    return PP_MakeUndefined();
    }
}

void
tables_add_npp_instance(NPP npp)
{
    g_hash_table_insert(npp_ht, (gpointer)npp, GINT_TO_POINTER(1));
}

void
tables_remove_npp_instance(NPP npp)
{
    g_hash_table_remove(npp_ht, (gpointer)npp);
}

NPP
tables_get_some_npp_instance(void)
{
    gpointer key, val;
    GHashTableIter iter;

    g_hash_table_iter_init(&iter, npp_ht);
    g_hash_table_iter_next(&iter, &key, &val);

    return key;
}
