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

#ifndef FPP_REVERSE_CONSTANT_H
#define FPP_REVERSE_CONSTANT_H

#include <npapi/npapi.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/dev/ppb_char_set_dev.h>
#include <ppapi/c/private/ppb_flash.h>
#include <ppapi/c/ppb_image_data.h>
#include <ppapi/c/private/ppb_flash_clipboard.h>
#include <ppapi/c/private/ppb_udp_socket_private.h>
#include <ppapi/c/dev/pp_video_dev.h>
#include <ppapi/c/dev/ppb_file_chooser_dev.h>
#include <ppapi/c/private/ppb_x509_certificate_private.h>
#include <ppapi/c/private/ppb_pdf.h>


const char *reverse_npp_variable(NPPVariable var);
const char *reverse_npn_variable(NPNVariable var);
const char *reverse_pp_url_response_property(PP_URLResponseProperty prop);
const char *reverse_pp_url_request_property(PP_URLRequestProperty prop);
const char *reverse_pp_text_input_type(PP_TextInput_Type_Dev type);
const char *reverse_pp_flash_setting(PP_FlashSetting setting);
const char *reverse_pp_image_data_format(PP_ImageDataFormat fmt);
const char *reverse_xevent_type(int type);
const char *reverse_char_set_conversion_error(enum PP_CharSet_ConversionError e);
const char *reverse_gl_enum(unsigned int e);
const char *reverse_clipboard_type(PP_Flash_Clipboard_Type ct);
const char *reverse_clipboard_format(uint32_t cf);
const char *reverse_video_decoder_profile(PP_VideoDecoder_Profile profile);
const char *reverse_file_chooser_mode(PP_FileChooserMode_Dev mode);
const char *reverse_udp_socket_feature(PP_UDPSocketFeature_Private feature);
const char *reverse_x509_certificate_field(PP_X509Certificate_Private_Field field);
const char *reverse_pdf_feature(PP_PDFFeature feature);
const char *reverse_private_font_charset(PP_PrivateFontCharset charset);
const char *reverse_graphics3d_attribute(int32_t attr);

#endif // FPP_REVERSE_CONSTANT_H
