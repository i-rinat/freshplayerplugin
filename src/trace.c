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

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdlib.h>
#include "trace.h"
#include "config.h"
#include "pp_resource.h"
#include "ppb_var.h"
#include <inttypes.h>
#include <ppapi/c/pp_graphics_3d.h>
#include <glib.h>


static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static __thread struct timespec tictoc_ts;


void
trace_info(const char *fmt, ...)
{
    if (config.quiet)
        return;
    pthread_mutex_lock(&lock);
    va_list args;
//    fprintf(stdout, "[fresh] ");
    fprintf(stdout, "[fresh %5d] ", (int)syscall(__NR_gettid));
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    pthread_mutex_unlock(&lock);
}

void
trace_warning(const char *fmt, ...)
{
    pthread_mutex_lock(&lock);
    va_list args;
    fprintf(stdout, "[fresh] [warning] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    pthread_mutex_unlock(&lock);
}

void
trace_error(const char *fmt, ...)
{
    pthread_mutex_lock(&lock);
    va_list args;
    fprintf(stderr, "[fresh] [error] ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stdout, "[fresh] [error] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    pthread_mutex_unlock(&lock);
}

static
char *
dictionary_var_to_string(struct PP_Var dict)
{
    if (dict.type != PP_VARTYPE_DICTIONARY)
        return g_strdup("{}");

    GString *s = g_string_new("{");

    int first = 1;
    struct PP_Var keys = ppb_var_dictionary_get_keys(dict);
    const uint32_t count = ppb_var_array_get_length(keys);
    for (uint32_t k = 0; k < count; k ++) {
        struct PP_Var key = ppb_var_array_get(keys, k);
        struct PP_Var val = ppb_var_dictionary_get(dict, key);

        if (!first)
            g_string_append(s, ",");
        first = 0;

        char *s_key = trace_var_as_string(key);
        char *s_val = trace_var_as_string(val);
        g_string_append_printf(s, "%s:%s", s_key, s_val);
        g_free(s_key);
        g_free(s_val);

        ppb_var_release(key);
        ppb_var_release(val);
    }

    g_string_append(s, "}");
    return g_string_free(s, FALSE);
}

char *
trace_var_as_string(struct PP_Var var)
{
    char       *res = NULL;
    const char *tmp;
    char       *s_dict;
    int         ref_count;

    switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
        res = g_strdup_printf("{UNDEFINED}");
        break;
    case PP_VARTYPE_NULL:
        res = g_strdup_printf("{NULL}");
        break;
    case PP_VARTYPE_BOOL:
        res = g_strdup_printf("{BOOL:%s}", var.value.as_int ? "TRUE" : "FALSE");
        break;
    case PP_VARTYPE_INT32:
        res = g_strdup_printf("{INT32:%d}", var.value.as_int);
        break;
    case PP_VARTYPE_DOUBLE:
        res = g_strdup_printf("{DOUBLE:%f}", var.value.as_double);
        break;
    case PP_VARTYPE_STRING:
        tmp = ppb_var_var_to_utf8(var, NULL);
        ref_count = ppb_var_get_ref_count(var);
        res = g_strdup_printf("{STRING:%d:%p:%s}", ref_count, tmp, tmp);
        break;
    case PP_VARTYPE_OBJECT:
        res = ppb_var_trace_object_var(var);
        break;
    case PP_VARTYPE_ARRAY:
        ref_count = ppb_var_get_ref_count(var);
        res = g_strdup_printf("{ARRAY:%d:%"PRId64"}", ref_count, var.value.as_id);
        break;
    case PP_VARTYPE_DICTIONARY:
        s_dict = dictionary_var_to_string(var);
        ref_count = ppb_var_get_ref_count(var);
        res = g_strdup_printf("{DICTIONARY:%d:%"PRId64":%s}", ref_count, var.value.as_id, s_dict);
        g_free(s_dict);
        break;
    case PP_VARTYPE_ARRAY_BUFFER:
        ref_count = ppb_var_get_ref_count(var);
        res = g_strdup_printf("{ARRAY_BUFFER:%d}", ref_count);
        break;
    case PP_VARTYPE_RESOURCE:
        ref_count = ppb_var_get_ref_count(var);
        res = g_strdup_printf("{RESOURCE:%d}", ref_count);
        break;
    default:
        res = g_strdup_printf("{NOTIMPLEMENTED:%d}", var.type);
        break;
    }

    return res;
}

char *
trace_rect_as_string(const struct PP_Rect *rect)
{
    char *res = NULL;
    if (rect) {
        res = g_strdup_printf("{.x=%d, .y=%d, .w=%d, .h=%d}", rect->point.x, rect->point.y,
                 rect->size.width, rect->size.height);
    } else {
        res = g_strdup_printf("(nil)");
    }

    return res;
}

char *
trace_size_as_string(const struct PP_Size *size)
{
    char *res = NULL;
    if (size) {
        res = g_strdup_printf("{.w=%d, .h=%d}", size->width, size->height);
    } else {
        res = g_strdup_printf("(nil)");
    }

    return res;
}

char *
trace_point_as_string(const struct PP_Point *point)
{
    char *res = NULL;
    if (point)
        res = g_strdup_printf("{.x=%d, .y=%d}", point->x, point->y);
    else
        res = g_strdup_printf("(nil)");

    return res;
}

char *
trace_float_point_as_string(const struct PP_FloatPoint *point)
{
    char *res = NULL;
    if (point)
        res = g_strdup_printf("{.x=%f, .y=%f}", point->x, point->y);
    else
        res = g_strdup_printf("(nil)");

    return res;
}

char *
trace_touch_point_as_string(const struct PP_TouchPoint *point)
{
    char *res = NULL;
    if (point) {
        char *s_position = trace_float_point_as_string(&point->position);
        char *s_radius = trace_float_point_as_string(&point->radius);
        res = g_strdup_printf("{.id=%u, .position=%s, .radius=%s, .rotation_angle=%f, .presure=%f}",
                              point->id, s_position, s_radius, point->rotation_angle,
                              point->pressure);
        g_free(s_position);
        g_free(s_radius);
    } else {
        res = g_strdup_printf("(nil)");
    }

    return res;
}

char *
trace_np_window_as_string(const NPWindow *window)
{
    char *res = NULL;
    if (window) {
        res = g_strdup_printf("{.window=%p, .x=%u, .y=%u, .width=%u, .height=%u, "
                              ".clipRect={.top=%u, .left=%u, .bottom=%u, .right=%u}, .ws_info=%p, "
                              ".type=%d}", window->window, window->x, window->y, window->width,
                              window->height, window->clipRect.top, window->clipRect.left,
                              window->clipRect.bottom, window->clipRect.right, window->ws_info,
                              window->type);
    } else {
        res = g_strdup_printf("(nil)");
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

char *
trace_graphics3d_attributes_as_string(const int32_t attrib_list[])
{
    GString *s = g_string_new("{");
    int k = 0;
    int done = 0;

    while (!done) {
        switch (attrib_list[k]) {
        case PP_GRAPHICS3DATTRIB_ALPHA_SIZE:
            g_string_append_printf(s, "ALPHA=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_BLUE_SIZE:
            g_string_append_printf(s, "BLUE=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GREEN_SIZE:
            g_string_append_printf(s, "GREEN=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_RED_SIZE:
            g_string_append_printf(s, "RED=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_DEPTH_SIZE:
            g_string_append_printf(s, "DEPTH=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_STENCIL_SIZE:
            g_string_append_printf(s, "STENCIL=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLES:
            g_string_append_printf(s, "SAMPLES=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS:
            g_string_append_printf(s, "SAMPLE_BUFFERS=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_NONE:
            done = 1;
            break;
        case PP_GRAPHICS3DATTRIB_HEIGHT:
            g_string_append_printf(s, "HEIGHT=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_WIDTH:
            g_string_append_printf(s, "WIDTH=%d, ", attrib_list[k + 1]);
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SWAP_BEHAVIOR:
            switch (attrib_list[k + 1]) {
            case PP_GRAPHICS3DATTRIB_BUFFER_PRESERVED:
                g_string_append(s, "SWAP_BEHAVIOR=BUFFER_PRESERVED, ");
                break;
            case PP_GRAPHICS3DATTRIB_BUFFER_DESTROYED:
                g_string_append(s, "SWAP_BEHAVIOR=BUFFER_DESTROYED, ");
                break;
            default:
                g_string_append_printf(s, "SWAP_BEHAVIOR=unknown(0x%04x), ", attrib_list[k + 1]);
                break;
            }
            k += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GPU_PREFERENCE:
            switch (attrib_list[k + 1]) {
            case PP_GRAPHICS3DATTRIB_GPU_PREFERENCE_LOW_POWER:
                g_string_append(s, "GPU_PREFERENCE=LOW_POWER, ");
                break;
            case PP_GRAPHICS3DATTRIB_GPU_PREFERENCE_PERFORMANCE:
                g_string_append(s, "GPU_PREFERENCE=PERFORMANCE, ");
                break;
            default:
                g_string_append_printf(s, "GPU_PREFERENCE=unknown(0x%04x), ", attrib_list[k + 1]);
                break;
            }
            k += 2;
            break;
        default:
            g_string_append_printf(s, "unknown(0x%04x), ", attrib_list[k]);
            k += 1;
            break;
        }
    }

    // delete trailing
    if (s->len >= 3)
        g_string_truncate(s, s->len - 2);

    g_string_append(s, "}");

    // return combined string
    return g_string_free(s, FALSE);
}

void
trace_duration_tic(void)
{
    clock_gettime(CLOCK_REALTIME, &tictoc_ts);
}

double
trace_duration_toc(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (ts.tv_sec - tictoc_ts.tv_sec) + 1e-9 * (ts.tv_nsec - tictoc_ts.tv_nsec);
}
