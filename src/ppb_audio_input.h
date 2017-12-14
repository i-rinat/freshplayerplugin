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

#pragma once

#include <ppapi/c/dev/ppb_audio_input_dev.h>

PP_Resource
ppb_audio_input_create(PP_Instance instance);

PP_Bool
ppb_audio_input_is_audio_input(PP_Resource resource);

int32_t
ppb_audio_input_enumerate_devices(PP_Resource audio_input, struct PP_ArrayOutput output,
                                  struct PP_CompletionCallback callback);

int32_t
ppb_audio_input_monitor_device_change(PP_Resource audio_input,
                                      PP_MonitorDeviceChangeCallback callback, void *user_data);

int32_t
ppb_audio_input_open_0_3(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                         PPB_AudioInput_Callback_0_3 audio_input_callback, void *user_data,
                         struct PP_CompletionCallback callback);

int32_t
ppb_audio_input_open(PP_Resource audio_input, PP_Resource device_ref, PP_Resource config,
                     PPB_AudioInput_Callback audio_input_callback, void *user_data,
                     struct PP_CompletionCallback callback);

PP_Resource
ppb_audio_input_get_current_config(PP_Resource audio_input);

PP_Bool
ppb_audio_input_start_capture(PP_Resource audio_input);

PP_Bool
ppb_audio_input_stop_capture(PP_Resource audio_input);

void
ppb_audio_input_close(PP_Resource audio_input);
