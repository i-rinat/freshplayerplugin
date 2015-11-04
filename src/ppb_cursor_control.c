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

#include "ppb_cursor_control.h"
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/Xcursor/Xcursor.h>
#include <pthread.h>
#include "pp_resource.h"
#include "ppb_core.h"
#include "tables.h"
#include "trace.h"
#include "pp_interface.h"


struct comt_param_s {
    PP_Instance     instance_id;
    int             xtype;
    int             hide_cursor;
    PP_Resource     custom_image;
    int             hotspot_x;
    int             hotspot_y;
};


static
Cursor
create_cursor_from_image_data_resource(Display *dpy, Window wnd, PP_Resource image_data,
                                       int hotspot_x, int hotspot_y)
{
    struct pp_image_data_s *id = pp_resource_acquire(image_data, PP_RESOURCE_IMAGE_DATA);
    if (!id) {
        trace_warning("%s, bad resource\n", __func__);
        return None;
    }

    XcursorImage *cursor_image = XcursorImageCreate(id->width, id->height);
    cursor_image->xhot = hotspot_x;
    cursor_image->yhot = hotspot_y;

    memcpy(cursor_image->pixels, id->data, id->stride * id->height);

    Cursor cursor = XcursorImageLoadCursor(dpy, cursor_image);
    XcursorImageDestroy(cursor_image);

    pp_resource_release(image_data);
    return cursor;
}

void
set_cursor_ptac(void *user_data)
{
    Window wnd = None;
    Cursor cursor;
    struct comt_param_s *params = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(params->instance_id);

    if (!pp_i)
        goto quit;

    if (pp_i->is_fullscreen) {
        wnd = pp_i->fs_wnd;
    } else if (pp_i->windowed_mode) {
        wnd = pp_i->wnd;
    } else {
        if (npn.getvalue(pp_i->npp, NPNVnetscapeWindow, &wnd) != NPERR_NO_ERROR)
            wnd = None;
    }

    pthread_mutex_lock(&display.lock);

    if (params->hide_cursor) {
        cursor = display.transparent_cursor;
    } else {
        if (params->custom_image != 0) {
            cursor = create_cursor_from_image_data_resource(display.x, wnd, params->custom_image,
                                                            params->hotspot_x, params->hotspot_y);
        } else {
            cursor = XCreateFontCursor(display.x, params->xtype);
        }
    }

    if (wnd != None && cursor != None) {
        XDefineCursor(display.x, wnd, cursor);
        XFlush(display.x);

        // remember to free cursor unless we hid it
        pp_i->have_prev_cursor = !params->hide_cursor;
        pp_i->prev_cursor = cursor;
    }

    pthread_mutex_unlock(&display.lock);

quit:
    g_slice_free(struct comt_param_s, params);
}

PP_Bool
ppb_cursor_control_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                              PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return PP_FALSE;
    }

    if (!g_atomic_int_get(&pp_i->cursor_inside_instance)) {
        // avoid changing pointer shape
        return PP_TRUE;
    }

    int xtype = XC_arrow;
    int hide_cursor = 0;
    switch (type) {
    case PP_CURSORTYPE_CUSTOM:
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
        hide_cursor = 1;
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

    struct comt_param_s *comt_params = g_slice_alloc0(sizeof(*comt_params));

    comt_params->instance_id =  instance;
    comt_params->xtype =        xtype;
    comt_params->hide_cursor =  hide_cursor;
    comt_params->custom_image = (type == PP_CURSORTYPE_CUSTOM) ? custom_image : 0;

    if (hot_spot) {
        comt_params->hotspot_x = hot_spot->x;
        comt_params->hotspot_y = hot_spot->y;
    }

    ppb_core_call_on_browser_thread(instance, set_cursor_ptac, comt_params);

    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_unlock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_has_cursor_lock(PP_Instance instance)
{
    return PP_TRUE;
}

PP_Bool
ppb_cursor_control_can_lock_cursor(PP_Instance instance)
{
    return PP_TRUE;
}


// trace wrappers
TRACE_WRAPPER
PP_Bool
trace_ppb_cursor_control_set_cursor(PP_Instance instance, enum PP_CursorType_Dev type,
                                    PP_Resource custom_image, const struct PP_Point *hot_spot)
{
    char *s_hot_spot = trace_point_as_string(hot_spot);
    trace_info("[PPB] {full} %s instance=%d, type=%d, custom_image=%d, hot_spot=%s\n", __func__+6,
               instance, type, custom_image, s_hot_spot);
    g_free(s_hot_spot);
    return ppb_cursor_control_set_cursor(instance, type, custom_image, hot_spot);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_cursor_control_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_lock_cursor(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_cursor_control_unlock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_unlock_cursor(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_cursor_control_has_cursor_lock(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_has_cursor_lock(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_cursor_control_can_lock_cursor(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_cursor_control_can_lock_cursor(instance);
}


const struct PPB_CursorControl_Dev_0_4 ppb_cursor_control_dev_interface_0_4 = {
    .SetCursor =        TWRAPF(ppb_cursor_control_set_cursor),
    .LockCursor =       TWRAPZ(ppb_cursor_control_lock_cursor),
    .UnlockCursor =     TWRAPZ(ppb_cursor_control_unlock_cursor),
    .HasCursorLock =    TWRAPZ(ppb_cursor_control_has_cursor_lock),
    .CanLockCursor =    TWRAPZ(ppb_cursor_control_can_lock_cursor),
};

static
void
__attribute__((constructor))
constructor_ppb_cursor_control(void)
{
    register_interface(PPB_CURSOR_CONTROL_DEV_INTERFACE_0_4, &ppb_cursor_control_dev_interface_0_4);
}
