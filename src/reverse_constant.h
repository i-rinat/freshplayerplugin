#ifndef FPP__REVERSE_CONSTANT_H
#define FPP__REVERSE_CONSTANT_H

#include <npapi.h>
#include <ppapi/c/ppb_url_response_info.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>

const char *reverse_npp_variable(NPPVariable var);
const char *reverse_npn_variable(NPNVariable var);
const char *reverse_pp_url_response_property(PP_URLResponseProperty prop);
const char *reverse_pp_url_request_property(PP_URLRequestProperty prop);
const char *reverse_pp_text_input_type(PP_TextInput_Type type);

#endif // FPP__REVERSE_CONSTANT_H
