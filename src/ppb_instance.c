/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "n2p_proxy_class.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_core.h"
#include "ppb_graphics2d.h"
#include "ppb_graphics3d.h"
#include "ppb_instance.h"
#include "ppb_message_loop.h"
#include "ppb_var.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include "utils.h"
#include <ppapi/c/pp_errors.h>
#include <pthread.h>

STATIC_ASSERT(sizeof(struct pp_instance_s) <= LARGEST_RESOURCE_SIZE);

static
void
call_invalidaterect_ptac(void *param)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(GPOINTER_TO_SIZE(param));
    if (!pp_i)
        return;
    NPRect npr = {.top = 0, .left = 0, .bottom = pp_i->height, .right = pp_i->width};

    npn.invalidaterect(pp_i->npp, &npr);
    npn.forceredraw(pp_i->npp);
}

PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    PP_Bool retval;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    if (device == 0) {
        // unbind
        ppb_core_release_resource(pp_i->graphics);
        pthread_mutex_lock(&display.lock);
        pp_i->graphics = 0;
        pthread_mutex_unlock(&display.lock);
        return PP_TRUE;
    }

    struct pp_graphics2d_s *g2d = pp_resource_acquire(device, PP_RESOURCE_GRAPHICS2D);
    struct pp_graphics3d_s *g3d = pp_resource_acquire(device, PP_RESOURCE_GRAPHICS3D);
    int invalidate_area = 0;

    if (g2d) {
        if (pp_i != g2d->instance) {
            retval = PP_FALSE;
            goto done;
        }

        pthread_mutex_lock(&display.lock);
        const PP_Resource previous_device = pp_i->graphics;
        pp_i->graphics = device;
        pthread_mutex_unlock(&display.lock);

        if (device != previous_device) {
            ppb_core_add_ref_resource(device);
            ppb_core_release_resource(previous_device);
        }

        pp_resource_release(device);
        retval = PP_TRUE;
        invalidate_area = 1;
    } else if (g3d) {
        if (pp_i != g3d->instance) {
            retval = PP_FALSE;
            goto done;
        }

        pthread_mutex_lock(&display.lock);
        const PP_Resource previous_device = pp_i->graphics;
        pp_i->graphics = device;
        pthread_mutex_unlock(&display.lock);

        if (device != previous_device) {
            ppb_core_add_ref_resource(device);
            ppb_core_release_resource(previous_device);
        }

        pp_resource_release(device);
        retval = PP_TRUE;
        invalidate_area = 1;
    } else {
        trace_warning("%s, unsupported graphics resource %d on instance %d\n", __func__,
                      device, instance);
        retval = PP_FALSE;
    }

done:

    if (invalidate_area) {
        // successful binding causes plugin graphics area invalidation
        ppb_core_call_on_browser_thread(instance, call_invalidaterect_ptac,
                                        GSIZE_TO_POINTER(instance));
    }

    return retval;
}

PP_Bool
ppb_instance_is_full_frame(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    pthread_mutex_lock(&display.lock);
    int is_fullframe = pp_i->is_fullframe;
    pthread_mutex_unlock(&display.lock);

    if (is_fullframe)
        return PP_TRUE;
    else
        return PP_FALSE;
}

struct get_window_object_param_s {
    PP_Instance     instance;
    struct PP_Var   result;
    PP_Resource     m_loop;
    int             depth;
};

static
void
get_window_object_ptac(void *param)
{
    struct get_window_object_param_s *p = param;

    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        p->result = PP_MakeUndefined();
        goto done;
    }

    npn.retainobject(pp_i->np_window_obj);
    p->result = ppb_var_create_object(p->instance, &n2p_proxy_class, pp_i->np_window_obj);

done:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
get_window_object_comt(void *user_data, int32_t result)
{
    struct get_window_object_param_s *p = user_data;
    ppb_core_call_on_browser_thread(0, get_window_object_ptac, p);
}

struct PP_Var
ppb_instance_get_window_object(PP_Instance instance)
{
    struct get_window_object_param_s *p = g_slice_alloc(sizeof(*p));
    p->instance =   instance;
    p->m_loop =     ppb_message_loop_get_current();
    p->depth =      ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(get_window_object_comt, p), 0,
                                           PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);
    return result;
}

struct PP_Var
ppb_instance_get_owner_element_object(PP_Instance instance)
{
    return PP_MakeUndefined();
}

struct execute_script_param_s {
    struct PP_Var           script;
    struct PP_Var           result;
    PP_Instance             instance_id;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
execute_script_ptac(void *user_data)
{
    struct execute_script_param_s *p = user_data;
    NPString  np_script;
    NPVariant np_result;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance_id);

    if (!pp_i) {
        p->result = PP_MakeUndefined();
        goto quit;
    }

    // no need to lock, this function is run only in browser thread
    if (!pp_i->npp) {
        trace_error("%s, plugin instance was destroyed\n", __func__);
        p->result = PP_MakeUndefined();
        goto quit;
    }

    np_script.UTF8Characters = ppb_var_var_to_utf8(p->script, &np_script.UTF8Length);
    if (!npn.evaluate(pp_i->npp, pp_i->np_window_obj, &np_script, &np_result)) {
        trace_error("%s, NPN_Evaluate failed\n", __func__);
        p->result = PP_MakeUndefined();
        goto quit;
    }

    // TODO: find out what exception is
    p->result = np_variant_to_pp_var(np_result);
    if (np_result.type == NPVariantType_Object)
        tables_add_npobj_npp_mapping(np_result.value.objectValue, pp_i->npp);
    else
        npn.releasevariantvalue(&np_result);

quit:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
execute_script_comt(void *user_data, int32_t result)
{
    struct execute_script_param_s *esp = user_data;
    ppb_core_call_on_browser_thread(0, execute_script_ptac, esp);
}

struct PP_Var
ppb_instance_execute_script(PP_Instance instance, struct PP_Var script, struct PP_Var *exception)
{
    if (script.type != PP_VARTYPE_STRING) {
        trace_error("%s, 'script' is not a string\n", __func__);
        // TODO: fill exception
        return PP_MakeUndefined();
    }

    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_MakeUndefined();
    }

    struct execute_script_param_s *p = g_slice_alloc(sizeof(*p));
    p->script =         script;
    p->instance_id =    instance;
    p->m_loop =         ppb_message_loop_get_current();
    p->depth =          ppb_message_loop_get_depth(p->m_loop) + 1;

    ppb_var_add_ref(script);
    ppb_message_loop_post_work_with_result(p->m_loop, PP_MakeCCB(execute_script_comt, p), 0, PP_OK,
                                           p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);
    ppb_var_release(script);

    struct PP_Var result = p->result;
    g_slice_free1(sizeof(*p), p);

    return result;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    trace_info("[PPB] {full} %s instance=%d, device=%d\n", __func__+6, instance, device);
    return ppb_instance_bind_graphics(instance, device);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_instance_is_full_frame(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_is_full_frame(instance);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_instance_get_window_object(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_get_window_object(instance);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_instance_get_owner_element_object(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_get_owner_element_object(instance);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_instance_execute_script(PP_Instance instance, struct PP_Var script,
                                  struct PP_Var *exception)
{
    char *s_script = trace_var_as_string(script);
    trace_info("[PPB] {full} %s instance=%d, script=%s\n", __func__+6, instance, s_script);
    g_free(s_script);
    return ppb_instance_execute_script(instance, script, exception);
}


const struct PPB_Instance_1_0 ppb_instance_interface_1_0 = {
    .BindGraphics = TWRAPF(ppb_instance_bind_graphics),
    .IsFullFrame =  TWRAPF(ppb_instance_is_full_frame),
};

const struct PPB_Instance_Private_0_1 ppb_instance_private_interface_0_1 = {
    .GetWindowObject =          TWRAPF(ppb_instance_get_window_object),
    .GetOwnerElementObject =    TWRAPZ(ppb_instance_get_owner_element_object),
    .ExecuteScript =            TWRAPF(ppb_instance_execute_script),
};

static
void
__attribute__((constructor))
constructor_ppb_instance(void)
{
    register_interface(PPB_INSTANCE_INTERFACE_1_0, &ppb_instance_interface_1_0);
    register_interface(PPB_INSTANCE_PRIVATE_INTERFACE_0_1, &ppb_instance_private_interface_0_1);
}
