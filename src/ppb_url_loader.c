#include <ppapi/c/ppb_url_loader.h>
#include <stddef.h>
#include "trace.h"


static
PP_Resource
ppb_url_loader_create(PP_Instance instance)
{
    trace_info("[PPB] {zilch} %s instance=%d\n", __func__, instance);
    return 0;
}

static
PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource)
{
    trace_info("[PPB] {zilch} %s resource=%d\n", __func__, resource);
    return PP_TRUE;
}

static
int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, request_info=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}\n", __func__, loader, request_info, callback.func, callback.user_data,
               callback.flags);
    return 0;
}

static
int32_t
ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, callback={.func=%p, .user_data=%p, .flags=%p}\n",
               __func__, loader, callback.func, callback.user_data, callback.flags);
    return 0;
}

static
PP_Bool
ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                   int64_t *total_bytes_to_be_sent)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__, loader);
    return PP_TRUE;
}

static
PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__, loader);
    return PP_TRUE;
}

static
PP_Resource
ppb_url_loader_get_response_info(PP_Resource loader)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__, loader);
    return 0;
}

static
int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, buffer=%p, bytes_to_read=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%p}\n", __func__, loader, buffer, bytes_to_read,
               callback.func, callback.user_data, callback.flags);
    return 0;
}

static
int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d callback={.func=%p, .user_data=%p, flags=%d}\n",
               __func__, callback.func, callback.user_data, callback.flags);
    return 0;
}

static
void
ppb_url_loader_close(PP_Resource loader)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__, loader);
}


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create =               ppb_url_loader_create,
    .IsURLLoader =          ppb_url_loader_is_url_loader,
    .Open =                 ppb_url_loader_open,
    .FollowRedirect =       ppb_url_loader_follow_redirect,
    .GetUploadProgress =    ppb_url_loader_get_upload_progress,
    .GetDownloadProgress =  ppb_url_loader_get_download_progress,
    .GetResponseInfo =      ppb_url_loader_get_response_info,
    .ReadResponseBody =     ppb_url_loader_read_response_body,
    .FinishStreamingToFile = ppb_url_loader_finish_streaming_to_file,
    .Close =                ppb_url_loader_close,
};
