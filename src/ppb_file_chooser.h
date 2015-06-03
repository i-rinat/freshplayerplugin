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

#ifndef FPP_PPB_FILE_CHOOSER_H
#define FPP_PPB_FILE_CHOOSER_H

#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <ppapi/c/trusted/ppb_file_chooser_trusted.h>

PP_Resource
ppb_file_chooser_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                        struct PP_Var accept_types);

PP_Bool
ppb_file_chooser_is_file_chooser(PP_Resource resource);

int32_t
ppb_file_chooser_show(PP_Resource chooser, struct PP_ArrayOutput output,
                      struct PP_CompletionCallback callback);

int32_t
ppb_file_chooser_show_without_user_gesture(PP_Resource chooser, PP_Bool save_as,
                                           struct PP_Var suggested_file_name,
                                           struct PP_ArrayOutput output,
                                           struct PP_CompletionCallback callback);

#endif // FPP_PPB_FILE_CHOOSER_H
