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
#include "ppb_var.h"
#include "p2n_proxy_class.h"
#include "n2p_proxy_class.h"


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
static GHashTable  *npobj_to_npp_ht = NULL;     // NPObject-to-NPP mapping

static PangoContext *pango_ctx = NULL;
static PangoFontMap *pango_fm = NULL;

static
void
__attribute__((constructor))
constructor_tables(void)
{
    var_ht =            g_hash_table_new(g_direct_hash, g_direct_equal);
    pp_to_np_ht =       g_hash_table_new(g_direct_hash, g_direct_equal);
    npp_ht =            g_hash_table_new(g_direct_hash, g_direct_equal);
    npobj_to_npp_ht =   g_hash_table_new(g_direct_hash, g_direct_equal);
    url_pair_list = NULL;

    // pango
    pango_fm = pango_ft2_font_map_new();
    pango_ctx = pango_font_map_create_context(pango_fm);
}

static
void
__attribute__((destructor))
destructor_tables(void)
{
    g_hash_table_unref(var_ht);
    g_hash_table_unref(pp_to_np_ht);
    g_hash_table_unref(npp_ht);
    g_hash_table_unref(npobj_to_npp_ht);
    g_list_free(url_pair_list);

    // pango
    g_object_unref(pango_ctx);
    g_object_unref(pango_fm);
    pango_ctx = NULL;
    pango_fm = NULL;
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
    return PP_MakeStringN(s, s ? strlen(s) : 0);
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
    if (reference_obj) {
        obj->klass = reference_obj->klass;
    } else {
        obj->klass = &n2p_proxy_class;
    }
    obj->data = data;
    var.type = PP_VARTYPE_OBJECT;
    var.value.as_id = (size_t)(void *)obj;
    tables_ref_var(var);
    return var;
}

struct PP_Var
np_variant_to_pp_var(NPVariant v)
{
    switch (v.type) {
    case NPVariantType_Void:    return PP_MakeUndefined();
    case NPVariantType_Null:    return PP_MakeNull();
    case NPVariantType_Bool:    return PP_MakeBool(v.value.boolValue);
    case NPVariantType_Int32:   return PP_MakeInt32(v.value.intValue);
    case NPVariantType_Double:  return PP_MakeDouble(v.value.doubleValue);
    case NPVariantType_String:  return PP_MakeStringN(v.value.stringValue.UTF8Characters,
                                                      v.value.stringValue.UTF8Length);
    default:                    return PP_MakeUndefined();

    case NPVariantType_Object:
        if (v.value.objectValue->_class == &p2n_proxy_class) {
            struct np_proxy_object_s *p = (void *)v.value.objectValue;
            return p->ppobj;
        } else {
            return PP_MakeBrowserObject(v.value.objectValue, NULL);
        }
        break;
    }
}

NPVariant
pp_var_to_np_variant(struct PP_Var var)
{
    NPVariant res;
    const char *s;
    struct np_proxy_object_s *np_proxy_object;
    struct pp_var_object_s *ppobj;

    switch (var.type) {
    case PP_VARTYPE_NULL:
        NULL_TO_NPVARIANT(res);
        break;
    case PP_VARTYPE_BOOL:
        BOOLEAN_TO_NPVARIANT(var.value.as_bool, res);
        break;
    case PP_VARTYPE_INT32:
        INT32_TO_NPVARIANT(var.value.as_int, res);
        break;
    case PP_VARTYPE_DOUBLE:
        DOUBLE_TO_NPVARIANT(var.value.as_double, res);
        break;
    case PP_VARTYPE_STRING:
        s = ppb_var_var_to_utf8(var, NULL);
        STRINGZ_TO_NPVARIANT(s, res);
        break;
    case PP_VARTYPE_OBJECT:
        ppobj = (void *)(size_t)var.value.as_id;
        res.type = NPVariantType_Object;
        if (ppobj->klass == &n2p_proxy_class) {
            res.type = NPVariantType_Object;
            res.value.objectValue = ppobj->data;
            npn.retainobject(res.value.objectValue);
        } else {
            res.type = NPVariantType_Object;
            np_proxy_object = malloc(sizeof(struct np_proxy_object_s));
            np_proxy_object->npobj.referenceCount = 1;
            np_proxy_object->npobj._class = &p2n_proxy_class;
            np_proxy_object->ppobj = var;
            res.value.objectValue = (NPObject *)np_proxy_object;
            tables_ref_var(var);
        }
        break;

    case PP_VARTYPE_UNDEFINED:
    case PP_VARTYPE_ARRAY:
    case PP_VARTYPE_DICTIONARY:
    case PP_VARTYPE_ARRAY_BUFFER:
    case PP_VARTYPE_RESOURCE:
    default:
        VOID_TO_NPVARIANT(res);
        break;
    }

    return res;
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

PangoContext *
tables_get_pango_ctx(void)
{
    return pango_ctx;
}

PangoFontMap *
tables_get_pango_font_map(void)
{
    return pango_fm;
}

PangoFontDescription *
pp_font_desc_to_pango_font_desc(const struct PP_BrowserFont_Trusted_Description *description)
{
    PangoFontDescription *font_desc;

    if (description->face.type == PP_VARTYPE_STRING) {
        const char *s = ppb_var_var_to_utf8(description->face, NULL);
        font_desc = pango_font_description_from_string(s);
    } else {
        font_desc = pango_font_description_new();
        switch (description->family) {
        case PP_BROWSERFONT_TRUSTED_FAMILY_SERIF:
            pango_font_description_set_family(font_desc, "serif");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_SANSSERIF:
            pango_font_description_set_family(font_desc, "sans-serif");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_MONOSPACE:
            pango_font_description_set_family(font_desc, "monospace");
            break;
        case PP_BROWSERFONT_TRUSTED_FAMILY_DEFAULT:
            // fall through
        default:
            // do nothing
            break;
        }
    }

    pango_font_description_set_absolute_size(font_desc, description->size * PANGO_SCALE);
    pango_font_description_set_weight(font_desc, (description->weight + 1) * 100);
    if (description->italic)
        pango_font_description_set_style(font_desc, PANGO_STYLE_ITALIC);
    if (description->small_caps)
        pango_font_description_set_variant(font_desc, PANGO_VARIANT_SMALL_CAPS);

    return font_desc;
}

void
tables_add_npobj_npp_mapping(NPObject *npobj, NPP npp)
{
    g_hash_table_insert(npobj_to_npp_ht, npobj, npp);
}

NPP
tables_get_npobj_npp_mapping(NPObject *npobj)
{
    return g_hash_table_lookup(npobj_to_npp_ht, npobj);
}

void
tables_remove_npobj_npp_mapping(NPObject *npobj)
{
    g_hash_table_remove(npobj_to_npp_ht, npobj);
}
