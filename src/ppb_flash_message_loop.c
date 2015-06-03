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

#include "ppb_flash_message_loop.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "ppb_message_loop.h"
#include "pp_interface.h"


PP_Resource
ppb_flash_message_loop_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource message_loop = pp_resource_allocate(PP_RESOURCE_FLASH_MESSAGE_LOOP, pp_i);
    return message_loop;
}

static
void
ppb_flash_message_loop_destroy(void *p)
{
    struct pp_flash_message_loop_s *fml = p;
    if (fml->running)
        ppb_flash_message_loop_quit(fml->self_id);
}

PP_Bool
ppb_flash_message_loop_is_flash_message_loop(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_FLASH_MESSAGE_LOOP;
}

int32_t
ppb_flash_message_loop_run(PP_Resource flash_message_loop)
{
    struct pp_flash_message_loop_s *fml =
                        pp_resource_acquire(flash_message_loop, PP_RESOURCE_FLASH_MESSAGE_LOOP);
    if (!fml) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    PP_Resource message_loop = ppb_message_loop_get_current();
    fml->running = 1;
    fml->message_loop = message_loop;
    fml->depth = ppb_message_loop_get_depth(message_loop) + 1;

    pp_resource_ref(flash_message_loop);        // prevent destroy of running loop
    pp_resource_release(flash_message_loop);

    // launching nested loop without depth increase to prevent hang up of previously pushed tasks
    ppb_message_loop_run_int(message_loop, ML_NESTED);

    fml = pp_resource_acquire(flash_message_loop, PP_RESOURCE_FLASH_MESSAGE_LOOP);
    if (fml) {
        fml->running = 0;
        pp_resource_release(flash_message_loop);
    }

    pp_resource_unref(flash_message_loop);
    return PP_OK;
}

void
ppb_flash_message_loop_quit(PP_Resource flash_message_loop)
{
    struct pp_flash_message_loop_s *fml =
                        pp_resource_acquire(flash_message_loop, PP_RESOURCE_FLASH_MESSAGE_LOOP);
    if (!fml) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }
    if (fml->running)
        ppb_message_loop_post_quit_depth(fml->message_loop, PP_FALSE, fml->depth);
    pp_resource_release(flash_message_loop);
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_flash_message_loop_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_flash_message_loop_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_message_loop_is_flash_message_loop(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_flash_message_loop_is_flash_message_loop(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_message_loop_run(PP_Resource flash_message_loop)
{
    trace_info("[PPB] {full} %s flash_message_loop=%d\n", __func__+6, flash_message_loop);
    return ppb_flash_message_loop_run(flash_message_loop);
}

TRACE_WRAPPER
void
trace_ppb_flash_message_loop_quit(PP_Resource flash_message_loop)
{
    trace_info("[PPB] {full} %s flash_message_loop=%d\n", __func__+6, flash_message_loop);
    return ppb_flash_message_loop_quit(flash_message_loop);
}


const struct PPB_Flash_MessageLoop_0_1 ppb_flash_message_loop_interface_0_1 = {
    .Create =               TWRAPF(ppb_flash_message_loop_create),
    .IsFlashMessageLoop =   TWRAPF(ppb_flash_message_loop_is_flash_message_loop),
    .Run =                  TWRAPF(ppb_flash_message_loop_run),
    .Quit =                 TWRAPF(ppb_flash_message_loop_quit),
};

static
void
__attribute__((constructor))
constructor_ppb_flash_message_loop(void)
{
    register_interface(PPB_FLASH_MESSAGELOOP_INTERFACE_0_1, &ppb_flash_message_loop_interface_0_1);
    register_resource(PP_RESOURCE_FLASH_MESSAGE_LOOP, ppb_flash_message_loop_destroy);
}
