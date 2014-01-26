#include <ppapi/c/ppb_url_loader.h>
#include <stddef.h>
#include "trace.h"
#include "pp_resource.h"


PP_Resource
ppb_url_loader_create(PP_Instance instance)
{
    return pp_resource_allocate(PP_RESOURCE_URL_LOADER);
}

PP_Bool
ppb_url_loader_is_url_loader(PP_Resource resource)
{
    return PP_RESOURCE_URL_LOADER == pp_resource_get_type(resource);
}

int32_t
ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                    struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    return 0;
}

PP_Bool
ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                   int64_t *total_bytes_to_be_sent)
{
    return PP_TRUE;
}

PP_Bool
ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                     int64_t *total_bytes_to_be_received)
{
    return PP_TRUE;
}

PP_Resource
ppb_url_loader_get_response_info(PP_Resource loader)
{
    PP_Resource ri = pp_resource_allocate(PP_RESOURCE_URL_RESPONSE_INFO);
    struct pp_url_response_info_resource_s *rir = pp_resource_acquire(ri);

    rir->url_loader = loader;
    pp_resource_release(ri);
    return ri;
}

int32_t
ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                  struct PP_CompletionCallback callback)
{
    return 0;
}

int32_t
ppb_url_loader_finish_streaming_to_file(PP_Resource loader, struct PP_CompletionCallback callback)
{
    return 0;
}

void
ppb_url_loader_close(PP_Resource loader)
{
    return;
}

// trace wrappers
static
PP_Resource
trace_ppb_url_loader_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_url_loader_create(instance);
}

static
PP_Bool
trace_ppb_url_loader_is_url_loader(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_url_loader_is_url_loader(resource);
}

static
int32_t
trace_ppb_url_loader_open(PP_Resource loader, PP_Resource request_info,
                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, request_info=%d, callback={.func=%p, .user_data=%p, "
               ".flags=%d}\n", __func__+6, loader, request_info, callback.func, callback.user_data,
               callback.flags);
    return ppb_url_loader_open(loader, request_info, callback);
}

static
int32_t
trace_ppb_url_loader_follow_redirect(PP_Resource loader, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_follow_redirect(loader, callback);
}

static
PP_Bool
trace_ppb_url_loader_get_upload_progress(PP_Resource loader, int64_t *bytes_sent,
                                         int64_t *total_bytes_to_be_sent)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_upload_progress(loader, bytes_sent, total_bytes_to_be_sent);
}

static
PP_Bool
trace_ppb_url_loader_get_download_progress(PP_Resource loader, int64_t *bytes_received,
                                           int64_t *total_bytes_to_be_received)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_download_progress(loader, bytes_received, total_bytes_to_be_received);
}

static
PP_Resource
trace_ppb_url_loader_get_response_info(PP_Resource loader)
{
    trace_info("[PPB] {full} %s loader=%d\n", __func__+6, loader);
    return ppb_url_loader_get_response_info(loader);
}

static
int32_t
trace_ppb_url_loader_read_response_body(PP_Resource loader, void *buffer, int32_t bytes_to_read,
                                        struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d, buffer=%p, bytes_to_read=%d, callback={.func=%p, "
               ".user_data=%p, .flags=%d}\n", __func__+6, loader, buffer, bytes_to_read,
               callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_read_response_body(loader, buffer, bytes_to_read, callback);
}

static
int32_t
trace_ppb_url_loader_finish_streaming_to_file(PP_Resource loader,
                                              struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s loader=%d callback={.func=%p, .user_data=%p, flags=%d}\n",
               __func__+6, loader, callback.func, callback.user_data, callback.flags);
    return ppb_url_loader_finish_streaming_to_file(loader, callback);
}

static
void
trace_ppb_url_loader_close(PP_Resource loader)
{
    trace_info("[PPB] {zilch} %s loader=%d\n", __func__+6, loader);
    ppb_url_loader_close(loader);
}


const struct PPB_URLLoader_1_0 ppb_url_loader_interface_1_0 = {
    .Create =               trace_ppb_url_loader_create,
    .IsURLLoader =          trace_ppb_url_loader_is_url_loader,
    .Open =                 trace_ppb_url_loader_open,
    .FollowRedirect =       trace_ppb_url_loader_follow_redirect,
    .GetUploadProgress =    trace_ppb_url_loader_get_upload_progress,
    .GetDownloadProgress =  trace_ppb_url_loader_get_download_progress,
    .GetResponseInfo =      trace_ppb_url_loader_get_response_info,
    .ReadResponseBody =     trace_ppb_url_loader_read_response_body,
    .FinishStreamingToFile = trace_ppb_url_loader_finish_streaming_to_file,
    .Close =                trace_ppb_url_loader_close,
};
