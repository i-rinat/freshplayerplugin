#include <ppapi/c/ppb_url_response_info.h>
#include <stddef.h>


const PPB_URLResponseInfo ppb_url_response_info = {
    .IsURLResponseInfo = NULL,
    .GetProperty = NULL,
    .GetBodyAsFileRef = NULL
};
