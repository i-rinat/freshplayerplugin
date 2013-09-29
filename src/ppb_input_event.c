#include <ppapi/c/ppb_input_event.h>
#include <stddef.h>


const struct PPB_InputEvent_1_0 ppb_input_event_interface_1_0 = {
    .RequestInputEvents = NULL,
    .RequestFilteringInputEvents = NULL,
    .ClearInputEventRequest = NULL,
    .IsInputEvent = NULL,
    .GetType = NULL,
    .GetTimeStamp = NULL,
    .GetModifiers = NULL
};
