#include <ppapi/c/ppb_url_loader.h>
#include <stddef.h>


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create = (void *)17,
    .IsURLLoader = (void *)18,
    .Open = (void *)19,
    .FollowRedirect = (void *)20,
    .GetUploadProgress = (void *)21,
    .GetDownloadProgress = (void *)22,
    .GetResponseInfo = (void *)23,
    .ReadResponseBody = (void *)24,
    .FinishStreamingToFile = (void *)25,
    .Close = (void *)26,
};
