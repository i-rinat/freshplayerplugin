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

/*
 * proxy class which wraps around NPObject while providing PP_Var object
 */

#include "n2p_proxy_class.h"
#include "ppapi/c/dev/deprecated_bool.h"
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include <npapi/npruntime.h>
#include "ppb_var.h"


static
bool
n2p_has_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    if (name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return false;
    }

    const char *s_name = ppb_var_var_to_utf8(name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);

    NPP npp = tables_get_npobj_npp_mapping(object);
    bool res = npn.hasproperty(npp, object, identifier);

    return res;
}

static
bool
n2p_has_method(void *object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

static
struct PP_Var
n2p_get_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    if (name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    const char *s_name = ppb_var_var_to_utf8(name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);
    NPVariant np_value;
    NPP npp = tables_get_npobj_npp_mapping(object);
    if (npn.getproperty(npp, object, identifier, &np_value)) {
        struct PP_Var var = np_variant_to_pp_var(np_value);

        if (np_value.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_value.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_value);

        return var;
    } else {
        return PP_MakeUndefined();
    }
}

static
void
n2p_get_all_property_names(void *object, uint32_t *property_count, struct PP_Var **properties,
                           struct PP_Var *exception)
{
}

static
void
n2p_set_property(void *object, struct PP_Var name, struct PP_Var value, struct PP_Var *exception)
{
}

static
void
n2p_remove_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
}

static
struct PP_Var
n2p_call(void *object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
         struct PP_Var *exception)
{
    if (method_name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    const char *s_method_name = ppb_var_var_to_utf8(method_name, NULL);
    NPIdentifier np_method_name = npn.getstringidentifier(s_method_name);
    NPVariant *np_args = malloc(argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < argc; k ++)
        np_args[k] = pp_var_to_np_variant(argv[k]);

    NPP npp = tables_get_npobj_npp_mapping(object);
    NPVariant np_result;
    if (npn.invoke(npp, object, np_method_name, np_args, argc, &np_result)) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_result);

        return var;
    } else {
        return PP_MakeUndefined();
    }

    return PP_MakeUndefined();
}

static
struct PP_Var
n2p_construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    NPVariant *np_args = malloc(argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < argc; k ++)
        np_args[k] = pp_var_to_np_variant(argv[k]);
    NPP npp = tables_get_npobj_npp_mapping(object);
    NPVariant np_result;

    bool res = npn.construct(npp, object, np_args, argc, &np_result);
    for (uint32_t k = 0; k < argc; k ++)
        npn.releasevariantvalue(&np_args[k]);
    free(np_args);

    if (res) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_result);

        return var;
    } else {
        return PP_MakeUndefined();
    }
}

static
void
n2p_deallocate(void *object)
{
    npn.releaseobject(object);
}


#ifndef NDEBUG
// trace wrappers
static
bool
trace_n2p_has_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {full} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_has_property(object, name, exception);
}

static
bool
trace_n2p_has_method(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {zilch} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_has_method(object, name, exception);
}

static
struct PP_Var
trace_n2p_get_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {full} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_get_property(object, name, exception);
}

static
void
trace_n2p_get_all_property_names(void *object, uint32_t *property_count, struct PP_Var **properties,
                                 struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s object=%p\n", __func__+6, object);
    n2p_get_all_property_names(object, property_count, properties, exception);
}

static
void
trace_n2p_set_property(void *object, struct PP_Var name, struct PP_Var value,
                       struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    char *s_value = trace_var_as_string(value);
    trace_info("[CLS] {zilch} %s object=%p, name=%s, value=%s\n", __func__+6, object,
               s_name, s_value);
    g_free(s_name);
    g_free(s_value);
    n2p_set_property(object, name, value, exception);
}

static
void
trace_n2p_remove_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {zilch} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    n2p_remove_property(object, name, exception);
}

static
struct PP_Var
trace_n2p_call(void *object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
                struct PP_Var *exception)
{
    char *s_method_name = trace_var_as_string(method_name);
    trace_info("[CLS] {full} %s object=%p, method_name=%s, argc=%u, argv=%p\n", __func__+6,
               object, s_method_name, argc, argv);
    g_free(s_method_name);
    return n2p_call(object, method_name, argc, argv, exception);
}

static
struct PP_Var
trace_n2p_construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    trace_info("[CLS] {full} %s object=%p, argc=%u, argv=%p\n", __func__+6, object, argc, argv);
    return n2p_construct(object, argc, argv, exception);
}

static
void
trace_n2p_deallocate(void *object)
{
    trace_info("[CLS] {full} %s object=%p\n", __func__+6, object);
    n2p_deallocate(object);
}
#endif // NDEBUG


const struct PPP_Class_Deprecated n2p_proxy_class = {
    .HasProperty =          TWRAP(n2p_has_property),
    .HasMethod =            TWRAP(n2p_has_method),
    .GetProperty =          TWRAP(n2p_get_property),
    .GetAllPropertyNames =  TWRAP(n2p_get_all_property_names),
    .SetProperty =          TWRAP(n2p_set_property),
    .RemoveProperty =       TWRAP(n2p_remove_property),
    .Call =                 TWRAP(n2p_call),
    .Construct =            TWRAP(n2p_construct),
    .Deallocate =           TWRAP(n2p_deallocate),
};
