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

#include "ppb_url_loader_trusted.h"
#include <stddef.h>
#include "trace.h"


void
ppb_url_loader_trusted_grant_universal_access(PP_Resource loader)
{
    // TODO: do something
    return;
}

void
ppb_url_loader_trusted_register_status_callback(PP_Resource loader,
                                                PP_URLLoaderTrusted_StatusCallback cb)
{
    return;
}


#ifndef NDEBUG
// trace wrappers
static
void
trace_ppb_url_loader_trusted_grant_universal_access(PP_Resource loader)
{
    trace_info("[PPB] {fake} %s loader=%d\n", __func__+6, loader);
    ppb_url_loader_trusted_grant_universal_access(loader);
}

static
void
trace_ppb_url_loader_trusted_register_status_callback(PP_Resource loader,
                                                      PP_URLLoaderTrusted_StatusCallback cb)
{
    trace_info("[PPB] {zilch} %s loader=%d, cb=%p\n", __func__+6, loader, cb);
    ppb_url_loader_trusted_register_status_callback(loader, cb);
}
#endif // NDEBUG


const struct PPB_URLLoaderTrusted_0_3 ppb_url_loader_trusted_interface_0_3 = {
    .GrantUniversalAccess =     TWRAP(ppb_url_loader_trusted_grant_universal_access),
    .RegisterStatusCallback =   TWRAP(ppb_url_loader_trusted_register_status_callback),
};
