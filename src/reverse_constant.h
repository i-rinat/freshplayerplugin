/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#ifndef FPP__REVERSE_CONSTANT_H
#define FPP__REVERSE_CONSTANT_H

#include <npapi/npapi.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <ppapi/c/private/ppb_flash.h>
#include <ppapi/c/ppb_image_data.h>


const char *reverse_npp_variable(NPPVariable var);
const char *reverse_npn_variable(NPNVariable var);
const char *reverse_pp_url_response_property(PP_URLResponseProperty prop);
const char *reverse_pp_url_request_property(PP_URLRequestProperty prop);
const char *reverse_pp_text_input_type(PP_TextInput_Type_Dev type);
const char *reverse_pp_flash_setting(PP_FlashSetting setting);
const char *reverse_pp_image_data_format(PP_ImageDataFormat fmt);
const char *reverse_xevent_type(int type);
const char *reverse_char_set_conversion_error(enum PP_CharSet_ConversionError e);

#endif // FPP__REVERSE_CONSTANT_H
