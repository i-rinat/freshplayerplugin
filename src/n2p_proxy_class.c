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

/*
 * proxy class which wraps around NPObject while providing PP_Var object
 */

#include "n2p_proxy_class.h"
#include "ppapi/c/dev/deprecated_bool.h"
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include <npapi/npruntime.h>
#include "ppb_core.h"
#include "ppb_var.h"
#include "ppb_message_loop.h"
#include <ppapi/c/pp_errors.h>


struct has_property_param_s {
    struct PP_Var       name;
    struct PP_Var      *exception;
    void               *object;
    bool                result;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
n2p_has_property_ptac(void *param)
{
    struct has_property_param_s *p = param;
    const char *s_name = ppb_var_var_to_utf8(p->name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);
    NPP npp = tables_get_npobj_npp_mapping(p->object);

    if (npp)
        p->result = npn.hasproperty(npp, p->object, identifier);
    else
        p->result = false;
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
n2p_has_property_comt(void *user_data, int32_t result)
{
    struct has_property_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, n2p_has_property_ptac, p);
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

    struct has_property_param_s *p = g_slice_alloc(sizeof(*p));
    p->object =     object;
    p->name =       name;
    p->exception =  exception;
    p->m_loop =     ppb_message_loop_get_current();
    p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(n2p_has_property_comt, p), 0,
                                           PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    bool result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}

static
bool
n2p_has_method(void *object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

struct get_property_param_s {
    void           *object;
    struct PP_Var   name;
    struct PP_Var  *exception;
    struct PP_Var   result;
    PP_Resource     m_loop;
    int             depth;
};

static
void
n2p_get_property_ptac(void *param)
{
    struct get_property_param_s *p = param;
    const char *s_name = ppb_var_var_to_utf8(p->name, NULL);
    NPIdentifier identifier = npn.getstringidentifier(s_name);
    NPVariant np_value;
    NPP npp = tables_get_npobj_npp_mapping(p->object);

    if (npp && npn.getproperty(npp, p->object, identifier, &np_value)) {
        struct PP_Var var = np_variant_to_pp_var(np_value);

        if (np_value.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_value.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_value);

        p->result = var;
    } else {
        p->result = PP_MakeUndefined();
    }
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
n2p_get_property_comt(void *user_data, int32_t result)
{
    struct get_property_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, n2p_get_property_ptac, p);
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

    struct get_property_param_s *p = g_slice_alloc(sizeof(*p));
    p->object =     object;
    p->name =       name;
    p->exception =  exception;
    p->m_loop =     ppb_message_loop_get_current();
    p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(n2p_get_property_comt, p), 0,
                                           PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
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
    void               *object;
    struct PP_Var       method_name;
    uint32_t            argc;
    struct PP_Var      *argv;
    struct PP_Var      *exception;
    struct PP_Var       result;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
n2p_call_ptac(void *param)
{
    struct call_param_s *p = param;
    const char *s_method_name = ppb_var_var_to_utf8(p->method_name, NULL);
    NPIdentifier np_method_name = npn.getstringidentifier(s_method_name);
    NPP npp = tables_get_npobj_npp_mapping(p->object);

    NPVariant *np_args = malloc(p->argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < p->argc; k ++)
        np_args[k] = pp_var_to_np_variant(p->argv[k]);

    NPVariant np_result;
    bool res = npp ? npn.invoke(npp, p->object, np_method_name, np_args, p->argc, &np_result)
                   : FALSE;

    for (uint32_t k = 0; k < p->argc; k ++)
        npn.releasevariantvalue(&np_args[k]);
    free(np_args);

    if (res) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_result);

        p->result = var;
    } else {
        p->result = PP_MakeUndefined();
    }

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
n2p_call_comt(void *user_data, int32_t result)
{
    struct call_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, n2p_call_ptac, p);
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

    struct call_param_s *p = g_slice_alloc(sizeof(*p));
    p->object =         object;
    p->method_name =    method_name;
    p->argc =           argc;
    p->argv =           argv;
    p->exception =      exception;
    p->m_loop =         ppb_message_loop_get_current();
    p->depth =          ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(n2p_call_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}

struct construct_param_s {
    void               *object;
    uint32_t            argc;
    struct PP_Var      *argv;
    struct PP_Var      *exception;
    struct PP_Var       result;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
n2p_construct_ptac(void *param)
{
    struct construct_param_s *p = param;
    NPP npp = tables_get_npobj_npp_mapping(p->object);

    NPVariant *np_args = malloc(p->argc * sizeof(NPVariant));
    for (uint32_t k = 0; k < p->argc; k ++)
        np_args[k] = pp_var_to_np_variant(p->argv[k]);

    NPVariant np_result;
    bool res = npp ? npn.construct(npp, p->object, np_args, p->argc, &np_result) : FALSE;

    for (uint32_t k = 0; k < p->argc; k ++)
        npn.releasevariantvalue(&np_args[k]);
    free(np_args);

    if (res) {
        struct PP_Var var = np_variant_to_pp_var(np_result);

        if (np_result.type == NPVariantType_Object)
            tables_add_npobj_npp_mapping(np_result.value.objectValue, npp);
        else
            npn.releasevariantvalue(&np_result);

        p->result = var;
    } else {
        p->result = PP_MakeUndefined();
    }

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
n2p_construct_comt(void *user_data, int32_t result)
{
    struct construct_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, n2p_construct_ptac, p);
}

static
struct PP_Var
n2p_construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    struct construct_param_s *p = g_slice_alloc(sizeof(*p));
    p->object =     object;
    p->argc =       argc;
    p->argv =       argv;
    p->exception =  exception;
    p->m_loop =     ppb_message_loop_get_current();
    p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(n2p_construct_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}

struct deallocate_param_s {
    void               *object;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
n2p_deallocate_ptac(void *param)
{
    struct deallocate_param_s *p = param;

    NPObject *np_object = p->object;
    uint32_t ref_cnt = np_object->referenceCount;

    npn.releaseobject(p->object);

    if (ref_cnt <= 1)
        tables_remove_npobj_npp_mapping(p->object);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
n2p_deallocate_comt(void *user_data, int32_t result)
{
    struct deallocate_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, n2p_deallocate_ptac, p);
}

static
void
n2p_deallocate(void *object)
{
    if (!object) {
        trace_warning("%s, object == NULL\n", __func__);
        return;
    }

    struct deallocate_param_s *p = g_slice_alloc(sizeof(*p));
    p->object = object;
    p->m_loop = ppb_message_loop_get_current();
    p->depth =  ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(n2p_deallocate_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    g_slice_free1(sizeof(*p), p);
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
