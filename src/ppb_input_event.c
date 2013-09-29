#include <ppapi/c/ppb_input_event.h>
#include <stddef.h>


const PPB_InputEvent ppb_input_event = {
    .RequestInputEvents = NULL,
    .RequestFilteringInputEvents = NULL,
    .ClearInputEventRequest = NULL,
    .IsInputEvent = NULL,
    .GetType = NULL,
    .GetTimeStamp = NULL,
    .GetModifiers = NULL
};
