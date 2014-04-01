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

#include "browser_object.h"
#include "ppapi/c/dev/deprecated_bool.h"
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include <npruntime.h>
#include "ppb_var.h"


static
bool
bobj_HasProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

static
bool
bobj_HasMethod(void *object, struct PP_Var name, struct PP_Var *exception)
{
    return false;
}

static
struct PP_Var
bobj_GetProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
    if (name.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    const char *s_name = ppb_var_var_to_utf8(name, NULL);
    const struct pp_var_object_s *obj = object;
    NPIdentifier identifier = npn.getstringidentifier(s_name);
    NPVariant np_value;
    if (npn.getproperty(obj->npp, obj->data, identifier, &np_value)) {
        struct PP_Var var = np_variant_to_pp_var(np_value, obj);
        if (var.type != PP_VARTYPE_OBJECT)
            npn.releasevariantvalue(&np_value);
        return var;
    } else {
        return PP_MakeUndefined();
    }
}

static
void
bobj_GetAllPropertyNames(void *object, uint32_t *property_count, struct PP_Var **properties, 
                         struct PP_Var *exception)
{
}

static
void
bobj_SetProperty(void *object, struct PP_Var name, struct PP_Var value, struct PP_Var *exception)
{
}

static
void
bobj_RemoveProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
}

static
struct PP_Var
bobj_Call(void *object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
          struct PP_Var *exception)
{
    return PP_MakeUndefined();
}

static
struct PP_Var
bobj_Construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    return PP_MakeUndefined();
}

static
void
bobj_Deallocate(void *object)
{
    const struct pp_var_object_s *obj = object;
    NPVariant np_val;
    np_val.type = NPVariantType_Object;
    np_val.value.objectValue = obj->data;
    npn.releasevariantvalue(&np_val);
}


// trace wrappers
static
bool
trace_bobj_HasProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return bobj_HasProperty(object, name, exception);
}

static
bool
trace_bobj_HasMethod(void *object, struct PP_Var name, struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return bobj_HasMethod(object, name, exception);
}

static
struct PP_Var
trace_bobj_GetProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
    char *s_name = trace_var_as_string(name);
    trace_info("[CLS] {full} %s object=%p, name=%s\n", __func__+6, object, s_name);
    free(s_name);
    return bobj_GetProperty(object, name, exception);
}

static
void
trace_bobj_GetAllPropertyNames(void *object, uint32_t *property_count, struct PP_Var **properties, 
                               struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    bobj_GetAllPropertyNames(object, property_count, properties, exception);
}

static
void
trace_bobj_SetProperty(void *object, struct PP_Var name, struct PP_Var value,
                       struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    bobj_SetProperty(object, name, value, exception);
}

static
void
trace_bobj_RemoveProperty(void *object, struct PP_Var name, struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    bobj_RemoveProperty(object, name, exception);
}

static
struct PP_Var
trace_bobj_Call(void *object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
                struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return bobj_Call(object, method_name, argc, argv, exception);
}

static
struct PP_Var
trace_bobj_Construct(void *object, uint32_t argc, struct PP_Var *argv, struct PP_Var *exception)
{
    trace_info("[CLS] {zilch} %s\n", __func__+6);
    return bobj_Construct(object, argc, argv, exception);
}

static
void
trace_bobj_Deallocate(void *object)
{
    trace_info("[CLS] {full} %s\n", __func__+6);
    bobj_Deallocate(object);
}


const struct PPP_Class_Deprecated browser_object_class = {
    .HasProperty =          trace_bobj_HasProperty,
    .HasMethod =            trace_bobj_HasMethod,
    .GetProperty =          trace_bobj_GetProperty,
    .GetAllPropertyNames =  trace_bobj_GetAllPropertyNames,
    .SetProperty =          trace_bobj_SetProperty,
    .RemoveProperty =       trace_bobj_RemoveProperty,
    .Call =                 trace_bobj_Call,
    .Construct =            trace_bobj_Construct,
    .Deallocate =           trace_bobj_Deallocate,
};
