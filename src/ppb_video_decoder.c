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

#include "ppb_video_decoder.h"
#include <stdlib.h>
#include <ppapi/c/pp_errors.h>
#include "trace.h"
#include "tables.h"
#include "reverse_constant.h"


PP_Resource
ppb_video_decoder_create(PP_Instance instance, PP_Resource context, PP_VideoDecoder_Profile profile)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource video_decoder = pp_resource_allocate(PP_RESOURCE_VIDEO_DECODER, pp_i);

    return video_decoder;
}

void
ppb_video_decoder_destroy_priv(void *p)
{
}

PP_Bool
ppb_video_decoder_is_video_decoder(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_VIDEO_DECODER;
}

int32_t
ppb_video_decoder_decode(PP_Resource video_decoder,
                         const struct PP_VideoBitstreamBuffer_Dev *bitstream_buffer,
                         struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

void
ppb_video_decoder_assign_picture_buffers(PP_Resource video_decoder, uint32_t no_of_buffers,
                                         const struct PP_PictureBuffer_Dev buffers[])
{
}

void
ppb_video_decoder_reuse_picture_buffer(PP_Resource video_decoder, int32_t picture_buffer_id)
{
}

int32_t
ppb_video_decoder_flush(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

int32_t
ppb_video_decoder_reset(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    return PP_ERROR_FAILED;
}

void
ppb_video_decoder_destroy(PP_Resource video_decoder)
{
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_video_decoder_create(PP_Instance instance, PP_Resource context,
                               PP_VideoDecoder_Profile profile)
{
    
    trace_info("[PPB] {full} %s instance=%d, context=%d, profile=%s\n", __func__+6, instance,
               context, reverse_video_decoder_profile(profile));
    return ppb_video_decoder_create(instance, context, profile);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_video_decoder_is_video_decoder(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_video_decoder_is_video_decoder(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_decoder_decode(PP_Resource video_decoder,
                               const struct PP_VideoBitstreamBuffer_Dev *bitstream_buffer,
                               struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s video_decoder=%d, bitstream_buffer=%p, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, video_decoder,
               bitstream_buffer, callback.func, callback.user_data, callback.flags);
    return ppb_video_decoder_decode(video_decoder, bitstream_buffer, callback);
}

TRACE_WRAPPER
void
trace_ppb_video_decoder_assign_picture_buffers(PP_Resource video_decoder, uint32_t no_of_buffers,
                                               const struct PP_PictureBuffer_Dev buffers[])
{
    trace_info("[PPB] {zilch} %s video_decoder=%d, no_of_buffers=%u, buffers=%p\n", __func__+6,
               video_decoder, no_of_buffers, buffers);
    return ppb_video_decoder_assign_picture_buffers(video_decoder, no_of_buffers, buffers);
}

TRACE_WRAPPER
void
trace_ppb_video_decoder_reuse_picture_buffer(PP_Resource video_decoder, int32_t picture_buffer_id)
{
    trace_info("[PPB] {zilch} %s video_decoder=%d, picture_buffer_id=%d\n", __func__+6,
               video_decoder, picture_buffer_id);
    return ppb_video_decoder_reuse_picture_buffer(video_decoder, picture_buffer_id);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_decoder_flush(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s video_decoder=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, video_decoder, callback.func, callback.user_data, callback.flags);
    return ppb_video_decoder_flush(video_decoder, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_decoder_reset(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s video_decoder=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, video_decoder, callback.func, callback.user_data, callback.flags);
    return ppb_video_decoder_reset(video_decoder, callback);
}

TRACE_WRAPPER
void
trace_ppb_video_decoder_destroy(PP_Resource video_decoder)
{
    trace_info("[PPB] {zilch} %s video_decoder=%d\n", __func__+6, video_decoder);
    return ppb_video_decoder_destroy(video_decoder);
}

const struct PPB_VideoDecoder_Dev_0_16 ppb_video_decoder_dev_interface_0_16 = {
    .Create =               TWRAPF(ppb_video_decoder_create),
    .IsVideoDecoder =       TWRAPF(ppb_video_decoder_is_video_decoder),
    .Decode =               TWRAPZ(ppb_video_decoder_decode),
    .AssignPictureBuffers = TWRAPZ(ppb_video_decoder_assign_picture_buffers),
    .ReusePictureBuffer =   TWRAPZ(ppb_video_decoder_reuse_picture_buffer),
    .Flush =                TWRAPZ(ppb_video_decoder_flush),
    .Reset =                TWRAPZ(ppb_video_decoder_reset),
    .Destroy =              TWRAPZ(ppb_video_decoder_destroy),
};
