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

#include <ppapi/c/private/ppb_instance_private.h>
#include <ppapi/c/pp_var.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "globals.h"
#include "browser_object.h"


struct PP_Var
ppb_instance_private_get_window_object(PP_Instance instance)
{
    struct PP_Var var = {0};
    struct pp_var_object_s *obj;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    NPObject *np_window_obj;

    if (!pp_i) {
        trace_error("%s, wrong instance %d\n", __func__, instance);
        return var;
    }

    NPError err = npn.getvalue(pp_i->npp, NPNVWindowNPObject, &np_window_obj);
    if (err != NPERR_NO_ERROR) {
        trace_error("%s, NPN_GetValue returned %d\n", __func__, err);
        return var;
    }

    obj = malloc(sizeof(*obj));
    obj->klass = &browser_object_class;
    obj->data = np_window_obj;
    obj->npp =  pp_i->npp;

    var.type = PP_VARTYPE_OBJECT;
    var.value.as_id = (size_t)(void *)obj;
    tables_ref_var(var);

    return var;
}

struct PP_Var
ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    struct PP_Var var = {0};
    return var;
}

struct PP_Var
ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                    struct PP_Var *exception)
{
    struct PP_Var var = {0};
    return var;
}

// trace wrappers
static
struct PP_Var
trace_ppb_instance_private_get_window_object(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_private_get_window_object(instance);
}

static
struct PP_Var
trace_ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_private_get_owner_element_object(instance);
}

static
struct PP_Var
trace_ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                          struct PP_Var *exception)
{
    char *s_script = trace_var_as_string(script);
    trace_info("[PPB] {zilch} %s instance=%d, script=%s\n", __func__+6, instance, s_script);
    free(s_script);
    return ppb_instance_private_execute_script(instance, script, exception);
}


const struct PPB_Instance_Private_0_1 ppb_instance_private_interface_0_1 = {
    .GetWindowObject =          trace_ppb_instance_private_get_window_object,
    .GetOwnerElementObject =    trace_ppb_instance_private_get_owner_element_object,
    .ExecuteScript =            trace_ppb_instance_private_execute_script,
};
