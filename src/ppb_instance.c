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

#include "ppb_instance.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"

PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_warning("%s, pp_i is NULL\n", __func__);
        return PP_FALSE;
    }

    if (device == 0) {
        // unbind
        pp_i->graphics = 0;
        return PP_TRUE;
    }

    struct pp_graphics2d_s *g2d = pp_resource_acquire(device, PP_RESOURCE_GRAPHICS2D);
    struct pp_graphics3d_s *g3d = pp_resource_acquire(device, PP_RESOURCE_GRAPHICS3D);

    if (g2d) {
        g2d->instance = instance;
        pp_i->graphics = device;
        pp_resource_release(device);
        return PP_TRUE;
    } else if (g3d) {
        pp_i->graphics = device;
        pp_resource_release(device);
        return PP_TRUE;
    } else {
        trace_warning("%s, unsupported graphics resource %d on instance %d\n", __func__,
                      device, instance);
        return PP_FALSE;
    }
}

PP_Bool
ppb_instance_is_full_frame(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);

    if (pp_i->is_fullframe)
        return PP_TRUE;
    else
        return PP_FALSE;
}


#ifndef NDEBUG
// trace wrappers
static
PP_Bool
trace_ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device)
{
    trace_info("[PPB] {full} %s instance=%d, device=%d\n", __func__+6, instance, device);
    return ppb_instance_bind_graphics(instance, device);
}

static
PP_Bool
trace_ppb_instance_is_full_frame(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_instance_is_full_frame(instance);
}
#endif // NDEBUG


const struct PPB_Instance_1_0 ppb_instance_interface_1_0 = {
    .BindGraphics = TWRAP(ppb_instance_bind_graphics),
    .IsFullFrame =  TWRAP(ppb_instance_is_full_frame),
};
