#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <stddef.h>


const PPB_TextInput_Dev ppb_text_input_dev = {
    .SetTextInputType = NULL,
    .UpdateCaretPosition = NULL,
    .CancelCompositionText = NULL,
    .UpdateSurroundingText = NULL,
    .SelectionChanged = NULL
};
