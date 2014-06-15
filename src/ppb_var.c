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
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "tables.h"
#include <ppapi/c/dev/ppb_var_deprecated.h>
#include <ppapi/c/dev/ppp_class_deprecated.h>
#include "n2p_proxy_class.h"


void
ppb_var_add_ref(struct PP_Var var)
{
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        tables_ref_var(var);
    }
}

void
ppb_var_release(struct PP_Var var)
{
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        struct pp_var_object_s *obj;
        int ref_cnt = tables_unref_var(var);
        if (ref_cnt != 0)
            return;

        switch (var.type) {
        case PP_VARTYPE_STRING:
            free((void*)(size_t)var.value.as_id);
            break;
        case PP_VARTYPE_OBJECT:
            obj = (void*)(size_t)var.value.as_id;
            if (obj->klass == &n2p_proxy_class)
                n2p_proxy_class.Deallocate(obj);
            free(obj);
            break;
        default:
            // do nothing
            break;
        }
    }
}

struct PP_Var
ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    return PP_MakeStringN(data, len);
}

struct PP_Var
ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    (void)module;
    return PP_MakeStringN(data, len);
}

const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    if (PP_VARTYPE_STRING == var.type) {
        const char *s = (void *)(size_t)(var.value.as_id + sizeof(uint32_t));
        if (len)
            *len = *(uint32_t *)(size_t)(var.value.as_id);
        return s;
    } else {
        trace_warning("var is not a string in %s\n", __func__);
        if (len)
            *len = 0;
        return "";
    }
}

bool
ppb_var_has_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || name.type != PP_VARTYPE_STRING)
        return false;
    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;

    if (obj->klass->HasProperty)
        return obj->klass->HasProperty(obj->data, name, exception);
    else
        return false;
}

bool
ppb_var_has_method(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || name.type != PP_VARTYPE_STRING)
        return false;
    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;

    if (obj->klass->HasMethod)
        return obj->klass->HasMethod(obj->data, name, exception);
    else
        return false;
}

struct PP_Var
ppb_var_get_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->GetProperty)
        return obj->klass->GetProperty(obj->data, name, exception);
    else
        return PP_MakeUndefined();
}

void
ppb_var_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                               struct PP_Var **properties, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        return;
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->GetAllPropertyNames)
        obj->klass->GetAllPropertyNames(obj->data, property_count, properties, exception);
}

void
ppb_var_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                     struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return;
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->SetProperty)
        obj->klass->SetProperty(obj->data, name, value, exception);
}

void
ppb_var_remove_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return;
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->RemoveProperty)
        obj->klass->RemoveProperty(obj->data, name, exception);
}

struct PP_Var
ppb_var_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc,
             struct PP_Var *argv, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT || method_name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->Call)
        return obj->klass->Call(obj->data, method_name, argc, argv, exception);
    else
        return PP_MakeUndefined();
}

struct PP_Var
ppb_var_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                  struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    if (obj->klass->Construct)
        return obj->klass->Construct(obj->data, argc, argv, exception);
    else
        return PP_MakeUndefined();
}

bool
ppb_var_is_instance_of(struct PP_Var var, const struct PPP_Class_Deprecated *object_class,
                       void **object_data)
{
    if (var.type != PP_VARTYPE_OBJECT)
        return false;
    struct pp_var_object_s *obj = (void *)(size_t)var.value.as_id;
    if (obj->klass == object_class) {
        *object_data = obj->data;
        return true;
    }
    return false;
}

struct PP_Var
ppb_var_create_object(PP_Instance instance, const struct PPP_Class_Deprecated *object_class,
                      void *object_data)
{
    (void)instance;
    struct PP_Var var = { };
    struct pp_var_object_s *obj;

    obj = malloc(sizeof(*obj));
    var.type = PP_VARTYPE_OBJECT;
    var.value.as_id = (int64_t)(size_t)obj;
    obj->klass = object_class;
    obj->data = object_data;
    tables_ref_var(var);

    return var;
}

struct PP_Var
ppb_var_create_object_with_module_deprecated(PP_Module module,
                                             const struct PPP_Class_Deprecated *object_class,
                                             void *object_data)
{
    (void)module;
    struct PP_Var var = { };
    struct pp_var_object_s *obj;

    obj = malloc(sizeof(*obj));
    var.type = PP_VARTYPE_OBJECT;
    var.value.as_id = (int64_t)(size_t)obj;
    obj->klass = object_class;
    obj->data = object_data;
    tables_ref_var(var);

    return var;
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
trace_ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s data=%.*s, len=%d\n", __func__+6, len, data, len);
    return ppb_var_var_from_utf8_1_1(data, len);
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


const struct PPB_Var_1_1 ppb_var_interface_1_1 = {
    .AddRef =       TWRAPF(ppb_var_add_ref),
    .Release =      TWRAPF(ppb_var_release),
    .VarFromUtf8 =  TWRAPF(ppb_var_var_from_utf8_1_1),
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
