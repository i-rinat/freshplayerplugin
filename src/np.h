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

#ifndef FPP__NP_H
#define FPP__NP_H

#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/pp_resource.h>
#include <X11/Xlib.h>
#include <npapi.h>

struct np_priv_s {
    const struct PPP_Instance_1_1  *ppp_instance_1_1;
    Window          wnd;
    PP_Instance     pp_instance_id;
    uint32_t        x;
    uint32_t        y;
    uint32_t        width;
    uint32_t        height;
    NPRect          clip_rect;
    void           *ws_info;
    NPWindowType    window_type;
    int             argc;
    const char    **argn;
    const char    **argv;
    int             instance_loaded;
    const char     *instance_url;
    pthread_t       pp_thread;
    PP_Resource     graphics;
};

#endif // FPP_NP_H
