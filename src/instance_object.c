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

#include "instance_object.h"
#include "trace.h"
#include "tables.h"
#include <ppapi/c/private/ppp_instance_private.h>
#include "pp_interface.h"
#include "ppb_var.h"
#include "ppb_var_deprecated.h"


struct instance_object_s {
    NPObject npobj;
    struct PP_Var ppobj;
};


NPObject *
iobj_allocate(NPP npp, NPClass *aClass)
{
    (void)npp;
    struct instance_object_s *obj = npn.memalloc(sizeof(*obj));
    obj->npobj.referenceCount = 1;
    obj->npobj._class = aClass;
    obj->ppobj = PP_MakeUndefined();

    return (NPObject*)obj;
}

void
iobj_deallocate(NPObject *npobj)
{
    struct instance_object_s *obj = (void *)npobj;
    if (--obj->npobj.referenceCount <= 0) {
        ppb_var_release(obj->ppobj);
        npn.memfree(npobj);
    }
}

void
iobj_invalidate(NPObject *npobj)
{
}

bool
iobj_has_method(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name))
        return false;

    struct instance_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = PP_MakeString(s);
    npn.memfree(s);
    bool res = ppb_var_deprecated_has_method(obj->ppobj, method_name, &exception);
    ppb_var_release(method_name);
    ppb_var_release(exception);

    return res;
}

bool
iobj_invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,
            NPVariant *result)
{
    if (!npn.identifierisstring(name))
        return false;

    unsigned int k;
    struct instance_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var method_name = PP_MakeString(s);
    struct PP_Var res;
    npn.memfree(s);

    struct PP_Var *pp_args = malloc(argCount * sizeof(*pp_args));
    for (k = 0; k < argCount; k ++) {
        // TODO: objects
        pp_args[k] = np_variant_to_pp_var(args[k], NULL);
    }

    res = ppb_var_deprecated_call(obj->ppobj, method_name, argCount, pp_args, &exception);

    for (k = 0; k < argCount; k ++)
        ppb_var_release(pp_args[k]);
    free(pp_args);

    if (result)
        *result = pp_var_to_np_variant(res);

    ppb_var_release(res);
    ppb_var_release(method_name);
    ppb_var_release(exception);

    return true;
}

bool
iobj_invoke_default(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    return true;
}

bool
iobj_has_property(NPObject *npobj, NPIdentifier name)
{
    if (!npn.identifierisstring(name))
        return false;

    struct instance_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = PP_MakeString(s);
    npn.memfree(s);
    bool res = ppb_var_deprecated_has_property(obj->ppobj, property_name, &exception);
    ppb_var_release(property_name);
    ppb_var_release(exception);

    return res;
}

bool
iobj_get_property(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    if (!npn.identifierisstring(name))
        return false;

    struct instance_object_s *obj = (void *)npobj;
    char *s = npn.utf8fromidentifier(name);
    struct PP_Var exception = PP_MakeUndefined();
    struct PP_Var property_name = PP_MakeString(s);
    npn.memfree(s);
    struct PP_Var res = ppb_var_deprecated_get_property(obj->ppobj, property_name, &exception);

    *result = pp_var_to_np_variant(res);
    ppb_var_release(res);
    ppb_var_release(exception);
    return true;
}

bool
iobj_set_property(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    return true;
}

bool
iobj_remove_property(NPObject *npobj, NPIdentifier name)
{
    return true;
}

bool
iobj_enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    return true;
}

bool
iobj_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    (void)result;
    if (argCount != 1001) {
        // TODO: figure out how to rid of magic numbers
        trace_warning("%s, wrong magic %d\n", __func__, argCount);
        return false;
    }

    struct PP_Var var = *(struct PP_Var *)args;
    struct instance_object_s *obj = (void *)npobj;
    tables_ref_var(var);
    obj->ppobj = var;

    if (result) {
        result->type = NPVariantType_Object;
        result->value.objectValue = npobj;
    }

    return true;
}


// trace wrappers
NPObject *
trace_iobj_allocate(NPP npp, NPClass *aClass)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return iobj_allocate(npp, aClass);
}

void
trace_iobj_deallocate(NPObject *npobj)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    iobj_deallocate(npobj);
}

void
trace_iobj_invalidate(NPObject *npobj)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    iobj_invalidate(npobj);
}

bool
trace_iobj_has_method(NPObject *npobj, NPIdentifier name)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s\n", __func__+6, npobj, s_name);
    npn.memfree(s_name);
    return iobj_has_method(npobj, name);
}

bool
trace_iobj_invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount,
                  NPVariant *result)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s, args=%p, argCount=%u\n", __func__+6, npobj,
               s_name, args, argCount);
    npn.memfree(s_name);
    return iobj_invoke(npobj, name, args, argCount, result);
}

bool
trace_iobj_invoke_default(NPObject *npobj, const NPVariant *args, uint32_t argCount,
                          NPVariant *result)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return iobj_invoke_default(npobj, args, argCount, result);
}

bool
trace_iobj_has_property(NPObject *npobj, NPIdentifier name)
{
    char *s_name = npn.utf8fromidentifier(name);
    trace_info("[CLS] {full} %s npobj=%p, name=%s\n", __func__+6, npobj, s_name);
    npn.memfree(s_name);
    return iobj_has_property(npobj, name);
}

bool
trace_iobj_get_property(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return iobj_get_property(npobj, name, result);
}

bool
trace_iobj_set_property(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return iobj_set_property(npobj, name, value);
}

bool
trace_iobj_remove_property(NPObject *npobj, NPIdentifier name)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return iobj_remove_property(npobj, name);
}

bool
trace_iobj_enumerate(NPObject *npobj, NPIdentifier **value, uint32_t *count)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return iobj_enumerate(npobj, value, count);
}

bool
trace_iobj_construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    return iobj_construct(npobj, args, argCount, result);
}

// can't be const due to npapi restrictions
struct NPClass instance_object_class = {
    .structVersion =    NP_CLASS_STRUCT_VERSION,
    .allocate =         trace_iobj_allocate,
    .deallocate =       trace_iobj_deallocate,
    .invalidate =       trace_iobj_invalidate,
    .hasMethod =        trace_iobj_has_method,
    .invoke =           trace_iobj_invoke,
    .invokeDefault =    trace_iobj_invoke_default,
    .hasProperty =      trace_iobj_has_property,
    .getProperty =      trace_iobj_get_property,
    .setProperty =      trace_iobj_set_property,
    .removeProperty =   trace_iobj_remove_property,
    .enumerate =        trace_iobj_enumerate,
    .construct =        trace_iobj_construct,
};
