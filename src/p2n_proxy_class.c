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
 * proxy class which wraps PP_Var object to provide NPObject interface
 */

#include "p2n_proxy_class.h"
#include "trace.h"
#include "tables.h"
#include <ppapi/c/private/ppp_instance_private.h>
#include <ppapi/c/pp_errors.h>
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include "ppb_message_loop.h"
#include "n2p_proxy_class.h"


NPObject *
p2n_allocate(NPP npp, NPClass *aClass)
{
    (void)npp;
    struct np_proxy_object_s *obj = npn.memalloc(sizeof(*obj));
    obj->npobj.referenceCount = 1;
    obj->npobj._class = aClass;
    obj->ppobj = PP_MakeUndefined();

    return (NPObject*)obj;
}

void
p2n_deallocate(NPObject *npobj)
{
    struct np_proxy_object_s *obj = (void *)npobj;
    if (--obj->npobj.referenceCount <= 0) {
        ppb_var_release(obj->ppobj);
        npn.memfree(npobj);
    }
}

void
p2n_invalidate(NPObject *npobj)
{
    // No-op.
    // According to spec, this call is always followed by deallocate(), so it's safe
    // to do nothing here.
}

struct has_method_param_s {
    NPObject           *npobj;
    char               *name;
    PP_Resource         m_loop;
    int                 depth;
    bool                result;
};

static
void
p2n_has_method_comt(void *user_data, int32_t result)
{
    struct has_method_param_s *p = user_data;
    struct np_proxy_object_s *obj = (void *)p->npobj;
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = ppb_var_var_from_utf8_z(p->name);

    p->result = ppb_var_has_method(obj->ppobj, method_name, &exception);

    ppb_var_release(method_name);
    ppb_var_release(exception);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
p2n_has_method_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(p2n_has_method_comt, user_data), PP_OK, __func__);
}

bool
p2n_has_method(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    if (npobj->_class == &p2n_proxy_class) {
        struct has_method_param_s *p = g_slice_alloc(sizeof(*p));
        p->npobj =      npobj;
        p->name =       npn.utf8fromidentifier(name);
        p->m_loop =     ppb_message_loop_get_for_browser_thread();
        p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

        ppb_message_loop_post_work_with_result(p->m_loop,
                                               PP_MakeCCB(p2n_has_method_prepare_comt, p), 0, PP_OK,
                                               0, __func__);
        ppb_message_loop_run_nested(p->m_loop);

        bool result = p->result;
        npn.memfree(p->name);
        g_slice_free1(sizeof(*p), p);
        return result;
    } else {
        return npobj->_class->hasMethod(npobj, name);
    }
}

struct invoke_param_s {
    NPObject               *npobj;
    char                   *name;
    const NPVariant        *args;
    uint32_t                argCount;
    NPVariant              *np_result;
    bool                    result;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
p2n_invoke_comt(void *user_data, int32_t result)
{
    struct invoke_param_s *p = user_data;
    unsigned int k;

    p->result = true;
    struct np_proxy_object_s *obj = (void *)p->npobj;
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = ppb_var_var_from_utf8_z(p->name);
    struct PP_Var res;

    struct PP_Var *pp_args = malloc(p->argCount * sizeof(*pp_args));
    for (k = 0; k < p->argCount; k ++) {
        pp_args[k] = np_variant_to_pp_var(p->args[k]);
    }

    res = ppb_var_call(obj->ppobj, method_name, p->argCount, pp_args, &exception);

    for (k = 0; k < p->argCount; k ++)
        ppb_var_release(pp_args[k]);
    free(pp_args);

    if (p->np_result) {
        *p->np_result = pp_var_to_np_variant(res);
        if (p->np_result->type == NPVariantType_Object) {
            NPP npp = tables_get_npobj_npp_mapping(p->npobj);
            tables_add_npobj_npp_mapping(p->np_result->value.objectValue, npp);
        }
    }

    ppb_var_release(res);
    ppb_var_release(method_name);
    ppb_var_release(exception);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
p2n_invoke_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(p2n_invoke_comt, user_data), PP_OK, __func__);
}

bool
p2n_invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,
           NPVariant *np_result)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    if (npobj->_class == &p2n_proxy_class) {
        struct invoke_param_s *p = g_slice_alloc(sizeof(*p));
        p->npobj =      npobj;
        p->name =       npn.utf8fromidentifier(name);
        p->args =       args;
        p->argCount =   argCount;
        p->np_result =  np_result;
        p->m_loop =     ppb_message_loop_get_for_browser_thread();
        p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

        ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(p2n_invoke_prepare_comt, p),
                                               0, PP_OK, 0, __func__);
        ppb_message_loop_run_nested(p->m_loop);
        bool result = p->result;
        npn.memfree(p->name);
        g_slice_free1(sizeof(*p), p);
        return result;
    } else {
        return npobj->_class->invoke(npobj, name, args, argCount, np_result);
    }
}

bool
p2n_invoke_default(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *np_result)
{
    return true;
}

struct has_property_param_s {
    NPObject           *npobj;
    char               *name;
    PP_Resource         m_loop;
    int                 depth;
    bool                result;
};

static
void
p2n_has_property_comt(void *user_data, int32_t result)
{
    struct has_property_param_s *p = user_data;

    struct np_proxy_object_s *obj = (void *)p->npobj;
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = ppb_var_var_from_utf8_z(p->name);

    p->result = ppb_var_has_property(obj->ppobj, property_name, &exception);
    ppb_var_release(property_name);
    ppb_var_release(exception);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
p2n_has_property_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(p2n_has_property_comt, user_data), PP_OK,
                                       __func__);
}

bool
p2n_has_property(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    if (npobj->_class == &p2n_proxy_class) {
        struct has_property_param_s *p = g_slice_alloc(sizeof(*p));
        p->npobj =      npobj;
        p->name =       npn.utf8fromidentifier(name);
        p->m_loop =     ppb_message_loop_get_for_browser_thread();
        p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

        ppb_message_loop_post_work_with_result(p->m_loop,
                                               PP_MakeCCB(p2n_has_property_prepare_comt, p), 0,
                                               PP_OK, 0, __func__);
        ppb_message_loop_run_nested(p->m_loop);

        bool result = p->result;
        npn.memfree(p->name);
        g_slice_free1(sizeof(*p), p);
        return result;
    } else {
        return npobj->_class->hasProperty(npobj, name);
    }
}

struct get_property_param_s {
    NPObject               *npobj;
    char                   *name;
    NPVariant              *np_result;
    bool                    result;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
p2n_get_property_comt(void *user_data, int32_t result)
{
    struct get_property_param_s *p = user_data;
    struct np_proxy_object_s *obj = (void *)p->npobj;
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = ppb_var_var_from_utf8_z(p->name);
    struct PP_Var res = ppb_var_get_property(obj->ppobj, property_name, &exception);

    p->result = true;
    *p->np_result = pp_var_to_np_variant(res);
    ppb_var_release(res);
    ppb_var_release(exception);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
p2n_get_property_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(p2n_get_property_comt, user_data), PP_OK,
                                       __func__);
}

bool
p2n_get_property(NPObject *npobj, NPIdentifier name, NPVariant *np_result)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    if (npobj->_class == &p2n_proxy_class) {
        struct get_property_param_s *p = g_slice_alloc(sizeof(*p));
        p->npobj =      npobj;
        p->name =       npn.utf8fromidentifier(name);
        p->np_result =  np_result;
        p->m_loop =     ppb_message_loop_get_for_browser_thread();
        p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

        ppb_message_loop_post_work_with_result(p->m_loop,
                                               PP_MakeCCB(p2n_get_property_prepare_comt, p), 0,
                                               PP_OK, 0, __func__);
        ppb_message_loop_run_nested(p->m_loop);
        bool result = p->result;
        npn.memfree(p->name);
        g_slice_free1(sizeof(*p), p);
        return result;
    } else {
        return npobj->_class->getProperty(npobj, name, np_result);
    }
}

bool
p2n_set_property(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    return true;
}

bool
p2n_remove_property(NPObject *npobj, NPIdentifier name)
{
    return true;
}

struct enumerate_param_s {
    NPObject           *npobj;
    uint32_t            count;
    struct PP_Var      *values;
    bool                result;
    PP_Resource         m_loop;
    int                 depth;
};

static
void
p2n_enumerate_comt(void *user_data, int32_t result)
{
    struct enumerate_param_s *p = user_data;
    struct np_proxy_object_s *obj = (void *)p->npobj;
    struct PP_Var exception;

    p->count = 0;
    p->values = NULL;
    p->result = true;

    ppb_var_get_all_property_names(obj->ppobj, &p->count, &p->values, &exception);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
p2n_enumerate_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(p2n_enumerate_comt, user_data), PP_OK, __func__);
}

bool
p2n_enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    if (npobj->_class == &p2n_proxy_class) {
        struct enumerate_param_s *p = g_slice_alloc(sizeof(*p));
        p->npobj =      npobj;
        p->m_loop =     ppb_message_loop_get_for_browser_thread();
        p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

        ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(p2n_enumerate_prepare_comt, p),
                                               0, PP_OK, 0, __func__);
        ppb_message_loop_run_nested(p->m_loop);
        bool result = p->result;
        *count = p->count;

        *value = npn.memalloc(p->count * sizeof(NPIdentifier));
        char *tmpbuf = malloc(1);
        for (uint32_t k = 0; k < p->count; k ++) {
            uint32_t len = 0;
            const char *s = ppb_var_var_to_utf8(p->values[k], &len);

            // make zero-terminated string
            char *ptr = realloc(tmpbuf, len + 1);
            if (!ptr) {
                result = false;
                goto err;
            }
            tmpbuf = ptr;
            memcpy(tmpbuf, s, len);
            tmpbuf[len] = 0;
            value[k] = npn.getstringidentifier(tmpbuf);
        }

    err:
        free(tmpbuf);
        g_slice_free1(sizeof(*p), p);

        return result;
    } else {
        return npobj->_class->enumerate(npobj, value, count);
    }
}

bool
p2n_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return false;
}


// trace wrappers
TRACE_WRAPPER
NPObject *
trace_p2n_allocate(NPP npp, NPClass *aClass)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return p2n_allocate(npp, aClass);
}

TRACE_WRAPPER
void
trace_p2n_deallocate(NPObject *npobj)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    p2n_deallocate(npobj);
}

TRACE_WRAPPER
void
trace_p2n_invalidate(NPObject *npobj)
{
    trace_info("[CLS] {full} %s npobj=%p\n", __func__+6, npobj);
    p2n_invalidate(npobj);
}

TRACE_WRAPPER
bool
trace_p2n_has_method(NPObject *npobj, NPIdentifier name)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s\n", __func__+6, npobj, s_name);
    npn.memfree(s_name);
    return p2n_has_method(npobj, name);
}

TRACE_WRAPPER
bool
trace_p2n_invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,
                 NPVariant *result)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s, args=%p, argCount=%u\n", __func__+6, npobj,
               s_name, args, argCount);
    npn.memfree(s_name);
    return p2n_invoke(npobj, name, args, argCount, result);
}

TRACE_WRAPPER
bool
trace_p2n_invoke_default(NPObject *npobj, const NPVariant *args, uint32_t argCount,
                         NPVariant *result)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return p2n_invoke_default(npobj, args, argCount, result);
}

TRACE_WRAPPER
bool
trace_p2n_has_property(NPObject *npobj, NPIdentifier name)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s\n", __func__+6, npobj, s_name);
    npn.memfree(s_name);
    return p2n_has_property(npobj, name);
}

TRACE_WRAPPER
bool
trace_p2n_get_property(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return p2n_get_property(npobj, name, result);
}

TRACE_WRAPPER
bool
trace_p2n_set_property(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return p2n_set_property(npobj, name, value);
}

TRACE_WRAPPER
bool
trace_p2n_remove_property(NPObject *npobj, NPIdentifier name)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return p2n_remove_property(npobj, name);
}

TRACE_WRAPPER
bool
trace_p2n_enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    trace_info("[CLS] {full} %s npobj=%p\n", __func__+6, npobj);
    return p2n_enumerate(npobj, value, count);
}

TRACE_WRAPPER
bool
trace_p2n_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return p2n_construct(npobj, args, argCount, result);
}


// can't be const due to npapi restrictions
struct NPClass p2n_proxy_class = {
    .structVersion =    NP_CLASS_STRUCT_VERSION,
    .allocate =         TWRAPF(p2n_allocate),
    .deallocate =       TWRAPF(p2n_deallocate),
    .invalidate =       TWRAPF(p2n_invalidate),
    .hasMethod =        TWRAPF(p2n_has_method),
    .invoke =           TWRAPF(p2n_invoke),
    .invokeDefault =    TWRAPZ(p2n_invoke_default),
    .hasProperty =      TWRAPF(p2n_has_property),
    .getProperty =      TWRAPF(p2n_get_property),
    .setProperty =      TWRAPZ(p2n_set_property),
    .removeProperty =   TWRAPZ(p2n_remove_property),
    .enumerate =        TWRAPF(p2n_enumerate),
    .construct =        TWRAPZ(p2n_construct),
};
