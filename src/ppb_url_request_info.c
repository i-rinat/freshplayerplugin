#include <ppapi/c/ppb_url_request_info.h>
#include <stddef.h>


const PPB_URLRequestInfo ppb_url_request_info = {
    .Create = NULL,
    .IsURLRequestInfo = NULL,
    .SetProperty = NULL,
    .AppendDataToBody = NULL,
    .AppendFileToBody = NULL
};
