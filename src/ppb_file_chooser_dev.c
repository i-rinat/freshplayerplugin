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

#include "ppb_file_chooser_dev.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_file_chooser_dev_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                            struct PP_Var accept_types)
{
    return 0;
}

PP_Bool
ppb_file_chooser_dev_is_file_chooser(PP_Resource resource)
{
    return PP_TRUE;
}

int32_t
ppb_file_chooser_dev_show(PP_Resource chooser, struct PP_ArrayOutput output,
                          struct PP_CompletionCallback callback)
{
    return 0;
}

// trace wrappers
static
PP_Resource
trace_ppb_file_chooser_dev_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                                  struct PP_Var accept_types)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_chooser_dev_create(instance, mode, accept_types);
}

static
PP_Bool
trace_ppb_file_chooser_dev_is_file_chooser(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_chooser_dev_is_file_chooser(resource);
}

static
int32_t
trace_ppb_file_chooser_dev_show(PP_Resource chooser, struct PP_ArrayOutput output,
                                struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_file_chooser_dev_show(chooser, output, callback);
}

const struct PPB_FileChooser_Dev_0_6 ppb_file_chooser_dev_interface_0_6 = {
    .Create =           trace_ppb_file_chooser_dev_create,
    .IsFileChooser =    trace_ppb_file_chooser_dev_is_file_chooser,
    .Show =             trace_ppb_file_chooser_dev_show,
};
