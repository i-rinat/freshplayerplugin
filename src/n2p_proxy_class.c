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


struct has_property_param_s {
    NPP                 npp;
    struct PP_Var       name;
    struct PP_Var      *exception;
    void               *object;
    bool                res;
    pthread_barrier_t   barrier;
};

static
void
_n2p_has_property_ptac(void *param)
{
    struct has_property_param_s *p = param;
    const char *s_name = ppb_var_var_to_utf8(p->name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);

    p->res = npn.hasproperty(p->npp, p->object, identifier);
    pthread_barrier_wait(&p->barrier);
}

static
bool
n2p_has_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, name is not a string\n", __func__);
        // TODO: fill exception
        return false;
    }

    struct has_property_param_s p;
    p.object = object;
    p.name = name;
    p.exception = exception;
    p.npp = tables_get_npobj_npp_mapping(object);

    pthread_barrier_init(&p.barrier, NULL, 2);
    npn.pluginthreadasynccall(p.npp, _n2p_has_property_ptac, &p);
    pthread_barrier_wait(&p.barrier);
    pthread_barrier_destroy(&p.barrier);

    return p.res;
}

static
bool
n2p_has_method(void *object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

struct get_property_param_s {
    NPP                 npp;
    void               *object;
    struct PP_Var       name;
    struct PP_Var      *exception;
    struct PP_Var       res;
    pthread_barrier_t   barrier;
};

static
void
_n2p_get_property_ptac(void *param)
{
    struct get_property_param_s *p = param;
    const char *s_name = ppb_var_var_to_utf8(p->name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);
    NPVariant np_value;

    if (npn.getproperty(p->npp, p->object, identifier, &np_value)) {
        struct PP_Var var = np_variant_to_pp_var(np_value);

        if (np_value.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_value.value.objectValue, p->npp);
        else
            npn.releasevariantvalue(&np_value);

        p->res = var;
    } else {
        p->res = PP_MakeUndefined();
    }
    pthread_barrier_wait(&p->barrier);
}

static
struct PP_Var
n2p_get_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    if (name.type != PP_VARTYPE_STRING) {
        trace_error("%s, name is not a string\n", __func__);
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct get_property_param_s p;
    p.npp = tables_get_npobj_npp_mapping(object);
    p.object = object;
    p.name = name;
    p.exception = exception;

    pthread_barrier_init(&p.barrier, NULL, 2);
    npn.pluginthreadasynccall(p.npp, _n2p_get_property_ptac, &p);
    pthread_barrier_wait(&p.barrier);
    pthread_barrier_destroy(&p.barrier);

    return p.res;
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

struct call_param_s {
    NPP                 npp;
    void               *object;
    struct PP_Var       method_name;
    uint32_t            argc;
    struct PP_Var      *argv;
    struct PP_Var      *exception;
    struct PP_Var       res;
    pthread_barrier_t   barrier;
};

static
void
_n2p_call_ptac(void *param)
{
    struct call_param_s *p = param;
    const char *s_method_name = ppb_var_var_to_utf8(p->method_name, NULL);
    NPIdentifier np_method_name = npn.getstringidentifier(s_method_name);

    NPVariant *np_args = malloc(p->argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < p->argc; k ++)
        np_args[k] = pp_var_to_np_variant(p->argv[k]);

    NPVariant np_result;
    bool res = npn.invoke(p->npp, p->object, np_method_name, np_args, p->argc, &np_result);

    for (uint32_t k = 0; k < p->argc; k ++)
        npn.releasevariantvalue(&np_args[k]);
    free(np_args);

    if (res) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, p->npp);
        else
            npn.releasevariantvalue(&np_result);

        p->res = var;
    } else {
        p->res = PP_MakeUndefined();
    }

    pthread_barrier_wait(&p->barrier);
}

static
struct PP_Var
n2p_call(void *object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
         struct PP_Var *exception)
{
    if (method_name.type != PP_VARTYPE_STRING) {
        trace_error("%s, method_name is not a string\n", __func__);
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct call_param_s p;
    p.npp = tables_get_npobj_npp_mapping(object);
    p.object =      object;
    p.method_name = method_name;
    p.argc =        argc;
    p.argv =        argv;
    p.exception =   exception;

    pthread_barrier_init(&p.barrier, NULL, 2);
    npn.pluginthreadasynccall(p.npp, _n2p_call_ptac, &p);
    pthread_barrier_wait(&p.barrier);
    pthread_barrier_destroy(&p.barrier);

    return p.res;
}

struct construct_param_s {
    NPP                 npp;
    void               *object;
    uint32_t            argc;
    struct PP_Var      *argv;
    struct PP_Var      *exception;
    struct PP_Var       res;
    pthread_barrier_t   barrier;
};

static
void
_n2p_construct_ptac(void *param)
{
    struct construct_param_s *p = param;

    NPVariant *np_args = malloc(p->argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < p->argc; k ++)
        np_args[k] = pp_var_to_np_variant(p->argv[k]);

    NPVariant np_result;
    bool res = npn.construct(p->npp, p->object, np_args, p->argc, &np_result);
    for (uint32_t k = 0; k < p->argc; k ++)
        npn.releasevariantvalue(&np_args[k]);
    free(np_args);

    if (res) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, p->npp);
        else
            npn.releasevariantvalue(&np_result);

        p->res = var;
    } else {
        p->res = PP_MakeUndefined();
    }

    pthread_barrier_wait(&p->barrier);
}

static
struct PP_Var
n2p_construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    struct construct_param_s p;
    p.npp =         tables_get_npobj_npp_mapping(object);
    p.object =      object;
    p.argc =        argc;
    p.argv =        argv;
    p.exception =   exception;

    pthread_barrier_init(&p.barrier, NULL, 2);
    npn.pluginthreadasynccall(p.npp, _n2p_construct_ptac, &p);
    pthread_barrier_wait(&p.barrier);
    pthread_barrier_destroy(&p.barrier);

    return p.res;
}

static
void
n2p_deallocate(void *object)
{
    npn.releaseobject(object);
}


// trace wrappers
TRACE_WRAPPER
bool
trace_n2p_has_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {full} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_has_property(object, name, exception);
}

TRACE_WRAPPER
bool
trace_n2p_has_method(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {zilch} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_has_method(object, name, exception);
}

TRACE_WRAPPER
struct PP_Var
trace_n2p_get_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {full} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    return n2p_get_property(object, name, exception);
}

TRACE_WRAPPER
void
trace_n2p_get_all_property_names(void *object, uint32_t *property_count, struct PP_Var **properties,
                                 struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s object=%p\n", __func__+6, object);
    n2p_get_all_property_names(object, property_count, properties, exception);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_n2p_remove_property(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {zilch} %s object=%p, name=%s\n", __func__+6, object, s_name);
    g_free(s_name);
    n2p_remove_property(object, name, exception);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
struct PP_Var
trace_n2p_construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    trace_info("[CLS] {full} %s object=%p, argc=%u, argv=%p\n", __func__+6, object, argc, argv);
    return n2p_construct(object, argc, argv, exception);
}

TRACE_WRAPPER
void
trace_n2p_deallocate(void *object)
{
    trace_info("[CLS] {full} %s object=%p\n", __func__+6, object);
    n2p_deallocate(object);
}


const struct PPP_Class_Deprecated n2p_proxy_class = {
    .HasProperty =          TWRAPF(n2p_has_property),
    .HasMethod =            TWRAPZ(n2p_has_method),
    .GetProperty =          TWRAPF(n2p_get_property),
    .GetAllPropertyNames =  TWRAPZ(n2p_get_all_property_names),
    .SetProperty =          TWRAPZ(n2p_set_property),
    .RemoveProperty =       TWRAPZ(n2p_remove_property),
    .Call =                 TWRAPF(n2p_call),
    .Construct =            TWRAPF(n2p_construct),
    .Deallocate =           TWRAPF(n2p_deallocate),
};
