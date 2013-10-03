#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <stddef.h>


const struct PPB_TextInput_Dev_0_2 ppb_text_input_dev_interface_0_2 = {
    .SetTextInputType = (void *)36,
    .UpdateCaretPosition = (void *)37,
    .CancelCompositionText = (void *)38,
    .UpdateSurroundingText = (void *)39,
    .SelectionChanged = (void *)40,
};
