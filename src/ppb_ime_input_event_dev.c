#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <stddef.h>


const PPB_IMEInputEvent_Dev ppb_ime_input_event_dev = {
    .Create = NULL,
    .IsIMEInputEvent = NULL,
    .GetText = NULL,
    .GetSegmentNumber = NULL,
    .GetSegmentOffset = NULL,
    .GetTargetSegment = NULL,
    .GetSelection = NULL
};
