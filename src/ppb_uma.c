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

#include "pp_interface.h"
#include "ppb_uma.h"
#include "trace_core.h"

void
ppb_uma_histogram_custom_times(PP_Instance instance, struct PP_Var name, int64_t sample,
                               int64_t min, int64_t max, uint32_t bucket_count)
{
}

void
ppb_uma_histogram_custom_counts(PP_Instance instance, struct PP_Var name, int32_t sample,
                                int32_t min, int32_t max, uint32_t bucket_count)
{
}

void
ppb_uma_histogram_enumeration(PP_Instance instance, struct PP_Var name, int32_t sample,
                              int32_t boundary_value)
{
}

int32_t
ppb_uma_is_crash_reporting_enabled(PP_Instance instance, struct PP_CompletionCallback callback)
{
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_uma_histogram_custom_times(PP_Instance instance, struct PP_Var name, int64_t sample,
                                     int64_t min, int64_t max, uint32_t bucket_count)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_uma_histogram_custom_times(instance, name, sample, min, max, bucket_count);
}

TRACE_WRAPPER
void
trace_ppb_uma_histogram_custom_counts(PP_Instance instance, struct PP_Var name, int32_t sample,
                                      int32_t min, int32_t max, uint32_t bucket_count)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_uma_histogram_custom_counts(instance, name, sample, min, max, bucket_count);
}

TRACE_WRAPPER
void
trace_ppb_uma_histogram_enumeration(PP_Instance instance, struct PP_Var name, int32_t sample,
                                    int32_t boundary_value)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_uma_histogram_enumeration(instance, name, sample, boundary_value);
}

TRACE_WRAPPER
int32_t
trace_ppb_uma_is_crash_reporting_enabled(PP_Instance instance,
                                         struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_uma_is_crash_reporting_enabled(instance, callback);
}

const struct PPB_UMA_Private_0_3 ppb_uma_private_interface_0_3 = {
    .HistogramCustomTimes =     TWRAPZ(ppb_uma_histogram_custom_times),
    .HistogramCustomCounts =    TWRAPZ(ppb_uma_histogram_custom_counts),
    .HistogramEnumeration =     TWRAPZ(ppb_uma_histogram_enumeration),
    .IsCrashReportingEnabled =  TWRAPZ(ppb_uma_is_crash_reporting_enabled),
};

static
void
__attribute__((constructor))
constructor_ppb_uma(void)
{
    register_interface(PPB_UMA_PRIVATE_INTERFACE_0_3, &ppb_uma_private_interface_0_3);
}
