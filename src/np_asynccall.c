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

#include "np_asynccall.h"
#include "trace_core.h"
#include <glib.h>
#include <npapi/npapi.h>
#include <stddef.h>

static GMainContext *g_main_context_of_main_thread;

int
np_asynccall_initialize(void)
{
    g_main_context_of_main_thread = g_main_context_default();
    if (!g_main_context_of_main_thread)
        return 1;

    return 0;
}

struct np_asynccall_callback_proxy_params {
    void (*func)(void *);
    void *user_data;
};

static gboolean
np_asynccall_callback_proxy(void *user_data)
{
    struct np_asynccall_callback_proxy_params *p = user_data;

    p->func(p->user_data);
    g_slice_free(struct np_asynccall_callback_proxy_params, p);

    // This is a oneshot operation, source should be removed.
    return FALSE;
}

void
np_asynccall_call(NPP instance, void (*func)(void *), void *user_data)
{
    GSource *source = g_idle_source_new();
    if (!source) {
        trace_error("%s, can't create GSource\n", __func__);
        return;
    }

    struct np_asynccall_callback_proxy_params *p = g_slice_alloc0(sizeof(*p));
    if (!p) {
        g_source_unref(source);
        trace_error("%s, can't allocate memory\n", __func__);
        return;
    }

    p->func = func;
    p->user_data = user_data;

    g_source_set_priority(source, G_PRIORITY_DEFAULT);
    g_source_set_callback(source, np_asynccall_callback_proxy, p, NULL);
    g_source_attach(source, g_main_context_of_main_thread);
    g_source_unref(source);
}
