/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "gtk_wrapper.h"
#include "pp_interface.h"
#include "ppb_core.h"
#include "ppb_instance.h"
#include "ppb_text_input.h"
#include "reverse_constant.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include "utils.h"
#include <glib.h>
#include <ppapi/c/dev/ppp_text_input_dev.h>
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

struct set_text_input_type_param_s {
    PP_Instance             instance;
    PP_TextInput_Type_Dev   type;
};

static
void
set_text_input_type_ptac(void *param)
{
    struct set_text_input_type_param_s *p = param;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    if (pp_i->im_context)
        gw_gtk_im_context_focus_out(pp_i->im_context);

    switch (p->type) {
    case PP_TEXTINPUT_TYPE_DEV_NONE:
    case PP_TEXTINPUT_TYPE_DEV_PASSWORD:
        pp_i->im_context = NULL;
        break;
    case PP_TEXTINPUT_TYPE_DEV_TEXT:
        pp_i->im_context = pp_i->im_context_multi;
        break;
    default:
        pp_i->im_context = pp_i->im_context_simple;
        break;
    }

    pp_i->textinput_type = p->type;
    if (pp_i->im_context)
        gw_gtk_im_context_focus_in(pp_i->im_context);

    g_slice_free1(sizeof(*p), p);
}

void
ppb_text_input_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type)
{
    struct set_text_input_type_param_s *p = g_slice_alloc0(sizeof(*p));
    p->instance =   instance;
    p->type =       type;

    ppb_core_call_on_browser_thread(instance, set_text_input_type_ptac, p);
}

struct update_caret_position_param_s {
    PP_Instance     instance;
    GdkRectangle    caret;
};

static
void
update_caret_position_ptac(void *param)
{
    struct update_caret_position_param_s *p = param;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    p->caret.x += pp_i->offset_x;
    p->caret.y += pp_i->offset_y;

    if (pp_i->im_context)
        gw_gtk_im_context_set_cursor_location(pp_i->im_context, &p->caret);
    g_slice_free1(sizeof(*p), p);
}

void
ppb_text_input_interface_update_caret_position(PP_Instance instance, const struct PP_Rect *caret,
                                               const struct PP_Rect *bounding_box)
{
    if (!caret)
        return;

    struct update_caret_position_param_s *p = g_slice_alloc0(sizeof(*p));
    p->instance =     instance;
    p->caret.x =      caret->point.x;
    p->caret.y =      caret->point.y;
    p->caret.width =  caret->size.width;
    p->caret.height = caret->size.height;

    ppb_core_call_on_browser_thread(instance, update_caret_position_ptac, p);
}

static
void
cancel_composition_text_ptac(void *param)
{
    PP_Instance instance = GPOINTER_TO_SIZE(param);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    if (pp_i->im_context)
        gw_gtk_im_context_reset(pp_i->im_context);
}

void
ppb_text_input_interface_cancel_composition_text(PP_Instance instance)
{
    ppb_core_call_on_browser_thread(instance, cancel_composition_text_ptac,
                                    GSIZE_TO_POINTER(instance));
}

struct update_surrounding_text_param_s {
    PP_Instance     instance;
    char           *text;
    uint32_t        caret;
};

static
void
update_surrounding_text_ptac(void *param)
{
    struct update_surrounding_text_param_s *p = param;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        goto done;
    }

    if (pp_i->im_context) {
        const size_t len = p->text ? strlen(p->text) : 0;
        gw_gtk_im_context_set_surrounding(pp_i->im_context, p->text, len, p->caret);
    }

done:
    g_free(p->text);
    g_slice_free1(sizeof(*p), p);
}

void
ppb_text_input_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                 uint32_t caret, uint32_t anchor)
{
    struct update_surrounding_text_param_s *p = g_slice_alloc0(sizeof(*p));
    p->instance =       instance;
    p->text =           g_strdup(text);
    p->caret =          caret;

    ppb_core_call_on_browser_thread(instance, update_surrounding_text_ptac, p);

    return;
}

static
void
selection_changed_comt(void *user_data, int32_t result)
{
    PP_Instance instance = GPOINTER_TO_SIZE(user_data);
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    if (!pp_i->ppp_text_input_dev)
        pp_i->ppp_text_input_dev = ppp_get_interface(PPP_TEXTINPUT_DEV_INTERFACE_0_1);

    if (pp_i->ppp_text_input_dev)
        pp_i->ppp_text_input_dev->RequestSurroundingText(pp_i->id, 100);
}

void
ppb_text_input_interface_selection_changed(PP_Instance instance)
{
    ppb_core_call_on_main_thread2(0, PP_MakeCCB(selection_changed_comt, GSIZE_TO_POINTER(instance)),
                                  PP_OK, __func__);
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_text_input_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type)
{
    trace_info("[PPB] {full} %s instance=%d, type=%s\n", __func__+6, instance,
               reverse_pp_text_input_type(type));
    ppb_text_input_interface_set_text_input_type(instance, type);
}

TRACE_WRAPPER
void
trace_ppb_text_input_interface_update_caret_position(PP_Instance instance,
                                                     const struct PP_Rect *caret,
                                                     const struct PP_Rect *bounding_box)
{
    char *caret_str = trace_rect_as_string(caret);
    char *bounding_box_str = trace_rect_as_string(bounding_box);
    trace_info("[PPB] {full} %s instance=%d, caret=%s, bounding_box=%s\n", __func__+6, instance,
               caret_str, bounding_box_str);
    g_free(caret_str);
    g_free(bounding_box_str);
    ppb_text_input_interface_update_caret_position(instance, caret, bounding_box);
}

TRACE_WRAPPER
void
trace_ppb_text_input_interface_cancel_composition_text(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_text_input_interface_cancel_composition_text(instance);
}

TRACE_WRAPPER
void
trace_ppb_text_input_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                       uint32_t caret, uint32_t anchor)
{
    trace_info("[PPB] {full} %s instance=%d, text=%s, caret=%u, anchor=%u\n", __func__+6, instance,
               text, caret, anchor);
    ppb_text_input_interface_update_surrounding_text(instance, text, caret, anchor);
}

TRACE_WRAPPER
void
trace_ppb_text_input_interface_selection_changed(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    ppb_text_input_interface_selection_changed(instance);
}


const struct PPB_TextInput_Dev_0_2 ppb_text_input_dev_interface_0_2 = {
    .SetTextInputType =         TWRAPF(ppb_text_input_interface_set_text_input_type),
    .UpdateCaretPosition =      TWRAPF(ppb_text_input_interface_update_caret_position),
    .CancelCompositionText =    TWRAPF(ppb_text_input_interface_cancel_composition_text),
    .UpdateSurroundingText =    TWRAPF(ppb_text_input_interface_update_surrounding_text),
    .SelectionChanged =         TWRAPF(ppb_text_input_interface_selection_changed),
};

static
void
__attribute__((constructor))
constructor_ppb_text_input(void)
{
    register_interface(PPB_TEXTINPUT_DEV_INTERFACE_0_2, &ppb_text_input_dev_interface_0_2);
}
