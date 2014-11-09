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

#include "ppb_file_chooser.h"
#include <stdlib.h>
#include "pp_resource.h"
#include "tables.h"
#include "trace.h"
#include "ppb_var.h"


PP_Resource
ppb_file_chooser_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                        struct PP_Var accept_types)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource file_chooser = pp_resource_allocate(PP_RESOURCE_FILE_CHOOSER, pp_i);
    struct pp_file_chooser_s *fc = pp_resource_acquire(file_chooser, PP_RESOURCE_FILE_CHOOSER);
    if (!fc) {
        trace_error("%s, failed to create file chooser resource\n", __func__);
        return 0;
    }

    fc->mode = mode;
    fc->accept_types = accept_types;
    ppb_var_add_ref(accept_types);

    pp_resource_release(file_chooser);
    return file_chooser;
}

void
ppb_file_chooser_destroy(void *p)
{
    struct pp_file_chooser_s *fc = p;

    ppb_var_release(fc->accept_types);
}

PP_Bool
ppb_file_chooser_is_file_chooser(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_FILE_CHOOSER;
}

int32_t
ppb_file_chooser_show(PP_Resource chooser, struct PP_ArrayOutput output,
                      struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_file_chooser_show_without_user_gesture(PP_Resource chooser, PP_Bool save_as,
                                           struct PP_Var suggested_file_name,
                                           struct PP_ArrayOutput output,
                                           struct PP_CompletionCallback callback)
{
    return 0;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_file_chooser_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                              struct PP_Var accept_types)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_file_chooser_create(instance, mode, accept_types);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_file_chooser_is_file_chooser(PP_Resource resource)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_file_chooser_is_file_chooser(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_chooser_show(PP_Resource chooser, struct PP_ArrayOutput output,
                            struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_chooser_show(chooser, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_chooser_show_without_user_gesture(PP_Resource chooser, PP_Bool save_as,
                                                 struct PP_Var suggested_file_name,
                                                 struct PP_ArrayOutput output,
                                                 struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_chooser_show_without_user_gesture(chooser, save_as, suggested_file_name, output,
                                                      callback);
}


const struct PPB_FileChooser_Dev_0_6 ppb_file_chooser_dev_interface_0_6 = {
    .Create =           TWRAPF(ppb_file_chooser_create),
    .IsFileChooser =    TWRAPF(ppb_file_chooser_is_file_chooser),
    .Show =             TWRAPZ(ppb_file_chooser_show),
};

const struct PPB_FileChooserTrusted_0_6 ppb_file_chooser_trusted_interface_0_6 = {
    .ShowWithoutUserGesture = TWRAPZ(ppb_file_chooser_show_without_user_gesture),
};
