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

#ifndef FPP_PPB_CORE_H
#define FPP_PPB_CORE_H

#include <ppapi/c/ppb_core.h>
#include <ppapi/c/pp_instance.h>


void
ppb_core_add_ref_resource(PP_Resource resource);

void
ppb_core_release_resource(PP_Resource resource);

PP_Time
ppb_core_get_time(void);

PP_TimeTicks
ppb_core_get_time_ticks(void);

void
ppb_core_trampoline_to_main_thread(struct PP_CompletionCallback callback, int32_t result,
                                   const char *origin);

void
ppb_core_call_on_main_thread(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                             int32_t result);

void
ppb_core_call_on_main_thread2(int32_t delay_in_milliseconds, struct PP_CompletionCallback callback,
                              int32_t result, const char *origin);

/// schedule task for execution on browser thread
///
/// @param instance is optional, pass 0 if there are no instance id available
void
ppb_core_call_on_browser_thread(PP_Instance instance, void (*func)(void *), void *user_data);

PP_Bool
ppb_core_is_main_thread(void);

#endif // FPP_PPB_CORE_H
