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

#include "ppb_printing.h"
#include "pp_resource.h"
#include "trace.h"
#include "tables.h"
#include "pp_interface.h"


PP_Resource
ppb_printing_create(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource printing = pp_resource_allocate(PP_RESOURCE_PRINTING, pp_i);
    return printing;
}

static
void
ppb_printing_destroy(void *ptr)
{
}

int32_t
ppb_printing_get_default_print_settings(PP_Resource resource,
                                        struct PP_PrintSettings_Dev *print_settings,
                                        struct PP_CompletionCallback callback)
{
    return 0;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_printing_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_printing_create(instance);
}

TRACE_WRAPPER
int32_t
trace_ppb_printing_get_default_print_settings(PP_Resource resource,
                                              struct PP_PrintSettings_Dev *print_settings,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s resource=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, resource, callback.func, callback.user_data, callback.flags);
    return ppb_printing_get_default_print_settings(resource, print_settings, callback);
}


const struct PPB_Printing_Dev_0_7 ppb_printing_dev_interface_0_7 = {
    .Create =                   TWRAPF(ppb_printing_create),
    .GetDefaultPrintSettings =  TWRAPZ(ppb_printing_get_default_print_settings),
};

static
void
__attribute__((constructor))
constructor_ppb_printing(void)
{
    register_interface(PPB_PRINTING_DEV_INTERFACE_0_7, &ppb_printing_dev_interface_0_7);
    register_resource(PP_RESOURCE_PRINTING, ppb_printing_destroy);
}
