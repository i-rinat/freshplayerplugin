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

#include "pp_interface.h"
#include "ppb_messaging.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include <glib.h>
#include <ppapi/c/pp_errors.h>

void
ppb_messaging_post_message(PP_Instance instance, struct PP_Var message)
{
}

int32_t
ppb_messaging_register_message_handler(PP_Instance instance, void *user_data,
                                       const struct PPP_MessageHandler_0_2 *handler,
                                       PP_Resource message_loop)
{
    return PP_OK;
}

void
ppb_messaging_unregister_message_handler(PP_Instance instance)
{
}


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_messaging_post_message(PP_Instance instance, struct PP_Var message)
{
    gchar *s_message = trace_var_as_string(message);
    trace_info("[PPB] {zilch} %s instance=%d, message=%s\n", __func__+6, instance, s_message);
    g_free(s_message);
    return ppb_messaging_post_message(instance, message);
}

TRACE_WRAPPER
int32_t
trace_ppb_messaging_register_message_handler(PP_Instance instance, void *user_data,
                                             const struct PPP_MessageHandler_0_2 *handler,
                                             PP_Resource message_loop)
{
    trace_info("[PPB] {zilch} %s instance=%d, user_data=%p, handler={.HandleMessage=%p, "
               ".HandleBlockingMessage=%p, .Destroy=%p}, message_loop=%d\n", __func__+6, instance,
               user_data, handler->HandleMessage, handler->HandleBlockingMessage, handler->Destroy,
               message_loop);
    return ppb_messaging_register_message_handler(instance, user_data, handler, message_loop);
}

TRACE_WRAPPER
void
trace_ppb_messaging_unregister_message_handler(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__+6, instance);
    return ppb_messaging_unregister_message_handler(instance);
}


const struct PPB_Messaging_1_2 ppb_messaging_interface_1_2 = {
    .PostMessage =              TWRAPZ(ppb_messaging_post_message),
    .RegisterMessageHandler =   TWRAPZ(ppb_messaging_register_message_handler),
    .UnregisterMessageHandler = TWRAPZ(ppb_messaging_unregister_message_handler),
};

const struct PPB_Messaging_1_0 ppb_messaging_interface_1_0 = {
    .PostMessage =              TWRAPZ(ppb_messaging_post_message),
};

static
void
__attribute__((constructor))
constructor_ppb_messaging(void)
{
    register_interface(PPB_MESSAGING_INTERFACE_1_0, &ppb_messaging_interface_1_0);
    register_interface(PPB_MESSAGING_INTERFACE_1_2, &ppb_messaging_interface_1_2);
}
