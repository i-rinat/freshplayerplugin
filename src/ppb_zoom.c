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

#include "ppb_zoom.h"
#include <stdlib.h>
#include "trace.h"
#include "pp_interface.h"


void
ppb_zoom_zoom_changed(PP_Instance instance, double factor)
{
}

void
ppb_zoom_zoom_limits_changed(PP_Instance instance, double minimum_factor, double maximum_factor)
{
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_zoom_zoom_changed(PP_Instance instance, double factor)
{
    trace_info("[PPB] {zilch} %s instance=%d, factor=%f\n", __func__+6, instance, factor);
    return ppb_zoom_zoom_changed(instance, factor);
}

TRACE_WRAPPER
void
trace_ppb_zoom_zoom_limits_changed(PP_Instance instance, double minimum_factor,
                                   double maximum_factor)
{
    trace_info("[PPB] {zilch} %s instance=%d, minimum_factor=%f, maximum_factor=%f\n", __func__+6,
               instance, minimum_factor, maximum_factor);
    return ppb_zoom_zoom_limits_changed(instance, minimum_factor, maximum_factor);
}

const struct PPB_Zoom_Dev_0_2 ppb_zoom_dev_interface_0_2 = {
    .ZoomChanged =          TWRAPZ(ppb_zoom_zoom_changed),
    .ZoomLimitsChanged =    TWRAPZ(ppb_zoom_zoom_limits_changed),
};

static
void
__attribute__((constructor))
constructor_ppb_zoom(void)
{
    register_interface(PPB_ZOOM_DEV_INTERFACE_0_2, &ppb_zoom_dev_interface_0_2);
}
