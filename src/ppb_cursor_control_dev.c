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

#include "ppb_cursor_control_dev.h"
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <pthread.h>
#include "pp_resource.h"
#include "ppb_core.h"
#include "tables.h"
#include "trace.h"


struct comt_param_s {
    PP_Instance         instance;
    pthread_barrier_t  *barrier;
    int                 wait;
    int                 xtype;
};

void
_set_cursor_comt(void *user_data)
{
    Window wnd;
    Cursor cursor;
    struct comt_param_s *params = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(params->instance);


    pthread_mutex_lock(&pp_i->lock);
    cursor = XCreateFontCursor(pp_i->dpy, params->xtype);
    if (pp_i->is_fullscreen) {
        XDefineCursor(pp_i->dpy, pp_i->fs_wnd, cursor);
        XFlush(pp_i->dpy);
    } else {
        if (npn.getvalue(pp_i->npp, NPNVnetscapeWindow, &wnd) == NPERR_NO_ERROR) {
            XDefineCursor(pp_i->dpy, wnd, cursor);
            XFlush(pp_i->dpy);
        }
    }
    pthread_mutex_unlock(&pp_i->lock);

    if (params->wait)
        pthread_barrier_wait(params->barrier);
    g_slice_free(struct comt_param_s, params);
}

PP_Bool
ppb_cursor_control_dev_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                                  PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    int xtype = XC_arrow;
    switch (type) {
    case PP_CURSORTYPE_CUSTOM:
        trace_warning("%s, custom cursors not implemented\n", __func__);
        xtype = XC_arrow;
        break;
    case PP_CURSORTYPE_POINTER:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_CROSS:
        xtype = XC_crosshair;
        break;
    case PP_CURSORTYPE_HAND:
        xtype = XC_hand2;
        break;
    case PP_CURSORTYPE_IBEAM:
        xtype = XC_xterm;
        break;
    case PP_CURSORTYPE_WAIT:
        xtype = XC_watch;
        break;
    case PP_CURSORTYPE_HELP:
        xtype = XC_question_arrow;
        break;
    case PP_CURSORTYPE_EASTRESIZE:
        xtype = XC_right_side;
        break;
    case PP_CURSORTYPE_NORTHRESIZE:
        xtype = XC_top_side;
        break;
    case PP_CURSORTYPE_NORTHEASTRESIZE:
        xtype = XC_top_right_corner;
        break;
    case PP_CURSORTYPE_NORTHWESTRESIZE:
        xtype = XC_top_left_corner;
        break;
    case PP_CURSORTYPE_SOUTHRESIZE:
        xtype = XC_bottom_side;
        break;
    case PP_CURSORTYPE_SOUTHEASTRESIZE:
        xtype = XC_bottom_right_corner;
        break;
    case PP_CURSORTYPE_SOUTHWESTRESIZE:
        xtype = XC_bottom_left_corner;
        break;
    case PP_CURSORTYPE_WESTRESIZE:
        xtype = XC_left_side;
        break;
    case PP_CURSORTYPE_NORTHSOUTHRESIZE:
        xtype = XC_sb_v_double_arrow;
        break;
    case PP_CURSORTYPE_EASTWESTRESIZE:
        xtype = XC_sb_h_double_arrow;
        break;
    case PP_CURSORTYPE_NORTHEASTSOUTHWESTRESIZE:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_NORTHWESTSOUTHEASTRESIZE:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_COLUMNRESIZE:
        xtype = XC_sb_h_double_arrow;
        break;
    case PP_CURSORTYPE_ROWRESIZE:
        xtype = XC_sb_v_double_arrow;
        break;
    case PP_CURSORTYPE_MIDDLEPANNING:
        xtype = XC_fleur;
        break;
    case PP_CURSORTYPE_EASTPANNING:
        xtype = XC_sb_right_arrow;
        break;
    case PP_CURSORTYPE_NORTHPANNING:
        xtype = XC_sb_up_arrow;
        break;
    case PP_CURSORTYPE_NORTHEASTPANNING:
        xtype = XC_top_right_corner;
        break;
    case PP_CURSORTYPE_NORTHWESTPANNING:
        xtype = XC_top_left_corner;
        break;
    case PP_CURSORTYPE_SOUTHPANNING:
        xtype = XC_sb_down_arrow;
        break;
    case PP_CURSORTYPE_SOUTHEASTPANNING:
        xtype = XC_bottom_right_corner;
        break;
    case PP_CURSORTYPE_SOUTHWESTPANNING:
        xtype = XC_bottom_left_corner;
        break;
    case PP_CURSORTYPE_WESTPANNING:
        xtype = XC_sb_left_arrow;
        break;
    case PP_CURSORTYPE_MOVE:
        xtype = XC_fleur;
        break;
    case PP_CURSORTYPE_VERTICALTEXT:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_CELL:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_CONTEXTMENU:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_ALIAS:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_PROGRESS:
        xtype = XC_watch;
        break;
    case PP_CURSORTYPE_NODROP:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_COPY:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_NONE:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_NOTALLOWED:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_ZOOMIN:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_ZOOMOUT:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_GRAB:
        xtype = XC_left_ptr;
        break;
    case PP_CURSORTYPE_GRABBING:
        xtype = XC_left_ptr;
        break;
    }

    struct comt_param_s *comt_params = g_slice_alloc(sizeof(*comt_params));

    comt_params->instance = instance;
    comt_params->xtype =    xtype;

    if (ppb_core_is_main_thread()) {
        comt_params->wait = 0;
        _set_cursor_comt(comt_params);
    } else {
        struct pp_instance_s *pp_i;
        pthread_barrier_t barrier;

        pthread_barrier_init(&barrier, NULL, 2);
        comt_params->wait = 1;
        comt_params->barrier = &barrier;

        pp_i = tables_get_pp_instance(instance);
        npn.pluginthreadasynccall(pp_i->npp, _set_cursor_comt, comt_params);
        pthread_barrier_wait(&barrier);
        pthread_barrier_destroy(&barrier);
    }

    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_unlock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_has_cursor_lock(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_dev_can_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}


#ifndef NDEBUG
// trace wrappers
static
PP_Bool
trace_ppb_cursor_control_dev_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                                        PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    char *s_hot_spot = trace_point_as_string(hot_spot);
    trace_info("[PPB] {full} %s instance=%d, type=%d, custom_image=%d, hot_spot=%s\n", __func__+6,
               instance, type, custom_image, s_hot_spot);
    g_free(s_hot_spot);
    return ppb_cursor_control_dev_set_cursor(instance, type, custom_image, hot_spot);
}

static
PP_Bool
trace_ppb_cursor_control_dev_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_lock_cursor(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_unlock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_unlock_cursor(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_has_cursor_lock(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_has_cursor_lock(instance);
}

static
PP_Bool
trace_ppb_cursor_control_dev_can_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_dev_can_lock_cursor(instance);
}
#endif // NDEBUG


const struct PPB_CursorControl_Dev_0_4 ppb_cursor_control_dev_interface_0_4 = {
    .SetCursor =        TWRAP(ppb_cursor_control_dev_set_cursor),
    .LockCursor =       TWRAP(ppb_cursor_control_dev_lock_cursor),
    .UnlockCursor =     TWRAP(ppb_cursor_control_dev_unlock_cursor),
    .HasCursorLock =    TWRAP(ppb_cursor_control_dev_has_cursor_lock),
    .CanLockCursor =    TWRAP(ppb_cursor_control_dev_can_lock_cursor),
};
