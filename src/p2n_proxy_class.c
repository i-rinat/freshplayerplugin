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
}

bool
p2n_has_method(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    struct np_proxy_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = PP_MakeString(s);
    npn.memfree(s);
    bool res = ppb_var_has_method(obj->ppobj, method_name, &exception);
    ppb_var_release(method_name);
    ppb_var_release(exception);

    return res;
}

struct invoke_param_s {
    NPObject               *npobj;
    NPIdentifier            name;
    const NPVariant        *args;
    uint32_t                argCount;
    NPVariant              *result;
    pthread_barrier_t       barrier;
};

static
void
_p2n_invoke_comt(void *user_data, int32_t result)
{
    struct invoke_param_s *p = user_data;

    unsigned int k;
    struct np_proxy_object_s *obj = (void *)p->npobj;
    char *s = npn.utf8fromidentifier(p->name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = PP_MakeString(s);
    struct PP_Var res;
    npn.memfree(s);

    struct PP_Var *pp_args = malloc(p->argCount * sizeof(*pp_args));
    for (k = 0; k < p->argCount; k ++) {
        pp_args[k] = np_variant_to_pp_var(p->args[k]);
    }

    res = ppb_var_call(obj->ppobj, method_name, p->argCount, pp_args, &exception);

    for (k = 0; k < p->argCount; k ++)
        ppb_var_release(pp_args[k]);
    free(pp_args);

    if (p->result) {
        *p->result = pp_var_to_np_variant(res);
        if (p->result->type == NPVariantType_Object) {
            NPP npp = tables_get_npobj_npp_mapping(p->npobj);
            tables_add_npobj_npp_mapping(p->result->value.objectValue, npp);
        }
    }

    ppb_var_release(res);
    ppb_var_release(method_name);
    ppb_var_release(exception);

    pthread_barrier_wait(&p->barrier);
}

bool
p2n_invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,
           NPVariant *result)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    if (npobj->_class == &p2n_proxy_class) {
        struct invoke_param_s p;
        p.npobj =       npobj;
        p.name =        name;
        p.args =        args;
        p.argCount =    argCount;
        p.result =      result;

        pthread_barrier_init(&p.barrier, NULL, 2);
        ppb_core_call_on_main_thread(0, PP_MakeCompletionCallback(_p2n_invoke_comt, &p), PP_OK);
        pthread_barrier_wait(&p.barrier);
        pthread_barrier_destroy(&p.barrier);

        return true;
    } else {
        return npobj->_class->invoke(npobj, name, args, argCount, result);
    }
}

bool
p2n_invoke_default(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}

bool
p2n_has_property(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    struct np_proxy_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = PP_MakeString(s);
    npn.memfree(s);
    bool res = ppb_var_has_property(obj->ppobj, property_name, &exception);
    ppb_var_release(property_name);
    ppb_var_release(exception);

    return res;
}

bool
p2n_get_property(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    if (!npn.identifierisstring(name)) {
        trace_error("%s, name is not a string\n", __func__);
        return false;
    }

    struct np_proxy_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = PP_MakeString(s);
    npn.memfree(s);
    struct PP_Var res = ppb_var_get_property(obj->ppobj, property_name, &exception);

    *result = pp_var_to_np_variant(res);
    ppb_var_release(res);
    ppb_var_release(exception);
    return true;
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

bool
p2n_enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    return true;
}

bool
p2n_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    (void)result;
    if (argCount != 1001) {
        // TODO: figure out how to rid of magic numbers
        trace_warning("%s, wrong magic %d\n", __func__, argCount);
        return false;
    }

    struct PP_Var var = *(struct PP_Var *)args;
    struct np_proxy_object_s *obj = (void *)npobj;
    tables_ref_var(var);
    obj->ppobj = var;

    if (result) {
        result->type = NPVariantType_Object;
        result->value.objectValue = npobj;
    }

    return true;
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
    trace_info("[CLS] {zilch} %s\n", __func__+6);
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
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return p2n_enumerate(npobj, value, count);
}

TRACE_WRAPPER
bool
trace_p2n_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return p2n_construct(npobj, args, argCount, result);
}


// can't be const due to npapi restrictions
struct NPClass p2n_proxy_class = {
    .structVersion =    NP_CLASS_STRUCT_VERSION,
    .allocate =         TWRAPF(p2n_allocate),
    .deallocate =       TWRAPF(p2n_deallocate),
    .invalidate =       TWRAPZ(p2n_invalidate),
    .hasMethod =        TWRAPF(p2n_has_method),
    .invoke =           TWRAPF(p2n_invoke),
    .invokeDefault =    TWRAPZ(p2n_invoke_default),
    .hasProperty =      TWRAPF(p2n_has_property),
    .getProperty =      TWRAPF(p2n_get_property),
    .setProperty =      TWRAPZ(p2n_set_property),
    .removeProperty =   TWRAPZ(p2n_remove_property),
    .enumerate =        TWRAPZ(p2n_enumerate),
    .construct =        TWRAPF(p2n_construct),
};
