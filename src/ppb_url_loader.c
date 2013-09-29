#include <ppapi/c/ppb_url_loader.h>
#include <stddef.h>


const PPB_URLLoader ppb_url_loader = {
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
