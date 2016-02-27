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

#include "ppb_video_capture.h"
#include <ppapi/c/pp_errors.h>
#include <stdlib.h>
#include "pp_resource.h"
#include "ppb_core.h"
#include "ppb_message_loop.h"
#include "trace.h"
#include "tables.h"
#include "config.h"
#include "ppb_device_ref.h"
#include "ppb_var.h"
#include "ppb_buffer.h"
#include <linux/videodev2.h>
#include "pp_interface.h"
#include "eintr_retry.h"
#include <dirent.h>
#include <sys/ioctl.h>
#if HAVE_LIBV4L2
#include <libv4l2.h>
#endif // HAVE_LIBV4L2


const char *default_capture_device = "/dev/video0";

#if !HAVE_LIBV4L2
// define simple wrappers, if libv4l2 is not used
static
ssize_t
v4l2_read(int fd, void *buffer, size_t n)
{
    return read(fd, buffer, n);
}

static
int
v4l2_open(const char *filename, int oflag)
{
    return open(filename, oflag);
}

static
int
v4l2_close(int fd)
{
    return close(fd);
}

static
int
v4l2_ioctl(int fd, unsigned long int request, void *data)
{
    return ioctl(fd, request, data);
}
#endif // !HAVE_LIBV4L2


PP_Resource
ppb_video_capture_create(PP_Instance instance)
{
    const struct PPP_VideoCapture_Dev_0_1 *ppp_video_capture_dev;
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    ppp_video_capture_dev = ppp_get_interface(PPP_VIDEO_CAPTURE_DEV_INTERFACE_0_1);
    if (!ppp_video_capture_dev) {
        trace_error("%s, no viable %s\n", __func__, PPP_VIDEO_CAPTURE_DEV_INTERFACE_0_1);
        return 0;
    }

    PP_Resource video_capture = pp_resource_allocate(PP_RESOURCE_VIDEO_CAPTURE, pp_i);
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, resource allocation failure\n", __func__);
        return 0;
    }

    vc->fd = -1;
    vc->ppp_video_capture_dev = ppp_video_capture_dev;

    pp_resource_release(video_capture);
    return video_capture;
}

static
void
ppb_video_capture_destroy(void *p)
{
    struct pp_video_capture_s *vc = p;

    if (vc->fd != -1) {
        v4l2_close(vc->fd);
        vc->fd = -1;
    }

    if (vc->buffers) {
        for (uint32_t k = 0; k < vc->buffer_count; k ++)
            ppb_core_release_resource(vc->buffers[k]);
    }

    free_and_nullify(vc->buffers);
    free_and_nullify(vc->buffer_is_free);
}

PP_Bool
ppb_video_capture_is_video_capture(PP_Resource video_capture)
{
    return pp_resource_get_type(video_capture) == PP_RESOURCE_VIDEO_CAPTURE;
}

static
int
video_device_is_usable(const char *dev, char **shortname)
{
    if (!config.probe_video_capture_devices) {
        // do not probe device, assume it have default name,
        // and is capable of video capturing
        *shortname = g_strdup("Unknown");
        return 1;
    }

    int fd = v4l2_open(dev, O_RDWR);
    if (fd < 0)
        return 0;

    struct v4l2_capability caps;
    if (v4l2_ioctl(fd, VIDIOC_QUERYCAP, &caps) != 0)
        goto err_1;

#ifdef V4L2_CAP_DEVICE_CAPS
    const uint32_t device_caps = (caps.capabilities & V4L2_CAP_DEVICE_CAPS) ? caps.device_caps
                                                                            : caps.capabilities;
#else
    const uint32_t device_caps = caps.capabilities;
#endif // V4L2_CAP_DEVICE_CAPS

    if (!(device_caps & V4L2_CAP_VIDEO_CAPTURE))
        goto err_1;

    if (!(device_caps & V4L2_CAP_READWRITE))
        goto err_1;

    *shortname = g_strdup((char *)caps.card);

    v4l2_close(fd);
    return 1;

err_1:
    v4l2_close(fd);
    return 0;
}

int32_t
ppb_video_capture_enumerate_devices(PP_Resource video_capture, struct PP_ArrayOutput output,
                                    struct PP_CompletionCallback callback)
{
    int32_t retval;
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    GArray *vc_devices = g_array_new(FALSE, TRUE, sizeof(PP_Resource));

    struct dirent **namelist;
    int n = scandir("/dev", &namelist, NULL, NULL);
    if (n >= 0) {
        for (int k = 0; k < n; k ++) {
            if (strncmp(namelist[k]->d_name, "video", sizeof("video") - 1) != 0)
                continue;

            char *fullpath = g_strdup_printf("/dev/%s", namelist[k]->d_name);
            char *shortname = NULL;
            if (video_device_is_usable(fullpath, &shortname)) {
                struct PP_Var v_shortname = ppb_var_var_from_utf8_z(shortname);
                struct PP_Var v_fullpath = ppb_var_var_from_utf8_z(fullpath);
                PP_Resource device = ppb_device_ref_create(vc->instance->id, v_shortname,
                                                           v_fullpath,
                                                           PP_DEVICETYPE_DEV_VIDEOCAPTURE);
                g_array_append_val(vc_devices, device);
                free(shortname);
                ppb_var_release(v_shortname);
                ppb_var_release(v_fullpath);
            }
            g_free(fullpath);
        }

        for (int k = 0; k < n; k ++)
            free(namelist[k]);
        free(namelist);
    }

    PP_Resource *devs = output.GetDataBuffer(output.user_data, vc_devices->len,
                                             sizeof(PP_Resource));
    if (!devs) {
        retval = PP_ERROR_FAILED;
        for (uintptr_t k = 0; k < vc_devices->len; k ++)
            ppb_core_release_resource(g_array_index(vc_devices, PP_Resource, k));
        goto err;
    }

    for (uintptr_t k = 0; k < vc_devices->len; k ++)
        devs[k] = g_array_index(vc_devices, PP_Resource, k);

    retval = PP_OK_COMPLETIONPENDING;
    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, PP_OK, 0,
                                           __func__);

err:
    pp_resource_release(video_capture);
    g_array_free(vc_devices, TRUE);
    return retval;
}

int32_t
ppb_video_capture_monitor_device_change(PP_Resource video_capture,
                                        PP_MonitorDeviceChangeCallback callback, void *user_data)
{
    return 0;
}

int32_t
ppb_video_capture_open(PP_Resource video_capture, PP_Resource device_ref,
                       const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                       uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    int32_t result;
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    const char *capture_device = default_capture_device;
    struct PP_Var longname = ppb_device_ref_get_longname(device_ref);

    if (longname.type == PP_VARTYPE_STRING)
        capture_device = ppb_var_var_to_utf8(longname, NULL);

    vc->fd = v4l2_open(capture_device, O_RDWR);

    ppb_var_release(longname);

    if (vc->fd < 0) {
        result = PP_ERROR_NOACCESS;
        goto point_1;
    }

    struct v4l2_capability caps;
    if (v4l2_ioctl(vc->fd, VIDIOC_QUERYCAP, &caps) != 0) {
        result = PP_ERROR_FAILED;
        goto point_2;
    }

#ifdef V4L2_CAP_DEVICE_CAPS
    const uint32_t device_caps = (caps.capabilities & V4L2_CAP_DEVICE_CAPS) ? caps.device_caps
                                                                            : caps.capabilities;
#else
    const uint32_t device_caps = caps.capabilities;
#endif // V4L2_CAP_DEVICE_CAPS

    if (!(device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
        trace_error("%s, device can't capture\n", __func__);
        result = PP_ERROR_FAILED;
        goto point_2;
    }

    if (!(device_caps & V4L2_CAP_READWRITE)) {
        trace_error("%s, device doesn't support read/write interface\n", __func__);
        result = PP_ERROR_FAILED;
        goto point_2;
    }

    if (requested_info) {
        vc->width =  requested_info->width;
        vc->height = requested_info->height;
        vc->fps =    requested_info->frames_per_second;
    } else {
        vc->width =  640;
        vc->height = 480;
        vc->fps =    15;
    }

    struct v4l2_format fmt = {
        .type =                 V4L2_BUF_TYPE_VIDEO_CAPTURE,
        .fmt.pix.width =        vc->width,
        .fmt.pix.height =       vc->height,
        .fmt.pix.pixelformat =  V4L2_PIX_FMT_YUV420,    // PPAPI hardcodes format to YUV420
        .fmt.pix.field =        V4L2_FIELD_INTERLACED,
    };

    if (v4l2_ioctl(vc->fd, VIDIOC_S_FMT, &fmt) != 0) {
        trace_error("%s, failed to set resolution\n", __func__);
        result = PP_ERROR_FAILED;
        goto point_2;
    }

    vc->width =  fmt.fmt.pix.width;
    vc->height = fmt.fmt.pix.height;

    vc->buffer_size = fmt.fmt.pix.sizeimage;    // buffer size in bytes
    vc->buffer_count = MAX(buffer_count, 5);    // limit lowest number of buffers, just in case

    vc->buffers = calloc(sizeof(*vc->buffers), vc->buffer_count);
    if (!vc->buffers) {
        trace_error("%s, memory allocation failure (1)\n", __func__);
        result = PP_ERROR_FAILED;
        goto point_2;
    }

    vc->buffer_is_free = malloc(sizeof(*vc->buffer_is_free) * vc->buffer_count);
    if (!vc->buffer_is_free) {
        trace_error("%s, memory allocation failure (2)\n", __func__);
        result = PP_ERROR_FAILED;
        goto point_3;
    }

    for (unsigned int k = 0; k < vc->buffer_count; k ++) {
        vc->buffer_is_free[k] = 1;
        vc->buffers[k] = ppb_buffer_create(vc->instance->id, vc->buffer_size);
        if (vc->buffers[k] == 0) {
            result = PP_ERROR_FAILED;
            goto point_4;
        }
    }

    struct PP_VideoCaptureDeviceInfo_Dev info = {
        .width =             vc->width,
        .height =            vc->height,
        .frames_per_second = vc->fps,
    };

    vc->ppp_video_capture_dev->OnDeviceInfo(vc->instance->id, video_capture, &info,
                                            vc->buffer_count, vc->buffers);

    result = PP_OK;
    goto point_1;

point_4:
    for (unsigned int k = 0; k < vc->buffer_count; k ++)
        ppb_core_release_resource(vc->buffers[k]);
    free_and_nullify(vc->buffer_is_free);
point_3:
    free_and_nullify(vc->buffers);
point_2:
    v4l2_close(vc->fd);
    vc->fd = -1;
point_1:
    pp_resource_release(video_capture);

    ppb_message_loop_post_work_with_result(ppb_message_loop_get_current(), callback, 0, result,
                                           0, __func__);
    return PP_OK_COMPLETIONPENDING;
}

struct on_buffer_ready_param_s {
    PP_Instance                            instance;
    PP_Resource                            video_capture;
    uint32_t                               buf_idx;
    const struct PPP_VideoCapture_Dev_0_1 *ppp_video_capture_dev;
};

static
void
on_buffer_ready_comt(void *user_data, int32_t result)
{
    struct on_buffer_ready_param_s *p = user_data;
    struct pp_instance_s *pp_i = tables_get_pp_instance(p->instance);
    if (!pp_i)
        return;

    p->ppp_video_capture_dev->OnBufferReady(p->instance, p->video_capture, p->buf_idx);
    g_slice_free1(sizeof(*p), p);
}

static
void *
video_capture_thread(void *param)
{
    struct pp_video_capture_s *vc = param;

    PP_Resource  video_capture = vc->self_id;
    PP_Instance  instance = vc->instance->id;
    const int    fd = vc->fd;
    const size_t buffer_size = vc->buffer_size;

    vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc)
        goto gone;

    while (!vc->terminate_thread) {
        // find free buffer
        uint32_t buf_idx = (uint32_t)-1;
        for (uint32_t k = 0; k < vc->buffer_count; k ++) {
            if (vc->buffer_is_free[k]) {
                buf_idx = k;
                vc->buffer_is_free[k] = 0;
                break;
            }
        }

        if (buf_idx == (uint32_t)-1) {
            // all buffers are busy, wait for some to free, with resource unlocked
            pp_resource_release(video_capture);
            usleep(10);
            vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
            if (!vc)
                goto gone;
            continue;
        }

        PP_Resource buffer = vc->buffers[buf_idx];
        pp_resource_release(video_capture);

        // wait on v4l2_read() with resource unlocked
        void *ptr = ppb_buffer_map(buffer);
        RETRY_ON_EINTR(v4l2_read(fd, ptr, buffer_size));
        ppb_buffer_unmap(buffer);

        vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
        if (!vc)
            goto gone;

        struct on_buffer_ready_param_s *p = g_slice_alloc(sizeof(*p));
        p->instance =               instance;
        p->video_capture =          video_capture;
        p->buf_idx =                buf_idx;
        p->ppp_video_capture_dev =  vc->ppp_video_capture_dev;
        ppb_message_loop_post_work_with_result(vc->message_loop,
                                               PP_MakeCCB(on_buffer_ready_comt, p), 0, PP_OK, 0,
                                               __func__);
    }

    pp_resource_release(video_capture);
    return NULL;

gone:
    trace_error("%s, resource gone\n", __func__);
    return NULL;
}

int32_t
ppb_video_capture_start_capture(PP_Resource video_capture)
{
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (vc->thread_started)
        goto done;

    if (vc->fd < 0) {
        trace_error("%s, device is closed\n", __func__);
        pp_resource_release(video_capture);
        return PP_ERROR_FAILED;
    }

    // memorize current message loop. It'll be used for callbacks
    vc->message_loop = ppb_message_loop_get_current();

    vc->ppp_video_capture_dev->OnStatus(vc->instance->id, video_capture,
                                        PP_VIDEO_CAPTURE_STATUS_STARTING);

    pp_resource_ref(video_capture); // prevents freeing while thread is still running
    pthread_create(&vc->thread, NULL, video_capture_thread, vc);
    vc->thread_started = 1;

    vc->ppp_video_capture_dev->OnStatus(vc->instance->id, video_capture,
                                        PP_VIDEO_CAPTURE_STATUS_STARTED);

done:
    pp_resource_release(video_capture);
    return PP_OK;
}

int32_t
ppb_video_capture_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (buffer < vc->buffer_count)
        vc->buffer_is_free[buffer] = 1;

    pp_resource_release(video_capture);
    return PP_OK;
}

int32_t
ppb_video_capture_stop_capture(PP_Resource video_capture)
{
    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!vc->thread_started)
        goto done;

    vc->ppp_video_capture_dev->OnStatus(vc->instance->id, video_capture,
                                        PP_VIDEO_CAPTURE_STATUS_STOPPING);

    vc->terminate_thread = 1;
    pthread_t thread = vc->thread;

    pp_resource_release(video_capture);

    pthread_join(thread, NULL);

    vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, resource gone\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    vc->thread_started = 0;
    vc->terminate_thread = 0;
    vc->ppp_video_capture_dev->OnStatus(vc->instance->id, video_capture,
                                        PP_VIDEO_CAPTURE_STATUS_STOPPED);

    pp_resource_unref(video_capture);   // remove reference made in start_capture()

done:
    pp_resource_release(video_capture);
    return PP_OK;
}

void
ppb_video_capture_close(PP_Resource video_capture)
{
    ppb_video_capture_stop_capture(video_capture);

    struct pp_video_capture_s *vc = pp_resource_acquire(video_capture, PP_RESOURCE_VIDEO_CAPTURE);
    if (!vc) {
        trace_error("%s, bad resource\n", __func__);
        return;
    }

    ppb_video_capture_destroy(vc);

    pp_resource_release(video_capture);
    return;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_video_capture_create(PP_Instance instance)
{
    trace_info("[PPB] {full} %s instance=%d\n", __func__+6, instance);
    return ppb_video_capture_create(instance);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_video_capture_is_video_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s video_capture=%d\n", __func__+6, video_capture);
    return ppb_video_capture_is_video_capture(video_capture);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_enumerate_devices(PP_Resource video_capture,
                                          struct PP_ArrayOutput output,
                                          struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s video_capture=%d, output={.GetDataBuffer=%p, .user_data=%p}, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, video_capture,
               output.GetDataBuffer, output.user_data, callback.func, callback.user_data,
               callback.flags);
    return ppb_video_capture_enumerate_devices(video_capture, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_monitor_device_change(PP_Resource video_capture,
                                              PP_MonitorDeviceChangeCallback callback,
                                              void *user_data)
{
    trace_info("[PPB] {zilch} %s video_capture=%d, callback=%p, user_data=%p\n", __func__+6,
               video_capture, callback, user_data);
    return ppb_video_capture_monitor_device_change(video_capture, callback, user_data);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_open(PP_Resource video_capture, PP_Resource device_ref,
                             const struct PP_VideoCaptureDeviceInfo_Dev *requested_info,
                             uint32_t buffer_count, struct PP_CompletionCallback callback)
{
    gchar *s_requested_info;
    if (requested_info) {
        s_requested_info = g_strdup_printf("{.width=%u, .height=%u, .frames_per_second=%u}",
                                           requested_info->width, requested_info->height,
                                           requested_info->frames_per_second);
    } else {
        s_requested_info = g_strdup("(nil)");
    }

    trace_info("[PPB] {full} %s video_capture=%d, device_ref=%d, request_info=%s, buffer_count=%u,"
               " callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, video_capture,
               device_ref, s_requested_info, buffer_count, callback.func, callback.user_data,
               callback.flags);
    g_free(s_requested_info);

    return ppb_video_capture_open(video_capture, device_ref, requested_info, buffer_count,
                                  callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_start_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s video_capture=%d\n", __func__+6, video_capture);
    return ppb_video_capture_start_capture(video_capture);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_reuse_buffer(PP_Resource video_capture, uint32_t buffer)
{
    trace_info("[PPB] {full} %s video_capture=%d, buffer=%u\n", __func__+6, video_capture, buffer);
    return ppb_video_capture_reuse_buffer(video_capture, buffer);
}

TRACE_WRAPPER
int32_t
trace_ppb_video_capture_stop_capture(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s video_capture=%d\n", __func__+6, video_capture);
    return ppb_video_capture_stop_capture(video_capture);
}

TRACE_WRAPPER
void
trace_ppb_video_capture_close(PP_Resource video_capture)
{
    trace_info("[PPB] {full} %s video_capture=%d\n", __func__+6, video_capture);
    ppb_video_capture_close(video_capture);
}


const struct PPB_VideoCapture_Dev_0_3 ppb_video_capture_dev_interface_0_3 = {
    .Create =               TWRAPF(ppb_video_capture_create),
    .IsVideoCapture =       TWRAPF(ppb_video_capture_is_video_capture),
    .EnumerateDevices =     TWRAPF(ppb_video_capture_enumerate_devices),
    .MonitorDeviceChange =  TWRAPZ(ppb_video_capture_monitor_device_change),
    .Open =                 TWRAPF(ppb_video_capture_open),
    .StartCapture =         TWRAPF(ppb_video_capture_start_capture),
    .ReuseBuffer =          TWRAPF(ppb_video_capture_reuse_buffer),
    .StopCapture =          TWRAPF(ppb_video_capture_stop_capture),
    .Close =                TWRAPF(ppb_video_capture_close),
};

static
void
__attribute__((constructor))
constructor_ppb_video_capture(void)
{
    register_interface(PPB_VIDEOCAPTURE_DEV_INTERFACE_0_3, &ppb_video_capture_dev_interface_0_3);
    register_resource(PP_RESOURCE_VIDEO_CAPTURE, ppb_video_capture_destroy);
}
