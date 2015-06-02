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
#include "pp_interface.h"
#include "ppb_buffer.h"
#include "ppb_core.h"
#include "ppb_opengles2.h"
#include "ppb_graphics3d.h"


static
void
__attribute__((constructor))
constructor_ppb_video_decoder(void)
{
    avcodec_register_all();
}

static
void
__attribute__((destructor))
destructor_ppb_video_decoder(void)
{
}

enum AVPixelFormat
get_format(struct AVCodecContext *s, const enum AVPixelFormat *fmt)
{
    struct pp_video_decoder_s *vd = s->opaque;
    int have_vaapi = 0;
    int have_vdpau = 0;

    for (int k = 0; fmt[k] != AV_PIX_FMT_NONE; k ++) {
        if (fmt[k] == AV_PIX_FMT_VAAPI_VLD)
            have_vaapi = display.va_available;
        if (fmt[k] == AV_PIX_FMT_VDPAU)
            have_vdpau = 1;
    }

    trace_info_f("      VDPAU:  %s\n", have_vdpau ? "present" : "not present");
    trace_info_f("      VA-API: %s\n", have_vaapi ? "present" : "not present");

    if (have_vaapi) {
        VAStatus status;

        vd->va_context.display = display.va;
        vd->va_context.config_id = VA_INVALID_ID;
        vd->va_context.context_id = VA_INVALID_ID;

        // function is called from libavcodec internals which were already protected by mutex
        status = vaCreateConfig(display.va, VAProfileH264High, VAEntrypointVLD, NULL, 0,
                                &vd->va_context.config_id);
        if (status != VA_STATUS_SUCCESS)
            trace_error("%s, can't create VA config\n", __func__);

#if VA_CHECK_VERSION(0, 34, 0)
        status = vaCreateSurfaces(display.va, VA_RT_FORMAT_YUV420, s->width, s->height,
                                  vd->surfaces, MAX_VIDEO_SURFACES, NULL, 0);
#else
        status = vaCreateSurfaces(display.va, s->width, s->height, VA_RT_FORMAT_YUV420,
                                  MAX_VIDEO_SURFACES, vd->surfaces);
#endif
        if (status != VA_STATUS_SUCCESS)
            trace_error("%s, can't create VA surfaces\n", __func__);

        status = vaCreateContext(display.va, vd->va_context.config_id, s->width, s->height,
                                 VA_PROGRESSIVE, vd->surfaces, MAX_VIDEO_SURFACES,
                                 &vd->va_context.context_id);
        if (status != VA_STATUS_SUCCESS)
            trace_error("%s, can't create VA context\n", __func__);

        vd->avctx->hwaccel_context = &vd->va_context;
        return AV_PIX_FMT_VAAPI_VLD;
    }

    (void)have_vdpau; // TODO: VDPAU support

    // nothing found, using default
    return fmt[0];
}

static
void
release_buffer(void *opaque, uint8_t *data)
{
    struct pp_video_decoder_s *vd = opaque;
    VASurfaceID surface = GPOINTER_TO_SIZE(data);

    for (int k = 0; k < MAX_VIDEO_SURFACES; k ++) {
        if (surface == vd->surfaces[k]) {
            vd->surface_used[k] = 0;
        }
    }
}

static
int
get_buffer2(struct AVCodecContext *s, AVFrame *pic, int flags)
{
    struct pp_video_decoder_s *vd = s->opaque;

    VASurfaceID surface = VA_INVALID_SURFACE;
    for (int k = 0; k < MAX_VIDEO_SURFACES; k ++) {
        if (!vd->surface_used[k]) {
            surface = vd->surfaces[k];
            vd->surface_used[k] = 1;
            break;
        }
    }

    pic->data[0] = GSIZE_TO_POINTER(surface);
    pic->data[1] = NULL;
    pic->data[2] = NULL;
    pic->data[3] = GSIZE_TO_POINTER(surface);

    if (surface == VA_INVALID_SURFACE)
        return -1;

    AVBufferRef *buf = av_buffer_create(pic->data[3], 0, release_buffer, vd, 0);
    if (!buf)
        return -1;
    pic->buf[0] = buf;
    pic->reordered_opaque = s->reordered_opaque;

    return 0;
}

PP_Resource
ppb_video_decoder_create(PP_Instance instance, PP_Resource context, PP_VideoDecoder_Profile profile)
{
    if (!display.va_available) {
        trace_info_f("      no hw acceleration available\n");
        return 0;
    }

    if (!display.glXBindTexImageEXT) {
        trace_info_f("      no glXBindTexImageEXT available\n");
        return 0;
    }

    switch (profile) {
    case PP_VIDEODECODER_H264PROFILE_BASELINE:
    case PP_VIDEODECODER_H264PROFILE_MAIN:
    case PP_VIDEODECODER_H264PROFILE_EXTENDED:
    case PP_VIDEODECODER_H264PROFILE_HIGH:
        // pass, there is an implementation below
        break;

    case PP_VIDEODECODER_H264PROFILE_NONE:
    case PP_VIDEODECODER_H264PROFILE_HIGH10PROFILE:
    case PP_VIDEODECODER_H264PROFILE_HIGH422PROFILE:
    case PP_VIDEODECODER_H264PROFILE_HIGH444PREDICTIVEPROFILE:
    case PP_VIDEODECODER_H264PROFILE_SCALABLEBASELINE:
    case PP_VIDEODECODER_H264PROFILE_SCALABLEHIGH:
    case PP_VIDEODECODER_H264PROFILE_STEREOHIGH:
    case PP_VIDEODECODER_H264PROFILE_MULTIVIEWHIGH:
    case PP_VIDEODECODER_VP8PROFILE_ANY:
    case PP_VIDEODECODER_PROFILE_UNKNOWN:
    default:
        trace_error("%s, profile %d is not supported\n", __func__, profile);
        return 0;
    }

    const struct PPP_VideoDecoder_Dev_0_11 *ppp_video_decoder_dev = NULL;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    ppp_video_decoder_dev = ppp_get_interface(PPP_VIDEODECODER_DEV_INTERFACE);
    if (!ppp_video_decoder_dev) {
        trace_error("%s, no viable %s\n", __func__, PPP_VIDEODECODER_DEV_INTERFACE);
        return 0;
    }

    if (pp_resource_get_type(context) != PP_RESOURCE_GRAPHICS3D) {
        trace_error("%s, bad resource\n", __func__);
        return 0;
    }

    PP_Resource video_decoder = pp_resource_allocate(PP_RESOURCE_VIDEO_DECODER, pp_i);
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, resource allocation failed\n", __func__);
        return 0;
    }

    vd->orig_graphics3d = pp_resource_ref(context);
    vd->ppp_video_decoder_dev = ppp_video_decoder_dev;

    vd->codec_id = AV_CODEC_ID_H264;        // TODO: other codecs
    vd->avcodec = avcodec_find_decoder(vd->codec_id);
    if (!vd->avcodec) {
        trace_error("%s, can't create codec\n", __func__);
        goto err_1;
    }

    vd->avparser = av_parser_init(vd->codec_id);
    if (!vd->avparser) {
        trace_error("%s, can't create parser\n", __func__);
        goto err_1;
    }

    vd->avctx = avcodec_alloc_context3(vd->avcodec);
    if (!vd->avctx) {
        trace_error("%s, can't create codec context\n", __func__);
        goto err_1;
    }

    if (vd->avcodec->capabilities & CODEC_CAP_TRUNCATED) {
        trace_info("%s, codec have CODEC_CAP_TRUNCATED\n", __func__);
        vd->avctx->flags |= CODEC_FLAG_TRUNCATED;
    }

    vd->avctx->opaque = vd;
    vd->avctx->refcounted_frames = 1;
    vd->avctx->thread_count = 1;            // TODO: why?
    vd->avctx->get_format = get_format;
    vd->avctx->get_buffer2 = get_buffer2;

    if (avcodec_open2(vd->avctx, vd->avcodec, NULL) < 0) {
        trace_error("%s, can't open codec\n", __func__);
        goto err_1;
    }

    vd->avframe = av_frame_alloc();
    if (!vd->avframe) {
        trace_error("%s, can't alloc frame\n", __func__);
        goto err_1;
    }

    pp_resource_release(video_decoder);
    return video_decoder;

err_1:
    ppb_video_decoder_destroy_priv(vd);

    pp_resource_release(video_decoder);
    pp_resource_expunge(video_decoder);
    return 0;
}

void
ppb_video_decoder_destroy_priv(void *p)
{
    struct pp_video_decoder_s *vd = p;

    if (vd->orig_graphics3d) {
        pp_resource_unref(vd->orig_graphics3d);
        vd->orig_graphics3d = 0;
    }

    if (vd->graphics3d) {
        pp_resource_unref(vd->graphics3d);
        vd->graphics3d = 0;
    }

    if (vd->avparser) {
        av_parser_close(vd->avparser);
        vd->avparser = NULL;
    }

    if (vd->avctx)
        avcodec_free_context(&vd->avctx);

    if (vd->avframe)
        av_frame_free(&vd->avframe);

    for (uintptr_t k = 0; k < vd->buffer_count; k ++) {
        vd->ppp_video_decoder_dev->DismissPictureBuffer(vd->instance->id, vd->self_id,
                                                        vd->buffers[k].id);
    }
    vd->buffer_count = 0;

    if (vd->va_context.context_id) {
        vaDestroyContext(display.va, vd->va_context.context_id);
        vd->va_context.context_id = 0;
    }

    if (vd->va_context.config_id) {
        vaDestroyConfig(display.va, vd->va_context.config_id);
        vd->va_context.config_id = 0;
    }

    vaDestroySurfaces(display.va, vd->surfaces, MAX_VIDEO_SURFACES);
    for (uintptr_t k = 0; k < MAX_VIDEO_SURFACES; k ++) {
        vd->surfaces[k] = VA_INVALID_SURFACE;
        vd->surface_used[k] = 0;

    }

    for (uintptr_t k = 0; k < vd->buffer_count; k ++) {
        vd->ppp_video_decoder_dev->DismissPictureBuffer(vd->instance->id, vd->self_id,
                                                        vd->buffers[k].id);
        pthread_mutex_lock(&display.lock);
        glXDestroyPixmap(display.x, vd->buffers[k].glx_pixmap);
        XFreePixmap(display.x, vd->buffers[k].pixmap);
        pthread_mutex_unlock(&display.lock);
    }

    vd->buffer_count = 0;
    vd->buffers_were_requested = 0;
    free_and_nullify(vd->buffers);
}

PP_Bool
ppb_video_decoder_is_video_decoder(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_VIDEO_DECODER;
}

static
void
request_buffers(struct pp_video_decoder_s *vd)
{
    const PP_Instance instance = vd->instance->id;
    const struct PP_Size dimensions = { .width = vd->avctx->width,
                                        .height = vd->avctx->height };

    // create auxiliary GL context
    int32_t attribs[] = {
        PP_GRAPHICS3DATTRIB_WIDTH,      vd->avctx->width,
        PP_GRAPHICS3DATTRIB_HEIGHT,     vd->avctx->height,
        PP_GRAPHICS3DATTRIB_RED_SIZE,   8,
        PP_GRAPHICS3DATTRIB_GREEN_SIZE, 8,
        PP_GRAPHICS3DATTRIB_BLUE_SIZE,  8,
        PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
        PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 16,
        GLX_Y_INVERTED_EXT,             True,
        GLX_BIND_TO_TEXTURE_RGBA_EXT,   True,
        PP_GRAPHICS3DATTRIB_NONE,
    };

    vd->graphics3d = ppb_graphics3d_create(vd->instance->id, vd->orig_graphics3d, attribs);
    if (!vd->graphics3d)
        trace_error("%s, can't create graphics3d context\n", __func__);

    pp_resource_release(vd->self_id);
    // TODO: how many surfaces do we need?
    vd->ppp_video_decoder_dev->ProvidePictureBuffers(instance, vd->self_id, 21, &dimensions,
                                                     GL_TEXTURE_2D);
    pp_resource_acquire(vd->self_id, PP_RESOURCE_VIDEO_DECODER);
}

static
uint32_t
find_free_buffer(struct pp_video_decoder_s *vd)
{
    for (uint32_t idx = 0; idx < vd->buffer_count; idx ++) {
        if (!vd->buffers[idx].used) {
            vd->buffers[idx].used = 1;
            return idx;
        }
    }

    return (uint32_t)-1;
}

static
void
issue_frame(struct pp_video_decoder_s *vd, int32_t bitstream_buffer_id)
{
    AVFrame *frame = vd->avframe;
    uint32_t idx = find_free_buffer(vd);

    if (idx == (uint32_t)-1) {
        trace_warning("%s, no free buffer available\n", __func__);
        return;
    }

    struct pp_graphics3d_s *g3d = pp_resource_acquire(vd->graphics3d, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    VASurfaceID va_surf = GPOINTER_TO_SIZE(frame->data[3]);

    pthread_mutex_lock(&display.lock);
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    glBindTexture(GL_TEXTURE_2D, vd->buffers[idx].texture_id);
    display.glXBindTexImageEXT(display.x, vd->buffers[idx].glx_pixmap, GLX_FRONT_EXT, NULL);
    XFlush(display.x);
    vaPutSurface(display.va, va_surf, vd->buffers[idx].pixmap,
                 0, 0, frame->width, frame->height,
                 0, 0, frame->width, frame->height,
                 NULL, 0, VA_FRAME_PICTURE);
    XFlush(display.x);
    glXMakeCurrent(display.x, None, NULL);
    pthread_mutex_unlock(&display.lock);

    pp_resource_release(vd->graphics3d);

    const PP_Instance instance = vd->instance->id;
    const struct PP_Picture_Dev picture = {
        .picture_buffer_id = vd->buffers[idx].id,
        .bitstream_buffer_id = bitstream_buffer_id,
    };

    pp_resource_release(vd->self_id);
    vd->ppp_video_decoder_dev->PictureReady(instance, vd->self_id, &picture);
    pp_resource_acquire(vd->self_id, PP_RESOURCE_VIDEO_DECODER);
}

static
void
decode_frame(struct pp_video_decoder_s *vd, uint8_t *data, size_t data_len,
             int32_t bitstream_buffer_id)
{
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = data;
    packet.size = data_len;

    // libavcodec can call hw functions, which in turn can call Xlib functions,
    // therefore we need to lock
    pthread_mutex_lock(&display.lock);
    int got_frame = 0;
    int len = avcodec_decode_video2(vd->avctx, vd->avframe, &got_frame, &packet);
    pthread_mutex_unlock(&display.lock);
    if (len < 0) {
        trace_error("%s, error %d while decoding frame\n", __func__, len);
        return;
    }

    if (got_frame) {
        if (!vd->buffers_were_requested) {
            request_buffers(vd);
            vd->buffers_were_requested = 1;
        }

        issue_frame(vd, bitstream_buffer_id);
    }
}

int32_t
ppb_video_decoder_decode(PP_Resource video_decoder,
                         const struct PP_VideoBitstreamBuffer_Dev *bitstream_buffer,
                         struct PP_CompletionCallback callback)
{
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    void *rawdata = ppb_buffer_map(bitstream_buffer->data);
    if (!rawdata) {
        trace_error("%s, bad bitstream buffer\n", __func__);
        pp_resource_release(video_decoder);
        return PP_ERROR_FAILED;
    }

    uint8_t *inbuf = rawdata;
    size_t   inbuf_sz = bitstream_buffer->size;

    while (inbuf_sz > 0) {
        uint8_t *outbuf = NULL;
        int      outbuf_sz = 0;
        int len = av_parser_parse2(vd->avparser, vd->avctx, &outbuf, &outbuf_sz,
                                   inbuf, inbuf_sz, 0, 0, AV_NOPTS_VALUE);
        if (outbuf_sz > 0)
            decode_frame(vd, outbuf, outbuf_sz, bitstream_buffer->id);
        inbuf += len;
        inbuf_sz -= len;
    }

    ppb_buffer_unmap(bitstream_buffer->data);

    pp_resource_release(video_decoder);
    ppb_core_call_on_main_thread(0, callback, PP_OK);
    return PP_OK_COMPLETIONPENDING;
}

void
ppb_video_decoder_assign_picture_buffers(PP_Resource video_decoder, uint32_t no_of_buffers,
                                         const struct PP_PictureBuffer_Dev buffers[])
{
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, bad resource\n", __func__);
        goto err_1;
    }

    struct pp_graphics3d_s *g3d = pp_resource_acquire(vd->graphics3d, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad graphics3d context\n", __func__);
        goto err_2;
    }

    vd->buffers = malloc(no_of_buffers * sizeof(*vd->buffers));
    if (!vd->buffers) {
        trace_error("%s, memory allocation failure\n", __func__);
        goto err_3;
    }

    vd->buffer_count = no_of_buffers;
    for (uintptr_t k = 0; k < no_of_buffers; k ++) {
        vd->buffers[k].id =         buffers[k].id;
        vd->buffers[k].width =      buffers[k].size.width;
        vd->buffers[k].height =     buffers[k].size.height;
        vd->buffers[k].texture_id = buffers[k].texture_id;
        vd->buffers[k].used =       0;

        pthread_mutex_lock(&display.lock);
        vd->buffers[k].pixmap = XCreatePixmap(display.x, DefaultRootWindow(display.x),
                                              buffers[k].size.width, buffers[k].size.height,
                                              g3d->depth);
        int tfp_pixmap_attrs[] = {
            GLX_TEXTURE_TARGET_EXT, GLX_TEXTURE_2D_EXT,
            GLX_MIPMAP_TEXTURE_EXT, GL_FALSE,
            GLX_TEXTURE_FORMAT_EXT, GLX_TEXTURE_FORMAT_RGB_EXT,
            GL_NONE
        };
        vd->buffers[k].glx_pixmap = glXCreatePixmap(display.x, g3d->fb_config,
                                                    vd->buffers[k].pixmap, tfp_pixmap_attrs);
        pthread_mutex_unlock(&display.lock);
        if (vd->buffers[k].glx_pixmap == None) {
            trace_error("%s, failed to create GLX pixmap\n", __func__);
            goto err_3;
        }
    }

err_3:
    pp_resource_release(vd->graphics3d);
err_2:
    pp_resource_release(video_decoder);
err_1:
    ;
}

void
ppb_video_decoder_reuse_picture_buffer(PP_Resource video_decoder, int32_t picture_buffer_id)
{
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    for (uintptr_t k = 0; k < vd->buffer_count; k ++) {
        if (vd->buffers[k].id == picture_buffer_id && vd->buffers[k].used)
            vd->buffers[k].used = 0;
    }

    pp_resource_release(video_decoder);
}

int32_t
ppb_video_decoder_flush(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    avcodec_flush_buffers(vd->avctx);

    pp_resource_release(video_decoder);
    ppb_core_call_on_main_thread(0, callback, PP_OK);
    return PP_OK_COMPLETIONPENDING;
}

int32_t
ppb_video_decoder_reset(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    struct pp_video_decoder_s *vd = pp_resource_acquire(video_decoder, PP_RESOURCE_VIDEO_DECODER);
    if (!vd) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    avcodec_flush_buffers(vd->avctx);

    pp_resource_release(video_decoder);
    ppb_core_call_on_main_thread(0, callback, PP_OK);
    return PP_OK_COMPLETIONPENDING;
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
    trace_info("[PPB] {full} %s video_decoder=%d, bitstream_buffer=%p, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, video_decoder,
               bitstream_buffer, callback.func, callback.user_data, callback.flags);
    return ppb_video_decoder_decode(video_decoder, bitstream_buffer, callback);
}

TRACE_WRAPPER
void
trace_ppb_video_decoder_assign_picture_buffers(PP_Resource video_decoder, uint32_t no_of_buffers,
                                               const struct PP_PictureBuffer_Dev buffers[])
{
    trace_info("[PPB] {full} %s video_decoder=%d, no_of_buffers=%u, buffers=%p\n", __func__+6,
               video_decoder, no_of_buffers, buffers);
    return ppb_video_decoder_assign_picture_buffers(video_decoder, no_of_buffers, buffers);
}

TRACE_WRAPPER
void
trace_ppb_video_decoder_reuse_picture_buffer(PP_Resource video_decoder, int32_t picture_buffer_id)
{
    trace_info("[PPB] {full} %s video_decoder=%d, picture_buffer_id=%d\n", __func__+6,
               video_decoder, picture_buffer_id);
    return ppb_video_decoder_reuse_picture_buffer(video_decoder, picture_buffer_id);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_decoder_flush(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s video_decoder=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
               __func__+6, video_decoder, callback.func, callback.user_data, callback.flags);
    return ppb_video_decoder_flush(video_decoder, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_decoder_reset(PP_Resource video_decoder, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s video_decoder=%d, callback={.func=%p, .user_data=%p, .flags=%u}\n",
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
    .Decode =               TWRAPF(ppb_video_decoder_decode),
    .AssignPictureBuffers = TWRAPF(ppb_video_decoder_assign_picture_buffers),
    .ReusePictureBuffer =   TWRAPF(ppb_video_decoder_reuse_picture_buffer),
    .Flush =                TWRAPF(ppb_video_decoder_flush),
    .Reset =                TWRAPF(ppb_video_decoder_reset),
    .Destroy =              TWRAPZ(ppb_video_decoder_destroy),
};
