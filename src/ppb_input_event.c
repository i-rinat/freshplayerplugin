#include <ppapi/c/ppb_input_event.h>
#include <stddef.h>
#include "trace.h"


static
int32_t
ppb_input_event_request_input_events(PP_Instance instance, uint32_t event_classes)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
int32_t
ppb_input_event_request_filtering_input_events(PP_Instance instance, uint32_t event_classes)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
void
ppb_input_event_clear_input_event_request(PP_Instance instance, uint32_t event_classes)
{
    trace_info("{zilch} %s\n", __func__);
}

static
PP_Bool
ppb_input_event_is_input_event(PP_Resource resource)
{
    trace_info("{zilch} %s\n", __func__);
    return PP_TRUE;
}

static
PP_InputEvent_Type
ppb_input_event_get_type(PP_Resource event)
{
    PP_InputEvent_Type t = {0};
    trace_info("{zilch} %s\n", __func__);
    return t;
}

static
PP_TimeTicks
ppb_input_event_get_time_stamp(PP_Resource event)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}

static
uint32_t
ppb_input_event_get_modifiers(PP_Resource event)
{
    trace_info("{zilch} %s\n", __func__);
    return 0;
}
  

const struct PPB_InputEvent_1_0 ppb_input_event_interface_1_0 = {
    .RequestInputEvents = ppb_input_event_request_input_events,
    .RequestFilteringInputEvents = ppb_input_event_request_filtering_input_events,
    .ClearInputEventRequest = ppb_input_event_clear_input_event_request,
    .IsInputEvent = ppb_input_event_is_input_event,
    .GetType = ppb_input_event_get_type,
    .GetTimeStamp = ppb_input_event_get_time_stamp,
    .GetModifiers = ppb_input_event_get_modifiers
};
