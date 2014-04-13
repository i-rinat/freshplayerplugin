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

#define _GNU_SOURCE     // asprintf
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include "trace.h"
#include "pp_resource.h"


void
trace_info(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "[fresh] ");
//    fprintf(stdout, "[fresh %5d] ", (int)syscall(__NR_gettid));
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void
trace_warning(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "[fresh] [warning] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void
trace_error(const char *fmt, ...)
{
    va_list args;
    fprintf(stderr, "[fresh] [error] ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stdout, "[fresh] [error] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

char *
trace_var_as_string(struct PP_Var var)
{
    char *res = NULL;

    switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
        asprintf(&res, "{UNDEFINED}");
        break;
    case PP_VARTYPE_NULL:
        asprintf(&res, "{NULL}");
        break;
    case PP_VARTYPE_BOOL:
        asprintf(&res, "{BOOLEAN:%s}", var.value.as_int ? "TRUE" : "FALSE");
        break;
    case PP_VARTYPE_INT32:
        asprintf(&res, "{INT32:%d}", var.value.as_int);
        break;
    case PP_VARTYPE_DOUBLE:
        asprintf(&res, "{DOUBLE:%f}", var.value.as_double);
        break;
    case PP_VARTYPE_STRING:
        // TODO: handle inline '\0's
        asprintf(&res, "{STRING:%s}", (char*)(size_t)(var.value.as_id + sizeof(uint32_t)));
        break;
    case PP_VARTYPE_OBJECT:
        {
            struct pp_var_object_s *obj = (void*)(size_t)var.value.as_id;
            asprintf(&res, "{OBJECT:class=%p,data=%p}", obj->klass, obj->data);
        }
        break;
    default:
        asprintf(&res, "{NOTIMPLEMENTED:%d}", var.type);
        break;
    }

    return res;
}

char *
trace_rect_as_string(const struct PP_Rect *rect)
{
    char *res = NULL;
    if (rect) {
        asprintf(&res, "{.x=%d, .y=%d, .w=%d, .h=%d}", rect->point.x, rect->point.y,
                 rect->size.width, rect->size.height);
    } else {
        asprintf(&res, "(nil)");
    }

    return res;
}

char *
trace_size_as_string(const struct PP_Size *size)
{
    char *res = NULL;
    if (size) {
        asprintf(&res, "{.w=%d, .h=%d}", size->width, size->height);
    } else {
        asprintf(&res, "(nil)");
    }

    return res;
}

char *
trace_point_as_string(const struct PP_Point *point)
{
    char *res = NULL;
    if (point)
        asprintf(&res, "{.x=%d, .y=%d}", point->x, point->y);
    else
        asprintf(&res, "(nil)");

    return res;
}

char *
trace_float_point_as_string(const struct PP_FloatPoint *point)
{
    char *res = NULL;
    if (point)
        asprintf(&res, "{.x=%f, .y=%f}", point->x, point->y);
    else
        asprintf(&res, "(nil)");

    return res;
}

char *
trace_touch_point_as_string(const struct PP_TouchPoint *point)
{
    char *res = NULL;
    if (point) {
        char *s_position = trace_float_point_as_string(&point->position);
        char *s_radius = trace_float_point_as_string(&point->radius);
        asprintf(&res, "{.id=%u, .position=%s, .radius=%s, .rotation_angle=%f, .presure=%f}",
                 point->id, s_position, s_radius, point->rotation_angle, point->pressure);
        free(s_position);
        free(s_radius);
    } else {
        asprintf(&res, "(nil)");
    }

    return res;
}

char *
trace_np_window_as_string(const NPWindow *window)
{
    char *res = NULL;
    if (window) {
        asprintf(&res, "{.window=%p, .x=%u, .y=%u, .width=%u, .height=%u, .clipRect={.top=%u, "
                 ".left=%u, .bottom=%u, .right=%u}, .ws_info=%p, .type=%d}", window->window,
                 window->x, window->y, window->width, window->height, window->clipRect.top,
                 window->clipRect.left, window->clipRect.bottom, window->clipRect.right,
                 window->ws_info, window->type);
    } else {
        asprintf(&res, "(nil)");
    }
    return res;
}

char *
trace_event_classes_as_string(uint32_t ec)
{
    int req_len = 0;
    char *res;

    req_len += (ec & PP_INPUTEVENT_CLASS_MOUSE) ?    strlen("MOUSE|") : 0;
    req_len += (ec & PP_INPUTEVENT_CLASS_KEYBOARD) ? strlen("KEYBOARD|") : 0;
    req_len += (ec & PP_INPUTEVENT_CLASS_WHEEL) ?    strlen("WHEEL|") : 0;
    req_len += (ec & PP_INPUTEVENT_CLASS_TOUCH) ?    strlen("TOUCH|") : 0;
    req_len += (ec & PP_INPUTEVENT_CLASS_IME) ?      strlen("IME|") : 0;
    req_len += 1;
    res = malloc(req_len);

    strcpy(res, "");
    if (ec & PP_INPUTEVENT_CLASS_MOUSE)     strcat(res, "MOUSE|");
    if (ec & PP_INPUTEVENT_CLASS_KEYBOARD)  strcat(res, "KEYBOARD|");
    if (ec & PP_INPUTEVENT_CLASS_WHEEL)     strcat(res, "WHEEL|");
    if (ec & PP_INPUTEVENT_CLASS_TOUCH)     strcat(res, "TOUCH|");
    if (ec & PP_INPUTEVENT_CLASS_IME)       strcat(res, "IME|");

    // delete trailing '|'
    if (strlen(res) > 0)
        res[strlen(res) - 1] = 0;

    return res;
}
