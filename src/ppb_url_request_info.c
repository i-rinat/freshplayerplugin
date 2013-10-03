#include <ppapi/c/ppb_url_request_info.h>
#include <stddef.h>


const struct PPB_URLRequestInfo_1_0 ppb_url_request_info_interface_1_0 = {
    .Create = (void *)10,
    .IsURLRequestInfo = (void *)11,
    .SetProperty = (void *)12,
    .AppendDataToBody = (void *)13,
    .AppendFileToBody = (void *)14,
};
