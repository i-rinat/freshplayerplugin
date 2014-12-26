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

#include "screensaver_control.h"
#include <X11/Xatom.h>
#include <string.h>
#include "trace.h"


static
Window
find_xscreensaver_window(Display *dpy)
{
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
}

static
void
deactivate_xscreensaver(Display *dpy)
{
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
}

void
screensaver_deactivate(Display *dpy, uint32_t types)
{
    if (types & SST_XSCREENSAVER)
        deactivate_xscreensaver(dpy);
}

uint32_t
screensaver_type_detect(Display *dpy)
{
    uint32_t flags = 0;

    if (find_xscreensaver_window(dpy) != 0)
        flags |= SST_XSCREENSAVER;

    // TODO: detect screensavers

    return flags;
}
