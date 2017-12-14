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

#include "compat.h"

#ifdef VER
#error macro name collision
#endif
#define VER(major, minor)       ((major) << 16 | (minor) << 8)


#if (VER(GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) < VER(2, 32))
gpointer
g_async_queue_timeout_pop(GAsyncQueue *queue, guint64 timeout)
{
    GTimeVal end_time;
    g_get_current_time(&end_time);
    g_time_val_add(&end_time, timeout);
    return g_async_queue_timed_pop(queue, &end_time);
}

void
g_array_set_clear_func(GArray *array, GDestroyNotify clear_func)
{
   //  Only for compatibility.
   // TODO: memory leaks?
}
#endif


#if (VER(GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) < VER(2, 28))
static
void
call_destroy_notify(gpointer data, gpointer user_data)
{
    GDestroyNotify func = (GDestroyNotify)user_data;
    func(data);
}

void
g_list_free_full(GList *list, GDestroyNotify free_func)
{
    g_list_foreach(list, call_destroy_notify, free_func);
    g_list_free(list);
}
#endif
