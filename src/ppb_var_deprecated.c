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

#include "ppb_var_deprecated.h"
#include <inttypes.h>
#include <stddef.h>
#include "pp_resource.h"
#include "trace.h"
#include "tables.h"
#include "browser_object.h"


void
ppb_var_deprecated_add_ref(struct PP_Var var)
{
}

void
ppb_var_deprecated_release(struct PP_Var var)
{
}

struct PP_Var
ppb_var_deprecated_var_from_utf8(PP_Module module, const char *data, uint32_t len)
{
    struct PP_Var var = {0};
    return var;
}

const char *
ppb_var_deprecated_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    return "placeholder";
}

bool
ppb_var_deprecated_has_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

bool
ppb_var_deprecated_has_method(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

struct PP_Var
ppb_var_deprecated_get_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception)
{
    if (object.type != PP_VARTYPE_OBJECT) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct pp_var_object_s *obj = (void *)(size_t)object.value.as_id;
    return obj->klass->GetProperty(obj, name, exception);
}

void
ppb_var_deprecated_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                                          struct PP_Var **properties, struct PP_Var *exception)
{
}

void
ppb_var_deprecated_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                                struct PP_Var *exception)
{
}

void
ppb_var_deprecated_remove_property(struct PP_Var object, struct PP_Var name,
                                   struct PP_Var *exception)
{
}

struct PP_Var
ppb_var_deprecated_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc,
                        struct PP_Var *argv, struct PP_Var *exception)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_var_deprecated_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                             struct PP_Var *exception)
{
    struct PP_Var var = {0};
    return var;
}

bool
ppb_var_deprecated_is_instance_of(struct PP_Var var,
                                  const struct PPP_Class_Deprecated *object_class,
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
ppb_var_deprecated_create_object(PP_Instance instance,
                                 const struct PPP_Class_Deprecated *object_class,
                                 void *object_data)
{
    struct PP_Var var;
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
ppb_var_deprecated_create_object_with_module_deprecated(PP_Module module,
                            const struct PPP_Class_Deprecated *object_class, void *object_data)
{
    struct PP_Var var = {0};
    return var;
}

// trace wrappers
void
trace_ppb_var_deprecated_add_ref(struct PP_Var var)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {zilch} %s var=%s\n", __func__+6, s_var);
    free(s_var);
    ppb_var_deprecated_add_ref(var);
}

void
trace_ppb_var_deprecated_release(struct PP_Var var)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {zilch} %s var=%s\n", __func__+6, s_var);
    free(s_var);
    ppb_var_deprecated_release(var);
}

struct PP_Var
trace_ppb_var_deprecated_var_from_utf8(PP_Module module, const char *data, uint32_t len)
{
    trace_info("[PPB] {zilch} %s module=%d, data=%.*s, len=%u\n", __func__+6,
               module, len, data, len);
    return ppb_var_deprecated_var_from_utf8(module, data, len);
}

const char *
trace_ppb_var_deprecated_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {zilch} %s var=%s\n", __func__+6, s_var);
    free(s_var);
    return ppb_var_deprecated_var_to_utf8(var, len);
}

bool
trace_ppb_var_deprecated_has_property(struct PP_Var object, struct PP_Var name,
                                      struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {zilch} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    free(s_name);
    free(s_object);
    return ppb_var_deprecated_has_property(object, name, exception);
}

bool
trace_ppb_var_deprecated_has_method(struct PP_Var object, struct PP_Var name,
                                    struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {zilch} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    free(s_name);
    free(s_object);
    return ppb_var_deprecated_has_method(object, name, exception);
}

struct PP_Var
trace_ppb_var_deprecated_get_property(struct PP_Var object, struct PP_Var name,
                                      struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {full} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    free(s_name);
    free(s_object);
    return ppb_var_deprecated_get_property(object, name, exception);
}

void
trace_ppb_var_deprecated_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                                                struct PP_Var **properties,
                                                struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    trace_info("[PPB] {zilch} %s object=%s\n", __func__+6, s_object);
    free(s_object);
    ppb_var_deprecated_get_all_property_names(object, property_count, properties, exception);
}

void
trace_ppb_var_deprecated_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                                      struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    char *s_value = trace_var_as_string(value);
    trace_info("[PPB] {zilch} %s object=%s, name=%s, value=%s\n", __func__+6, s_object, s_name,
               s_value);
    free(s_object);
    free(s_name);
    free(s_value);
    ppb_var_deprecated_set_property(object, name, value, exception);
}

void
trace_ppb_var_deprecated_remove_property(struct PP_Var object, struct PP_Var name,
                                         struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_name = trace_var_as_string(name);
    trace_info("[PPB] {zilch} %s object=%s, name=%s\n", __func__+6, s_object, s_name);
    free(s_object);
    free(s_name);
    ppb_var_deprecated_remove_property(object, name, exception);
}

struct PP_Var
trace_ppb_var_deprecated_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc,
                              struct PP_Var *argv, struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    char *s_method_name = trace_var_as_string(method_name);
    trace_info("[PPB] {zilch} %s object=%s, method_name=%s, argc=%u, argv=TODO\n", __func__+6,
               s_object, s_method_name, argc);
    free(s_object);
    free(s_method_name);
    return ppb_var_deprecated_call(object, method_name, argc, argv, exception);
}

struct PP_Var
trace_ppb_var_deprecated_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                                   struct PP_Var *exception)
{
    char *s_object = trace_var_as_string(object);
    trace_info("[PPB] {zilch} %s object=%s, argc=%u, argv=TODO\n", __func__+6, s_object, argc);
    free(s_object);
    return ppb_var_deprecated_construct(object, argc, argv, exception);
}

bool
trace_ppb_var_deprecated_is_instance_of(struct PP_Var var,
                                        const struct PPP_Class_Deprecated *object_class,
                                        void **object_data)
{
    char *s_var = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s, object_class=%p\n", __func__+6, s_var, object_class);
    free(s_var);
    return ppb_var_deprecated_is_instance_of(var, object_class, object_data);
}

struct PP_Var
trace_ppb_var_deprecated_create_object(PP_Instance instance,
                                       const struct PPP_Class_Deprecated *object_class,
                                       void *object_data)
{
    trace_info("[PPB] {full} %s instanse=%d, object_class=%p, object_data=%p\n", __func__+6,
               instance, object_class, object_data);
    return ppb_var_deprecated_create_object(instance, object_class, object_data);
}

struct PP_Var
trace_ppb_var_deprecated_create_object_with_module_deprecated(PP_Module module,
            const struct PPP_Class_Deprecated *object_class, void *object_data)
{
    trace_info("[PPB] {zilch} %s module=%d, object_class=%p, object_data=%p\n", __func__+6,
               module, object_class, object_data);
    return ppb_var_deprecated_create_object_with_module_deprecated(module, object_class,
                                                                   object_data);
}


const struct PPB_Var_Deprecated ppb_var_deprecated_interface_0_3 = {
    .AddRef =               trace_ppb_var_deprecated_add_ref,
    .Release =              trace_ppb_var_deprecated_release,
    .VarFromUtf8 =          trace_ppb_var_deprecated_var_from_utf8,
    .VarToUtf8 =            trace_ppb_var_deprecated_var_to_utf8,
    .HasProperty =          trace_ppb_var_deprecated_has_property,
    .HasMethod =            trace_ppb_var_deprecated_has_method,
    .GetProperty =          trace_ppb_var_deprecated_get_property,
    .GetAllPropertyNames =  trace_ppb_var_deprecated_get_all_property_names,
    .SetProperty =          trace_ppb_var_deprecated_set_property,
    .RemoveProperty =       trace_ppb_var_deprecated_remove_property,
    .Call =                 trace_ppb_var_deprecated_call,
    .Construct =            trace_ppb_var_deprecated_construct,
    .IsInstanceOf =         trace_ppb_var_deprecated_is_instance_of,
    .CreateObject =         trace_ppb_var_deprecated_create_object,
    .CreateObjectWithModuleDeprecated = trace_ppb_var_deprecated_create_object_with_module_deprecated,
};
