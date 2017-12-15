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
#include "trace_core.h"
#include <glib.h>
#include <pthread.h>
#include <string.h>

static GHashTable      *interface_ht = NULL;
static pthread_mutex_t  lock = PTHREAD_MUTEX_INITIALIZER;

const void *(*ppp_get_interface)(const char *interface_name);

const void *
ppb_get_interface(const char *interface_name)
{
    trace_info_f("[PPB] {full} %s interface_name=%s\n", __func__, interface_name);

    pthread_mutex_lock(&lock);
    void *impl = g_hash_table_lookup(interface_ht, interface_name);
    pthread_mutex_unlock(&lock);

    if (impl)
        return impl;

    trace_info("not implemented: %s\n", interface_name);
    return NULL;
}

void
register_interface(const char *name, const void *implementation)
{
    pthread_mutex_lock(&lock);

    if (!interface_ht)
        interface_ht = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);

    g_hash_table_insert(interface_ht, g_strdup(name), (void *)implementation);
    pthread_mutex_unlock(&lock);
}

static
void
__attribute__((destructor))
destructor_pp_interface(void)
{
    g_hash_table_unref(interface_ht);
    interface_ht = NULL;
}
