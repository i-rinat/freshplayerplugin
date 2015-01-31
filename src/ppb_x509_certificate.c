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

#include "ppb_x509_certificate.h"
#include <stdlib.h>
#include "trace.h"


PP_Resource
ppb_x509_certificate_create(PP_Instance instance)
{
    return 0;
}

PP_Bool
ppb_x509_certificate_is_x509_certificate(PP_Resource resource)
{
    return PP_FALSE;
}

PP_Bool
ppb_x509_certificate_initialize(PP_Resource resource, const char *bytes, uint32_t length)
{
    return PP_FALSE;
}

struct PP_Var
ppb_x509_certificate_get_field(PP_Resource resource, PP_X509Certificate_Private_Field field)
{
    return PP_MakeUndefined();
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_x509_certificate_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_x509_certificate_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_x509_certificate_is_x509_certificate(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__+6, resource);
    return ppb_x509_certificate_is_x509_certificate(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_x509_certificate_initialize(PP_Resource resource, const char *bytes, uint32_t length)
{
    trace_info("[PPB] {zilch} %s resource=%d, bytes=%p, length=%u\n", __func__+6, resource, bytes,
               length);
    return ppb_x509_certificate_initialize(resource, bytes, length);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_x509_certificate_get_field(PP_Resource resource, PP_X509Certificate_Private_Field field)
{
    trace_info("[PPB] {zilch} %s resource=%d, field=%u\n", __func__+6, resource, field);
    return ppb_x509_certificate_get_field(resource, field);
}

const struct PPB_X509Certificate_Private_0_1 ppb_x509_certificate_interface_0_1 = {
    .Create =                   TWRAPZ(ppb_x509_certificate_create),
    .IsX509CertificatePrivate = TWRAPZ(ppb_x509_certificate_is_x509_certificate),
    .Initialize =               TWRAPZ(ppb_x509_certificate_initialize),
    .GetField =                 TWRAPZ(ppb_x509_certificate_get_field),
};
