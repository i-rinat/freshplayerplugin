/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#ifndef FPP_PPB_DEVICE_REF_H
#define FPP_PPB_DEVICE_REF_H

#include <ppapi/c/pp_instance.h>
#include <ppapi/c/dev/ppb_device_ref_dev.h>


PP_Resource
ppb_device_ref_create(PP_Instance instance, struct PP_Var name, struct PP_Var longname,
                      PP_DeviceType_Dev type);

PP_Bool
ppb_device_ref_is_device_ref(PP_Resource resource);

PP_DeviceType_Dev
ppb_device_ref_get_type(PP_Resource device_ref);

struct PP_Var
ppb_device_ref_get_name(PP_Resource device_ref);

struct PP_Var
ppb_device_ref_get_longname(PP_Resource device_ref);

#endif // FPP_PPB_DEVICE_REF_H
