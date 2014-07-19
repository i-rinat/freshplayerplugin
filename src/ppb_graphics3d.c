/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#include <assert.h>
#include "ppb_graphics3d.h"
#include <stdlib.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "trace.h"
#include "pp_resource.h"
#include "tables.h"
#include <ppapi/c/pp_errors.h>
#include "ppb_core.h"


int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    return 0;
}

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[])
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, wrong instance\n", __func__);
        return 0;
    }

    PP_Resource context = pp_resource_allocate(PP_RESOURCE_GRAPHICS3D, pp_i);
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, can't create context\n", __func__);
        return 0;
    }

    int attrib_len = 0;
    while (attrib_list[attrib_len] != PP_GRAPHICS3DATTRIB_NONE) {
        attrib_len += 2;
    }
    attrib_len ++;

    EGLint *egl_attribute_list = calloc(attrib_len + 3 * 2, sizeof(EGLint));
    int done = 0, k1 = 0, k2 = 0;
    egl_attribute_list[k2++] = EGL_SURFACE_TYPE;
    egl_attribute_list[k2++] = EGL_PIXMAP_BIT | EGL_WINDOW_BIT;
    egl_attribute_list[k2++] = EGL_RENDERABLE_TYPE;
    egl_attribute_list[k2++] = EGL_OPENGL_ES2_BIT;

    while (!done) {
        switch (attrib_list[k1]) {
        case PP_GRAPHICS3DATTRIB_HEIGHT:
            g3d->height = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_WIDTH:
            g3d->width = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GPU_PREFERENCE:
        case PP_GRAPHICS3DATTRIB_SWAP_BEHAVIOR:
            // TODO: save these values
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_NONE:
            egl_attribute_list[k2++] = EGL_NONE;
            done = 1;
            break;
        case PP_GRAPHICS3DATTRIB_ALPHA_SIZE:
            egl_attribute_list[k2++] = EGL_ALPHA_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_BLUE_SIZE:
            egl_attribute_list[k2++] = EGL_BLUE_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GREEN_SIZE:
            egl_attribute_list[k2++] = EGL_GREEN_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_RED_SIZE:
            egl_attribute_list[k2++] = EGL_RED_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_DEPTH_SIZE:
            egl_attribute_list[k2++] = EGL_DEPTH_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_STENCIL_SIZE:
            egl_attribute_list[k2++] = EGL_STENCIL_SIZE;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLES:
            egl_attribute_list[k2++] = EGL_SAMPLES;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS:
            egl_attribute_list[k2++] = EGL_SAMPLE_BUFFERS;
            egl_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        default:
            // skip unknown attribute
            trace_error("%s, unknown attribute 0x%x\n", __func__, attrib_list[k1]);
            k1 += 1;
            break;
        }
    }

    pthread_mutex_lock(&pp_i->lock);
    int nconfigs = 0;
    EGLBoolean ret = eglChooseConfig(pp_i->egl_dpy, egl_attribute_list,
                                     &g3d->egl_config, 1, &nconfigs);
    free(egl_attribute_list);
    if (!ret) {
        trace_error("%s, eglChooseConfig returned FALSE\n", __func__);
        goto err;
    }

    if (nconfigs != 1) {
        trace_error("%s, eglChooseConfig returned %d configs, expected 1\n", __func__, nconfigs);
        goto err;
    }

    EGLint ctxattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    g3d->glc = eglCreateContext(pp_i->egl_dpy, g3d->egl_config, EGL_NO_CONTEXT, ctxattr);
    if (g3d->glc == EGL_NO_CONTEXT) {
        trace_error("%s, eglCreateContext returned EGL_NO_CONTEXT\n", __func__);
        goto err;
    }

    g3d->pixmap = XCreatePixmap(pp_i->dpy, DefaultRootWindow(pp_i->dpy), g3d->width, g3d->height,
                                DefaultDepth(pp_i->dpy, 0));
    g3d->egl_surf = eglCreatePixmapSurface(pp_i->egl_dpy, g3d->egl_config, g3d->pixmap, NULL);
    if (g3d->egl_surf == EGL_NO_SURFACE) {
        trace_error("%s, failed to create EGL pixmap surface\n", __func__);
        goto err;
    }
    g3d->egl_dpy = pp_i->egl_dpy;
    g3d->dpy = pp_i->dpy;

    ret = eglMakeCurrent(g3d->egl_dpy, g3d->egl_surf, g3d->egl_surf, g3d->glc);
    if (!ret) {
        trace_error("%s, eglMakeCurrent failed\n", __func__);
        goto err;
    }

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    g3d->sub_maps = g_hash_table_new(g_direct_hash, g_direct_equal);
    pthread_mutex_unlock(&pp_i->lock);

    pp_resource_release(context);
    return context;
err:
    pthread_mutex_unlock(&pp_i->lock);
    pp_resource_release(context);
    pp_resource_expunge(context);
    return 0;
}

void
ppb_graphics3d_destroy(void *p)
{
    struct pp_graphics3d_s *g3d = p;
    struct pp_instance_s *pp_i = g3d->instance;
    g_hash_table_destroy(g3d->sub_maps);

    pthread_mutex_lock(&pp_i->lock);

    // bringing egl_surf to current thread releases it from any others
    eglMakeCurrent(g3d->egl_dpy, g3d->egl_surf, g3d->egl_surf, g3d->glc);
    // free it here, to be able to destroy X Pixmap
    eglMakeCurrent(g3d->egl_dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    eglDestroySurface(g3d->egl_dpy, g3d->egl_surf);
    XFreePixmap(g3d->dpy, g3d->pixmap);

    eglDestroyContext(g3d->egl_dpy, g3d->glc);
    pthread_mutex_unlock(&pp_i->lock);
}

PP_Bool
ppb_graphics3d_is_graphics3d(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_GRAPHICS3D;
}

int32_t
ppb_graphics3d_get_attribs(PP_Resource context, int32_t attrib_list[])
{
    return 0;
}

int32_t
ppb_graphics3d_set_attribs(PP_Resource context, const int32_t attrib_list[])
{
    return 0;
}

int32_t
ppb_graphics3d_get_error(PP_Resource context)
{
    return 0;
}

int32_t
ppb_graphics3d_resize_buffers(PP_Resource context, int32_t width, int32_t height)
{
    if (width < 0 || height < 0)
        return PP_ERROR_BADARGUMENT;

    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d)
        return PP_ERROR_BADRESOURCE;
    struct pp_instance_s *pp_i = g3d->instance;

    g3d->width = width;
    g3d->height = height;

    pthread_mutex_lock(&pp_i->lock);

    EGLSurface old_surf = g3d->egl_surf;
    Pixmap old_pixmap = g3d->pixmap;
    // release possibly bound to other thread g3d->egl_surf and bind it to current
    eglMakeCurrent(g3d->egl_dpy, g3d->egl_surf, g3d->egl_surf, g3d->glc);

    g3d->pixmap = XCreatePixmap(g3d->dpy, DefaultRootWindow(g3d->dpy), g3d->width, g3d->height,
                                DefaultDepth(g3d->dpy, 0));
    g3d->egl_surf = eglCreatePixmapSurface(g3d->egl_dpy, g3d->egl_config, g3d->pixmap, NULL);

    // make new g3d->egl_surf current to current thread to release old_surf
    eglMakeCurrent(g3d->egl_dpy, g3d->egl_surf, g3d->egl_surf, g3d->glc);

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // destroy old egl surface and x pixmap
    eglDestroySurface(g3d->egl_dpy, old_surf);
    XFreePixmap(g3d->dpy, old_pixmap);

    pthread_mutex_unlock(&pp_i->lock);
    pp_resource_release(context);
    return PP_OK;
}

static
void
_call_invalidaterect_ptac(void *param)
{
    struct pp_instance_s *pp_i = param;
    NPRect npr = {.top = 0, .left = 0, .bottom = pp_i->height, .right = pp_i->width};

    npn.invalidaterect(pp_i->npp, &npr);
    npn.forceredraw(pp_i->npp);
}

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d)
        return PP_ERROR_BADRESOURCE;

    struct pp_instance_s *pp_i = g3d->instance;

    pthread_mutex_lock(&pp_i->lock);
    if (pp_i->graphics != context) {
        // Other context bound, do nothing.
        pp_resource_release(context);
        pthread_mutex_unlock(&pp_i->lock);
        return PP_ERROR_FAILED;
    }

    if (pp_i->graphics_in_progress) {
        pp_resource_release(context);
        pthread_mutex_unlock(&pp_i->lock);
        return PP_ERROR_INPROGRESS;
    }
    pp_resource_release(context);

    pp_i->graphics_ccb = callback;
    pp_i->graphics_in_progress = 1;
    if (!callback.func)
        pthread_barrier_init(&pp_i->graphics_barrier, NULL, 2);

    if (pp_i->is_fullscreen) {
        XGraphicsExposeEvent ev = {
            .type = GraphicsExpose,
            .drawable = pp_i->fs_wnd,
            .width = pp_i->width,
            .height = pp_i->height
        };

        XSendEvent(pp_i->dpy, pp_i->fs_wnd, True, ExposureMask, (void *)&ev);
        XFlush(pp_i->dpy);
        pthread_mutex_unlock(&pp_i->lock);
    } else {
        pthread_mutex_unlock(&pp_i->lock);
        npn.pluginthreadasynccall(pp_i->npp, _call_invalidaterect_ptac, pp_i);
    }

    if (callback.func)
        return PP_OK_COMPLETIONPENDING;

    pthread_barrier_wait(&pp_i->graphics_barrier);
    return PP_OK;
}


// trace wrappers
TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    trace_info("[PPB] {zilch} %s instance=%d, attribute=%d\n", __func__+6, instance, attribute);
    return ppb_graphics3d_get_attrib_max_value(instance, attribute, value);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context,
                            const int32_t attrib_list[])
{
    trace_info("[PPB] {full} %s intance=%d, share_context=%d, attrib_list={TODO}\n", __func__+6,
               instance, share_context);
    return ppb_graphics3d_create(instance, share_context, attrib_list);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_graphics3d_is_graphics3d(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_graphics3d_is_graphics3d(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_get_attribs(PP_Resource context, int32_t attrib_list[])
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_graphics3d_get_attribs(context, attrib_list);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_set_attribs(PP_Resource context, const int32_t attrib_list[])
{
    trace_info("[PPB] {zilch} %s context=%d, attrib_list={TODO}\n", __func__+6, context);
    return ppb_graphics3d_set_attribs(context, attrib_list);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_get_error(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_graphics3d_get_error(context);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_resize_buffers(PP_Resource context, int32_t width, int32_t height)
{
    trace_info("[PPB] {full} %s context=%d, width=%d, height=%d\n", __func__+6, context,
               width, height);
    return ppb_graphics3d_resize_buffers(context, width, height);
}

TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s context=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, context, callback.func, callback.user_data, callback.flags);
    return ppb_graphics3d_swap_buffers(context, callback);
}


const struct PPB_Graphics3D_1_0 ppb_graphics3d_interface_1_0 = {
    .GetAttribMaxValue =    TWRAPZ(ppb_graphics3d_get_attrib_max_value),
    .Create =               TWRAPF(ppb_graphics3d_create),
    .IsGraphics3D =         TWRAPF(ppb_graphics3d_is_graphics3d),
    .GetAttribs =           TWRAPZ(ppb_graphics3d_get_attribs),
    .SetAttribs =           TWRAPZ(ppb_graphics3d_set_attribs),
    .GetError =             TWRAPZ(ppb_graphics3d_get_error),
    .ResizeBuffers =        TWRAPF(ppb_graphics3d_resize_buffers),
    .SwapBuffers =          TWRAPF(ppb_graphics3d_swap_buffers),
};
