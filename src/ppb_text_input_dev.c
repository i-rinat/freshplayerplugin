#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <stdlib.h>
#include "trace.h"
#include "reverse_constant.h"


static
void
ppb_text_input_dev_interface_set_text_input_type(PP_Instance instance, PP_TextInput_Type_Dev type)
{
    trace_info("[PPB] {zilch} %s instance=%d, type=%s\n", __func__, instance,
               reverse_pp_text_input_type(type));
}

static
void
ppb_text_input_dev_interface_update_caret_position(PP_Instance instance,
                                                   const struct PP_Rect *caret,
                                                   const struct PP_Rect *bounding_box)
{
    char *caret_str = trace_rect_as_string(caret);
    char *bounding_box_str = trace_rect_as_string(bounding_box);
    trace_info("[PPB] {zilch} %s instance=%d, caret=%s, bounding_box=%s\n", __func__, instance,
               caret_str, bounding_box_str);
    free(caret_str);
    free(bounding_box_str);
}

static
void
ppb_text_input_dev_interface_cancel_composition_text(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
}

static
void
ppb_text_input_dev_interface_update_surrounding_text(PP_Instance instance, const char *text,
                                                     uint32_t caret, uint32_t anchor)
{
    trace_info("[PPB] {zilch} %s instance=%d, text=%s, caret=%u, anchor=%u\n", __func__, instance,
               text, caret, anchor);
}

static
void
ppb_text_input_dev_interface_selection_changed(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
}


const struct PPB_TextInput_Dev_0_2 ppb_text_input_dev_interface_0_2 = {
    .SetTextInputType =         ppb_text_input_dev_interface_set_text_input_type,
    .UpdateCaretPosition =      ppb_text_input_dev_interface_update_caret_position,
    .CancelCompositionText =    ppb_text_input_dev_interface_cancel_composition_text,
    .UpdateSurroundingText =    ppb_text_input_dev_interface_update_surrounding_text,
    .SelectionChanged =         ppb_text_input_dev_interface_selection_changed,
};
