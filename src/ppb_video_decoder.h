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

#ifndef FPP_PPB_VIDEO_DECODER_DEV_H
#define FPP_PPB_VIDEO_DECODER_DEV_H

#include <ppapi/c/dev/ppb_video_decoder_dev.h>


PP_Resource
ppb_video_decoder_create(PP_Instance instance, PP_Resource context,
                         PP_VideoDecoder_Profile profile);

PP_Bool
ppb_video_decoder_is_video_decoder(PP_Resource resource);

int32_t
ppb_video_decoder_decode(PP_Resource video_decoder,
                         const struct PP_VideoBitstreamBuffer_Dev *bitstream_buffer,
                         struct PP_CompletionCallback callback);

void
ppb_video_decoder_assign_picture_buffers(PP_Resource video_decoder, uint32_t no_of_buffers,
                                         const struct PP_PictureBuffer_Dev buffers[]);

void
ppb_video_decoder_reuse_picture_buffer(PP_Resource video_decoder, int32_t picture_buffer_id);

int32_t
ppb_video_decoder_flush(PP_Resource video_decoder, struct PP_CompletionCallback callback);

int32_t
ppb_video_decoder_reset(PP_Resource video_decoder, struct PP_CompletionCallback callback);

void
ppb_video_decoder_destroy(PP_Resource video_decoder);

#endif // FPP_PPB_VIDEO_DECODER_DEV_H
