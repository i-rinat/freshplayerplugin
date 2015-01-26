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

#include <assert.h>
#include "screensaver_control.h"
#include <X11/Xatom.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>
#include "trace.h"


#define GS_SERVICE      "org.gnome.ScreenSaver"
#define GS_PATH         "/org/gnome/ScreenSaver"
#define GS_INTERFACE    "org.gnome.ScreenSaver"

#define FDOS_SERVICE    "org.freedesktop.ScreenSaver"
#define FDOS_PATH       "/org/freedesktop/ScreenSaver"
#define FDOS_INTERFACE  "org.freedesktop.ScreenSaver"

#define KS_SERVICE      "org.kde.screensaver"
#define KS_PATH         "/ScreenSaver"
#define KS_INTERFACE    "org.kde.screensaver"

#ifdef USE_GIO
static GDBusConnection *connection = NULL;
#endif

static
Window
find_xscreensaver_window(Display *dpy)
{
#ifdef USE_GIO
    Window          root = DefaultRootWindow(dpy);
    Window          root2, parent_wnd, wnd;
    Window         *children;
    unsigned int    nchildren;
    Status          status;

    status = XQueryTree(dpy, root, &root2, &parent_wnd, &children, &nchildren);
    if (status == 0) {
        // failure
        return 0;
    }

    Atom XA_SCREENSAVER_VERSION = XInternAtom (dpy, "_SCREENSAVER_VERSION", False);

    wnd = None;
    for (unsigned int k = 0; k < nchildren; k ++) {
        Atom            type;
        int             format;
        unsigned long   nitems;
        unsigned long   bytes_after;
        unsigned char  *prop;

        status = XGetWindowProperty(dpy, children[k], XA_SCREENSAVER_VERSION, 0, 200, False,
                                    XA_STRING, &type, &format, &nitems, &bytes_after, &prop);
        // relying on error handler already set
        if (status == Success && type != None) {
            wnd = children[k];
            goto done;
        }
    }

done:
    if (children)
        XFree(children);
    return wnd;
#else
    return 0;
#endif

}

static
void
deactivate_xscreensaver(Display *dpy)
{
#ifdef USE_GIO
    Window xssw = find_xscreensaver_window(dpy);

    if (!xssw) {
        trace_warning("%s, no XScreenSaver's window found\n", __func__);
        return;
    }

    XEvent  ev;
    Status  status;
    Atom    XA_SCREENSAVER =    XInternAtom(dpy, "SCREENSAVER", False);
    Atom    XA_DEACTIVATE =     XInternAtom(dpy, "DEACTIVATE", False);

    ev.xany.type =              ClientMessage;
    ev.xclient.display =        dpy;
    ev.xclient.window =         xssw;
    ev.xclient.message_type =   XA_SCREENSAVER;
    ev.xclient.format =         32;

    memset(&ev.xclient.data, 0, sizeof(ev.xclient.data));
    ev.xclient.data.l[0] =      XA_DEACTIVATE;
    ev.xclient.data.l[1] =      0;
    ev.xclient.data.l[2] =      0;

    status = XSendEvent(dpy, xssw, False, 0, &ev);
    if (status == 0) {
        trace_warning("%s, can't send event to XScreenSaver's window\n", __func__);
        return;
    }
#endif
}

static
void
deactivate_dbus_based_screensaver(const char *d_service, const char *d_path,
                                  const char *d_interface)
{
#ifdef USE_GIO
    if (!connection)
        screensaver_connect();

    if (!connection)
        return;

    GDBusMessage *msg = g_dbus_message_new_method_call(d_service, d_path, d_interface,
                                                       "SimulateUserActivity");
    if (!msg)
        return;

    GError *error = NULL;
    g_dbus_connection_send_message(connection, msg, G_DBUS_SEND_MESSAGE_FLAGS_NONE, NULL, &error);

    if (error != NULL) {
        trace_error("%s, can't send message, %s\n", __func__, error->message);
        g_clear_error(&error);
        goto err;
    }

    g_dbus_connection_flush_sync(connection, NULL, &error);
    if (error != NULL) {
        trace_error("%s, can't flush dbus connection, %s\n", __func__, error->message);
        g_clear_error(&error);
        goto err;
    }

err:
    g_object_unref(msg);
#endif
}

void
screensaver_deactivate(Display *dpy, uint32_t types)
{
#ifdef USE_GIO
    if (types & SST_XSCREENSAVER)
        deactivate_xscreensaver(dpy);

    if (types & SST_FDO_SCREENSAVER)
        deactivate_dbus_based_screensaver(FDOS_SERVICE, FDOS_PATH, FDOS_INTERFACE);

    if (types & SST_GNOME_SCREENSAVER)
        deactivate_dbus_based_screensaver(GS_SERVICE, GS_PATH, GS_INTERFACE);

    if (types & SST_KDE_SCREENSAVER)
        deactivate_dbus_based_screensaver(KS_SERVICE, KS_PATH, KS_INTERFACE);
#endif
}

static
uint32_t
detect_dbus_based_screensavers(void)
{
    uint32_t ret = 0;
#ifdef USE_GIO
    GDBusMessage *msg, *reply;

    assert(connection);

    // enumerate all services
    msg = g_dbus_message_new_method_call("org.freedesktop.DBus", "/org/freedesktop/DBus",
                                         "org.freedesktop.DBus", "ListNames");
    if (!msg) {
        trace_error("%s, can't allocate GDBusMessage\n", __func__);
        ret = 0;
        goto err_1;
    }

    GError *error = NULL;
    reply = g_dbus_connection_send_message_with_reply_sync(connection, msg,
                                                           G_DBUS_SEND_MESSAGE_FLAGS_NONE, -1,
                                                           NULL, NULL, &error);
    if (error != NULL) {
        trace_error("%s, can't send message, %s\n", __func__, error->message);
        g_clear_error(&error);
        ret = 0;
        goto err_2;
    }

    g_dbus_connection_flush_sync(connection, NULL, &error);
    if (error != NULL) {
        trace_error("%s, can't flush dbus connection, %s\n", __func__, error->message);
        g_clear_error(&error);
        ret = 0;
        goto err_3;
    }

    // iterate over the list
    GVariant *v = g_dbus_message_get_body(reply);
    GVariantIter *iter;
    gchar *str;

    uint32_t flags = 0;
    g_variant_get(v, "(as)", &iter);
    while (g_variant_iter_loop(iter, "s", &str)) {
        if (strcmp(str, GS_SERVICE) == 0)
            flags |= SST_GNOME_SCREENSAVER;

        if (strcmp(str, KS_SERVICE) == 0)
            flags |= SST_KDE_SCREENSAVER;

        if (strcmp(str, FDOS_SERVICE) == 0)
            flags |= SST_FDO_SCREENSAVER;
    }
    g_variant_iter_free(iter);
    ret = flags;

err_3:
    g_object_unref(reply);
err_2:
    g_object_unref(msg);
err_1:
#endif
    return ret;
}

uint32_t
screensaver_type_detect(Display *dpy)
{
    uint32_t flags = 0;
#ifdef USE_GIO
    if (!connection)
        screensaver_connect();

    if (!connection)
        return 0;

    if (find_xscreensaver_window(dpy) != 0)
        flags |= SST_XSCREENSAVER;

    flags |= detect_dbus_based_screensavers();
#endif
    return flags;
}

void
screensaver_connect(void)
{
#ifdef USE_GIO
    if (connection)
        g_object_unref(connection);

    GError *error = NULL;
    connection = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (connection == NULL) {
        trace_error("%s, can't connect to dbus, %s\n", __func__, error->message);
        g_clear_error(&error);
    }
#endif
}

void
screensaver_disconnect(void)
{
#ifdef USE_GIO
    g_object_unref(connection);
    connection = NULL;
#endif
}
