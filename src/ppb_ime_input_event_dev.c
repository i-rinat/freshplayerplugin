#include <ppapi/c/dev/ppb_ime_input_event_dev.h>
#include <stddef.h>


const struct PPB_IMEInputEvent_Dev_0_2 ppb_ime_input_event_dev_interface_0_2 = {
    .Create = (void *)212,
    .IsIMEInputEvent = (void *)213,
    .GetText = (void *)214,
    .GetSegmentNumber = (void *)215,
    .GetSegmentOffset = (void *)216,
    .GetTargetSegment = (void *)217,
    .GetSelection = (void *)218,
};
