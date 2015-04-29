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

#ifndef FPP_COMPAT_H
#define FPP_COMPAT_H

#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <X11/Xlib.h>

#ifdef VER
#error macro name collision
#endif

#define VER(major, minor)       ((major) << 16 | (minor) << 8)

#if (VER(GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) >= VER(2, 26))
#define HAVE_GLIB_DBUS 1
#else
#define HAVE_GLIB_DBUS 0
#endif

#if (VER(GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) < VER(2, 32))
gpointer
g_async_queue_timeout_pop(GAsyncQueue *queue, guint64 timeout);
<<<<<<< HEAD
void
g_array_set_clear_func (GArray *array, GDestroyNotify clear_func);
=======

void
g_array_set_clear_func(GArray *array, GDestroyNotify clear_func);
>>>>>>> upstream/master
#endif

#if (VER(GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) < VER(2, 28))
void
g_list_free_full(GList *list, GDestroyNotify free_func);
#endif

#if (GTK_MAJOR_VERSION * 1000 + GTK_MINOR_VERSION < 2 * 1000 + 24)
GdkWindow *
gdk_x11_window_lookup_for_display(GdkDisplay *gdpy, Window wnd);

GdkWindow *
gdk_x11_window_foreign_new_for_display(GdkDisplay *gdpy, Window wnd);
#endif

#undef VER

#endif // FPP_COMPAT_H
