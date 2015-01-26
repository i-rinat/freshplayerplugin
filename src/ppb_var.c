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

#include "ppb_var.h"
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "tables.h"
#include <ppapi/c/dev/ppb_var_deprecated.h>
#include <ppapi/c/dev/ppp_class_deprecated.h>
#include "n2p_proxy_class.h"
#include "p2n_proxy_class.h"
#include "config.h"


static GHashTable      *var_ht;
static pthread_mutex_t  lock;
static uint32_t         var_id = 1;

struct var_s {
    struct PP_Var   var;
    int             ref_count;
    struct {
        uint32_t    len;
        char       *data;
    } str;
    struct {
        const struct PPP_Class_Deprecated  *_class;
        void                               *data;
    } obj;
    void           *map_addr;
};


static
void
__attribute__((constructor))
constructor_ppb_var(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pthread_mutex_init(&lock, NULL);
}

static
void
__attribute__((destructor))
destructor_ppb_var(void)
{
    g_hash_table_unref(var_ht);
    pthread_mutex_destroy(&lock);
}

static
int
reference_countable(struct PP_Var var)
{
    return var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
           var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
           var.type == PP_VARTYPE_ARRAY_BUFFER;
}

/* should be run with lock held */
static
uint32_t
get_new_var_id(void)
{
    do {
        var_id ++;
    } while (g_hash_table_lookup(var_ht, GSIZE_TO_POINTER(var_id)) != NULL);

    return var_id;
}

static
struct var_s *
get_var_s(struct PP_Var var)
{
    pthread_mutex_lock(&lock);
    struct var_s *v = g_hash_table_lookup(var_ht, GSIZE_TO_POINTER(var.value.as_id));
    pthread_mutex_unlock(&lock);
    return v;
}

NPVariant
pp_var_to_np_variant(struct PP_Var var)
{
    NPVariant res;
    struct var_s *v;

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
        do {
            uint32_t len;
            const char *s1 = ppb_var_var_to_utf8(var, &len);
            char *s2 = npn.memalloc(len + 1); // TODO: call on main thread?
            memcpy(s2, s1, len + 1);
            res.type = NPVariantType_String;
            res.value.stringValue.UTF8Length = len;
            res.value.stringValue.UTF8Characters = s2;
        } while (0);
        break;
    case PP_VARTYPE_OBJECT:
        v = get_var_s(var);
        res.type = NPVariantType_Object;
        if (v->obj._class == &n2p_proxy_class) {
            res.type = NPVariantType_Object;
            res.value.objectValue = v->obj.data;
            npn.retainobject(res.value.objectValue); // TODO: call on main thread?
        } else {
            struct np_proxy_object_s *np_proxy_object = malloc(sizeof(struct np_proxy_object_s));
            res.type = NPVariantType_Object;
            np_proxy_object->npobj.referenceCount = 1;
            np_proxy_object->npobj._class = &p2n_proxy_class;
            np_proxy_object->ppobj = var;
            res.value.objectValue = (NPObject *)np_proxy_object;
            ppb_var_add_ref(var);
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

struct PP_Var
np_variant_to_pp_var(NPVariant v)
{
    switch (v.type) {
    case NPVariantType_Void:    return PP_MakeUndefined();
    case NPVariantType_Null:    return PP_MakeNull();
    case NPVariantType_Bool:    return PP_MakeBool(v.value.boolValue);
    case NPVariantType_Int32:   return PP_MakeInt32(v.value.intValue);
    case NPVariantType_Double:  return PP_MakeDouble(v.value.doubleValue);
    case NPVariantType_String:  return ppb_var_var_from_utf8(v.value.stringValue.UTF8Characters,
                                                             v.value.stringValue.UTF8Length);
    default:                    return PP_MakeUndefined();

    case NPVariantType_Object:
        if (v.value.objectValue->_class == &p2n_proxy_class) {
            struct np_proxy_object_s *p = (void *)v.value.objectValue;
            ppb_var_add_ref(p->ppobj);
            return p->ppobj;
        } else {
            return ppb_var_create_object(0, &n2p_proxy_class, v.value.objectValue);
        }
        break;
    }
}

void
ppb_var_add_ref(struct PP_Var var)
{
    if (!reference_countable(var))
        return;

    pthread_mutex_lock(&lock);
    void *key = GSIZE_TO_POINTER(var.value.as_id);
    struct var_s *v = g_hash_table_lookup(var_ht, key);
    if (v)
        v->ref_count ++;
    pthread_mutex_unlock(&lock);
}

struct PP_Var
ppb_var_add_ref2(struct PP_Var var)
{
    ppb_var_add_ref(var);
    return var;
}

void
ppb_var_release(struct PP_Var var)
{
    if (!reference_countable(var))
        return;

    pthread_mutex_lock(&lock);
    void *key = GSIZE_TO_POINTER(var.value.as_id);
    struct var_s *v = g_hash_table_lookup(var_ht, key);
    int retain = 1;
    if (v) {
        v->ref_count --;
        if (v->ref_count <= 0) {
            retain = 0;
            g_hash_table_remove(var_ht, key);
        }
    }
    pthread_mutex_unlock(&lock);

    if (retain)
        return;

    switch (var.type) {
    case PP_VARTYPE_STRING:
        free(v->str.data);
        break;
    case PP_VARTYPE_OBJECT:
        if (v->obj._class == &n2p_proxy_class)
            n2p_proxy_class.Deallocate(v->obj.data);
        break;
    case PP_VARTYPE_ARRAY_BUFFER:
        free(v->str.data);
        if (v->map_addr)
            free(v->map_addr);
        v->map_addr = NULL;
        break;
    default:
        // do nothing
        break;
    }

    g_slice_free(struct var_s, v);

    if (config.quirks.dump_variables) {
        time_t current_time = time(NULL);
        static uintptr_t throttling = 0;

        if (current_time % 5 == 0 || config.quirks.dump_variables > 1) {
            if (!throttling || config.quirks.dump_variables > 1) {
                pthread_mutex_lock(&lock);
                GList *key_list = g_hash_table_get_keys(var_ht);
                guint var_count = g_list_length(key_list);
                pthread_mutex_unlock(&lock);
                trace_info("--- %3u variables --------------------------------\n", var_count);

                GList *lptr = key_list;
                while (lptr) {
                    pthread_mutex_lock(&lock);
                    struct var_s *v = g_hash_table_lookup(var_ht, lptr->data);
                    struct PP_Var var = v ? v->var : PP_MakeUndefined();
                    pthread_mutex_unlock(&lock);

                    if (v) {
                        gchar *s_var = trace_var_as_string(var);
                        trace_info("[%d] = %s\n", GPOINTER_TO_INT(lptr->data), s_var);
                        g_free(s_var);
                    } else {
                        trace_info("[%d] expunged\n", GPOINTER_TO_INT(lptr->data));
                    }
                    lptr = g_list_next(lptr);
                }
                g_list_free(key_list);
                trace_info("==================================================\n");
                throttling = 1;
            }
        } else {
            throttling = 0;
        }
    }
}

int
ppb_var_get_ref_count(struct PP_Var var)
{
    if (!reference_countable(var))
        return 0;

    pthread_mutex_lock(&lock);
    void *key = GSIZE_TO_POINTER(var.value.as_id);
    struct var_s *v = g_hash_table_lookup(var_ht, key);
    int ref_count = v ? v->ref_count : 0;
    pthread_mutex_unlock(&lock);

    return ref_count;
}

struct PP_Var
ppb_var_var_from_utf8(const char *data, uint32_t len)
{
    struct var_s *v = g_slice_alloc(sizeof(*v));
    struct PP_Var var = {};

    var.type = PP_VARTYPE_STRING;
    v->str.len = len;
    v->str.data = malloc(len + 1);
    memcpy(v->str.data, data, len);
    v->str.data[len] = 0;       // ensure all strings are zero terminated
    v->ref_count = 1;

    pthread_mutex_lock(&lock);
    var.value.as_id = get_new_var_id();
    v->var = var;
    g_hash_table_insert(var_ht, GSIZE_TO_POINTER(var.value.as_id), v);
    pthread_mutex_unlock(&lock);

    return var;
}

struct PP_Var
ppb_var_var_from_utf8_z(const char *data)
{
    return ppb_var_var_from_utf8(data, data ? strlen(data) : 0);
}


struct PP_Var
ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    return ppb_var_var_from_utf8(data, len);
}

const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    if (PP_VARTYPE_STRING == var.type) {
        struct var_s *v = get_var_s(var);
        if (v) {
            if (len)
                *len = v->str.len;
            // assuming all strings are stored with zero terminator
            return v->str.data;
        }
    }

    trace_warning("%s, 'var' is not a string, (%d)\n", __func__, var.type);
    if (len)
        *len = 0;
    return "";
}

PP_Resource
ppb_var_var_to_resource(struct PP_Var var)
{
    return 0;
}

struct PP_Var
ppb_var_var_from_resource(PP_Resource resource)
{
    return PP_MakeUndefined();
}

bool
ppb_var_has_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        trace_error("%s, 'object' is not an object\n", __func__);
        return false;
    }

    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'name' is not a string\n", __func__);
        return false;
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->HasProperty)
        return objv->obj._class->HasProperty(objv->obj.data, name, exception);
    else
        return false;
}

bool
ppb_var_has_method(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        trace_error("%s, 'object' is not an object\n", __func__);
        return false;
    }

    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'name' is not a string\n", __func__);
        return false;
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->HasMethod)
        return objv->obj._class->HasMethod(objv->obj.data, name, exception);
    else
        return false;
}

struct PP_Var
ppb_var_get_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return PP_MakeUndefined();
    }

    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'name' is not a string\n", __func__);
        return PP_MakeUndefined();
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->GetProperty)
        return objv->obj._class->GetProperty(objv->obj.data, name, exception);
    else
        return PP_MakeUndefined();
}

void
ppb_var_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                               struct PP_Var **properties, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return;
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->GetAllPropertyNames)
        objv->obj._class->GetAllPropertyNames(objv->obj.data, property_count, properties,
                                              exception);
}

void
ppb_var_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                     struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return;
    }

    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'name' is not a string\n", __func__);
        return;
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->SetProperty)
        objv->obj._class->SetProperty(objv->obj.data, name, value, exception);
}

void
ppb_var_remove_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return;
    }

    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'name' is not a string\n", __func__);
        return;
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->RemoveProperty)
        objv->obj._class->RemoveProperty(objv->obj.data, name, exception);
}

struct PP_Var
ppb_var_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc,
             struct PP_Var *argv, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return PP_MakeUndefined();
    }

    if (method_name.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'method_name' is not a string\n", __func__);
        return PP_MakeUndefined();
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->Call)
        return objv->obj._class->Call(objv->obj.data, method_name, argc, argv, exception);
    else
        return PP_MakeUndefined();
}

struct PP_Var
ppb_var_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                  struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        trace_error("%s, 'object' is not an object\n", __func__);
        return PP_MakeUndefined();
    }

    struct var_s *objv = get_var_s(object);
    if (objv->obj._class->Construct)
        return objv->obj._class->Construct(objv->obj.data, argc, argv, exception);
    else
        return PP_MakeUndefined();
}

bool
ppb_var_is_instance_of(struct PP_Var var, const struct PPP_Class_Deprecated *object_class,
                       void **object_data)
{
    if (var.type != PP_VARTYPE_OBJECT) {
        trace_error("%s, 'var' is not an object\n", __func__);
        return false;
    }

    struct var_s *objv = get_var_s(var);
    if (objv->obj._class == object_class) {
        if (object_data)
            *object_data = objv->obj.data;
        return true;
    }
    return false;
}

struct PP_Var
ppb_var_create_object(PP_Instance instance, const struct PPP_Class_Deprecated *object_class,
                      void *object_data)
{
    (void)instance;
    struct PP_Var var = {};
    struct var_s *v = g_slice_alloc(sizeof(*v));

    var.type = PP_VARTYPE_OBJECT;
    v->obj._class = object_class;
    v->obj.data = object_data;
    v->ref_count = 1;

    pthread_mutex_lock(&lock);
    var.value.as_id = get_new_var_id();
    v->var = var;
    g_hash_table_insert(var_ht, GSIZE_TO_POINTER(var.value.as_id), v);
    pthread_mutex_unlock(&lock);

    return var;
}

struct PP_Var
ppb_var_create_object_with_module_deprecated(PP_Module module,
                                             const struct PPP_Class_Deprecated *object_class,
                                             void *object_data)
{
    (void)module;
    return ppb_var_create_object(0, object_class, object_data);
}

char *
ppb_var_trace_object_var(struct PP_Var var)
{
    if (var.type == PP_VARTYPE_OBJECT) {
        struct var_s *objv = get_var_s(var);
        int ref_count = ppb_var_get_ref_count(var);
        return g_strdup_printf("{OBJECT:%d:class=%p:data=%p}", ref_count, objv->obj._class,
                               objv->obj.data);
    } else {
        return "";
    }
}

struct PP_Var
ppb_var_array_buffer_create(uint32_t size_in_bytes)
{
    struct var_s *v = g_slice_alloc0(sizeof(*v));
    struct PP_Var var = {};

    var.type = PP_VARTYPE_ARRAY_BUFFER;
    v->str.len = size_in_bytes;
    v->str.data = calloc(size_in_bytes, 1);
    v->ref_count = 1;

    pthread_mutex_lock(&lock);
    var.value.as_id = get_new_var_id();
    v->var = var;
    g_hash_table_insert(var_ht, GSIZE_TO_POINTER(var.value.as_id), v);
    pthread_mutex_unlock(&lock);

    return var;
}

PP_Bool
ppb_var_array_buffer_byte_length(struct PP_Var var, uint32_t *byte_length)
{
    if (var.type != PP_VARTYPE_ARRAY_BUFFER || !byte_length)
        return PP_FALSE;

    struct var_s *v = get_var_s(var);
    if (!v)
        return PP_FALSE;

    *byte_length = v->str.len;
    return PP_TRUE;
}

void *
ppb_var_array_buffer_map(struct PP_Var var)
{
    if (var.type != PP_VARTYPE_ARRAY_BUFFER) {
        trace_error("%s, not an array buffer var\n", __func__);
        return NULL;
    }

    struct var_s *v = get_var_s(var);
    if (!v) {
        trace_error("%s, variable gone\n", __func__);
        return NULL;
    }

    if (v->map_addr)
        return v->map_addr;

    v->map_addr = malloc(v->str.len);
    if (!v->map_addr)
        return NULL;
    memcpy(v->map_addr, v->str.data, v->str.len);

    return v->map_addr;
}

void
ppb_var_array_buffer_unmap(struct PP_Var var)
{
    if (var.type != PP_VARTYPE_ARRAY_BUFFER) {
        trace_error("%s, not an array buffer var\n", __func__);
        return;
    }

    struct var_s *v = get_var_s(var);
    if (!v) {
        trace_error("%s, variable gone\n", __func__);
        return;
    }

    // return if not mapped
    if (!v->map_addr)
        return;

    // copy data back to the buffer
    memcpy(v->str.data, v->map_addr, v->str.len);
    free(v->map_addr);
    v->map_addr = NULL;
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_var_add_ref(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    g_free(var_str);
    ppb_var_add_ref(var);
}

TRACE_WRAPPER
void
trace_ppb_var_release(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    g_free(var_str);
    ppb_var_release(var);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_var_from_utf8(const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s data=%.*s, len=%d\n", __func__+6, len, data, len);
    return ppb_var_var_from_utf8(data, len);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s module=%d, data=%.*s, len=%d\n", __func__+6, module, len, data,len);
    return ppb_var_var_from_utf8_1_0(module, data, len);
}

TRACE_WRAPPER
const char *
trace_ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    g_free(var_str);
    return ppb_var_var_to_utf8(var, len);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_var_var_to_resource(struct PP_Var var)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_var_var_to_resource(var);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_var_from_resource(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_var_var_from_resource(resource);
}

TRACE_WRAPPER
bool
trace_ppb_var_has_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {full} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    g_free(s_name);
    g_free(s_object);
    return ppb_var_has_property(object, name, exception);
}

TRACE_WRAPPER
bool
trace_ppb_var_has_method(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {full} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    g_free(s_name);
    g_free(s_object);
    return ppb_var_has_method(object, name, exception);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_get_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {full} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    g_free(s_name);
    g_free(s_object);
    return ppb_var_get_property(object, name, exception);
}

TRACE_WRAPPER
void
trace_ppb_var_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                                     struct PP_Var **properties, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    trace_info("[PPB] {full} %s object=%s\n", __func__+6, s_object);
    g_free(s_object);
    ppb_var_get_all_property_names(object, property_count, properties, exception);
}

TRACE_WRAPPER
void
trace_ppb_var_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                           struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    char *s_value = trace_var_as_string(value);
    trace_info("[PPB] {full} %s object=%s, name=%s, value=%s\n", __func__+6, s_object, s_name,
               s_value);
    g_free(s_object);
    g_free(s_name);
    g_free(s_value);
    ppb_var_set_property(object, name, value, exception);
}

TRACE_WRAPPER
void
trace_ppb_var_remove_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {full} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    g_free(s_object);
    g_free(s_name);
    ppb_var_remove_property(object, name, exception);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc,
                   struct PP_Var *argv, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_method_name = trace_var_as_string(method_name);
    trace_info("[PPB] {full} %s object=%s, method_name=%s, argc=%u, argv=TODO\n", __func__+6,
               s_object, s_method_name, argc);
    g_free(s_object);
    g_free(s_method_name);
    return ppb_var_call(object, method_name, argc, argv, exception);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                        struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    trace_info("[PPB] {full} %s object=%s, argc=%u, argv=TODO\n", __func__+6, s_object, argc);
    g_free(s_object);
    return ppb_var_construct(object, argc, argv, exception);
}

TRACE_WRAPPER
bool
trace_ppb_var_is_instance_of(struct PP_Var var, const struct PPP_Class_Deprecated *object_class,
                             void **object_data)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s, object_class=%p\n", __func__+6, s_var, object_class);
    g_free(s_var);
    return ppb_var_is_instance_of(var, object_class, object_data);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_create_object(PP_Instance instance, const struct PPP_Class_Deprecated *object_class,
                            void *object_data)
{
    trace_info("[PPB] {full} %s instanse=%d, object_class=%p, object_data=%p\n", __func__+6,
               instance, object_class, object_data);
    return ppb_var_create_object(instance, object_class, object_data);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_create_object_with_module_deprecated(PP_Module module,
                                                   const struct PPP_Class_Deprecated *object_class,
                                                   void *object_data)
{
    trace_info("[PPB] {full} %s module=%d, object_class=%p, object_data=%p\n", __func__+6,
               module, object_class, object_data);
    return ppb_var_create_object_with_module_deprecated(module, object_class,
                                                                   object_data);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_var_array_buffer_create(uint32_t size_in_bytes)
{
    trace_info("[PPB] {full} %s size_in_bytes=%u\n", __func__+6, size_in_bytes);
    return ppb_var_array_buffer_create(size_in_bytes);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_var_array_buffer_byte_length(struct PP_Var var, uint32_t *byte_length)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, s_var);
    g_free(s_var);
    return ppb_var_array_buffer_byte_length(var, byte_length);
}

TRACE_WRAPPER
void *
trace_ppb_var_array_buffer_map(struct PP_Var var)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, s_var);
    g_free(s_var);
    return ppb_var_array_buffer_map(var);
}

TRACE_WRAPPER
void
trace_ppb_var_array_buffer_unmap(struct PP_Var var)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, s_var);
    g_free(s_var);
    return ppb_var_array_buffer_unmap(var);
}


const struct PPB_Var_1_2 ppb_var_interface_1_2 = {
    .AddRef          = TWRAPF(ppb_var_add_ref),
    .Release         = TWRAPF(ppb_var_release),
    .VarFromUtf8     = TWRAPF(ppb_var_var_from_utf8),
    .VarToUtf8       = TWRAPF(ppb_var_var_to_utf8),
    .VarToResource   = TWRAPZ(ppb_var_var_to_resource),
    .VarFromResource = TWRAPZ(ppb_var_var_from_resource),
};

const struct PPB_Var_1_1 ppb_var_interface_1_1 = {
    .AddRef =       TWRAPF(ppb_var_add_ref),
    .Release =      TWRAPF(ppb_var_release),
    .VarFromUtf8 =  TWRAPF(ppb_var_var_from_utf8),
    .VarToUtf8 =    TWRAPF(ppb_var_var_to_utf8),
};

const struct PPB_Var_1_0 ppb_var_interface_1_0 = {
    .AddRef =       TWRAPF(ppb_var_add_ref),
    .Release =      TWRAPF(ppb_var_release),
    .VarFromUtf8 =  TWRAPF(ppb_var_var_from_utf8_1_0),
    .VarToUtf8 =    TWRAPF(ppb_var_var_to_utf8),
};

const struct PPB_Var_Deprecated ppb_var_deprecated_interface_0_3 = {
    .AddRef =               TWRAPF(ppb_var_add_ref),
    .Release =              TWRAPF(ppb_var_release),
    .VarFromUtf8 =          TWRAPF(ppb_var_var_from_utf8_1_0),
    .VarToUtf8 =            TWRAPF(ppb_var_var_to_utf8),
    .HasProperty =          TWRAPF(ppb_var_has_property),
    .HasMethod =            TWRAPF(ppb_var_has_method),
    .GetProperty =          TWRAPF(ppb_var_get_property),
    .GetAllPropertyNames =  TWRAPF(ppb_var_get_all_property_names),
    .SetProperty =          TWRAPF(ppb_var_set_property),
    .RemoveProperty =       TWRAPF(ppb_var_remove_property),
    .Call =                 TWRAPF(ppb_var_call),
    .Construct =            TWRAPF(ppb_var_construct),
    .IsInstanceOf =         TWRAPF(ppb_var_is_instance_of),
    .CreateObject =         TWRAPF(ppb_var_create_object),
    .CreateObjectWithModuleDeprecated = TWRAPF(ppb_var_create_object_with_module_deprecated),
};

const struct PPB_VarArrayBuffer_1_0 ppb_var_array_buffer_interface_1_0 = {
    .Create =       TWRAPF(ppb_var_array_buffer_create),
    .ByteLength =   TWRAPF(ppb_var_array_buffer_byte_length),
    .Map =          TWRAPF(ppb_var_array_buffer_map),
    .Unmap =        TWRAPF(ppb_var_array_buffer_unmap),
};
