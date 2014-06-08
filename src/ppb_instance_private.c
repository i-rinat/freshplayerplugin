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

#include "ppb_instance_private.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "n2p_proxy_class.h"


struct PP_Var
ppb_instance_private_get_window_object(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    NPObject *np_window_obj;

    if (!pp_i) {
        trace_error("%s, wrong instance %d\n", __func__, instance);
        return PP_MakeUndefined();
    }

    NPError err = npn.getvalue(pp_i->npp, NPNVWindowNPObject, &np_window_obj);
    if (err != NPERR_NO_ERROR) {
        trace_error("%s, NPN_GetValue returned %d\n", __func__, err);
        return PP_MakeUndefined();
    }
    tables_add_npobj_npp_mapping(np_window_obj, pp_i->npp);

    return PP_MakeBrowserObject(np_window_obj, NULL);
}

struct PP_Var
ppb_instance_private_get_owner_element_object(PP_Instance instance)
{
    return PP_MakeUndefined();
}

struct execute_script_param_s {
    pthread_barrier_t   barrier;
    int                 should_wait;
    struct PP_Var       script;
    struct PP_Var       result;
    NPP                 npp;
};

static
void
_execute_script_comt(void *p)
{
    struct execute_script_param_s *esp = p;
    NPError err;
    NPObject *np_window_obj;
    NPString  np_script;
    NPVariant np_result;

    err = npn.getvalue(esp->npp, NPNVWindowNPObject, &np_window_obj);
    if (err != NPERR_NO_ERROR) {
        trace_error("%s, NPN_GetValue failed with code %d\n", __func__, err);
        esp->result = PP_MakeUndefined();
        goto quit;
    }

    np_script.UTF8Characters = ppb_var_var_to_utf8(esp->script, &np_script.UTF8Length);
    if (!npn.evaluate(esp->npp, np_window_obj, &np_script, &np_result)) {
        trace_error("%s, NPN_Evaluate failed\n", __func__);
        esp->result = PP_MakeUndefined();
        goto quit;
    }

    // TODO: find out what exception is
    esp->result = np_variant_to_pp_var(np_result);
    if (np_result.type == NPVariantType_Object)
        tables_add_npobj_npp_mapping(np_result.value.objectValue, esp->npp);
    else
        npn.releasevariantvalue(&np_result);
quit:
    if (esp->should_wait)
        pthread_barrier_wait(&esp->barrier);
}

struct PP_Var
ppb_instance_private_execute_script(PP_Instance instance, struct PP_Var script,
                                    struct PP_Var *exception)
{
    struct pp_instance_s *pp_i;

    if (script.type != PP_VARTYPE_STRING) {
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    pp_i = tables_get_pp_instance(instance);

    struct execute_script_param_s *esp = calloc(1, sizeof(*esp));
    esp->script = script;
    esp->npp = pp_i->npp;

    if (ppb_core_is_main_thread()) {
        esp->should_wait = 0;
        //_execute_script_comt(esp, 0);
        _execute_script_comt(esp);
    } else {
        esp->should_wait = 1;
        pthread_barrier_init(&esp->barrier, NULL, 2);
        npn.pluginthreadasynccall(esp->npp, _execute_script_comt, esp);
        pthread_barrier_wait(&esp->barrier);
        pthread_barrier_destroy(&esp->barrier);
    }

    struct PP_Var result = esp->result;
    free(esp);

    return result;
}


#ifndef NDEBUG
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
    trace_info("[PPB] {full} %s instance=%d, script=%s\n", __func__+6, instance, s_script);
    g_free(s_script);
    return ppb_instance_private_execute_script(instance, script, exception);
}
#endif // NDEBUG


const struct PPB_Instance_Private_0_1 ppb_instance_private_interface_0_1 = {
    .GetWindowObject =          TWRAP(ppb_instance_private_get_window_object),
    .GetOwnerElementObject =    TWRAP(ppb_instance_private_get_owner_element_object),
    .ExecuteScript =            TWRAP(ppb_instance_private_execute_script),
};
