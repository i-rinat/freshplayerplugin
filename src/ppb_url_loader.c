#include <ppapi/c/ppb_url_loader.h>
#include <stddef.h>


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create = NULL,
    .IsURLLoader = NULL,
    .Open = NULL,
    .FollowRedirect = NULL,
    .GetUploadProgress = NULL,
    .GetDownloadProgress = NULL,
    .GetResponseInfo = NULL,
    .ReadResponseBody = NULL,
    .FinishStreamingToFile = NULL,
    .Close = NULL
};
