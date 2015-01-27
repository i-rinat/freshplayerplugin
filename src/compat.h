/*
 * * Copyright © 2013-2014 Rinat Ibragimov
 * *
 * * This file is part of FreshPlayerPlugin.
 * *
 * * Permission is hereby granted, free of charge, to any person obtaining a copy
 * * of this software and associated documentation files (the "Software"), to deal
 * * in the Software without restriction, including without limitation the rights
 * * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * * copies of the Software, and to permit persons to whom the Software is
 * * furnished to do so, subject to the following conditions:
 * *
 * * The above copyright notice and this permission notice shall be included in all
 * * copies or substantial portions of the Software.
 * *
 * * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * * SOFTWARE.
 * */
#ifndef FPP_COMPAT_H
#define FPP_COMPAT_H
#include <glib.h>
#include <pthread.h>

#ifndef G_ENCODE_VERSION 
#define G_ENCODE_VERSION(major,minor)   ((major) << 16 | (minor) << 8)
#endif

gpointer
g_async_queue_timeout_pop_compat(GAsyncQueue *queue, guint64 timeout);

GThread *
g_thread_new_compat (const gchar *name, GThreadFunc func, gpointer data);
#endif // FPP_COMPAT_H
