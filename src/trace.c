#define _GNU_SOURCE     // asprintf
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "trace.h"


void
trace_info(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "[fresh] ");
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
        asprintf(&res, "{STRING:%s}", (char*)(size_t)var.value.as_id);
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
