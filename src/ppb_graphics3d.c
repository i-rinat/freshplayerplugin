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
#include <stddef.h>
#include "trace.h"
#include "pp_resource.h"
#include "tables.h"


int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    return 0;
}

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[])
{
    PP_Resource context = pp_resource_allocate(PP_RESOURCE_GRAPHICS3D, instance);
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_warning("%s, can't create context\n", __func__);
        return 0;
    }
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_warning("%s, wrong instance\n", __func__);
        pp_resource_release(context);
        pp_resource_unref(context);
        return 0;
    }

    int attrib_len = 0;
    while (attrib_list[attrib_len] != PP_GRAPHICS3DATTRIB_NONE) {
        attrib_len += 2;
    }
    attrib_len ++;

    EGLint *attrib_list2 = calloc(attrib_len, sizeof(int32_t));
    int done = 0, k1 = 0, k2 = 0;
    while (!done) {
        switch (attrib_list[k1]) {
        case PP_GRAPHICS3DATTRIB_HEIGHT:
            g3d->height = attrib_list[k1 + 1];
            break;
        case PP_GRAPHICS3DATTRIB_WIDTH:
            g3d->width = attrib_list[k1 + 1];
            break;
        case PP_GRAPHICS3DATTRIB_GPU_PREFERENCE:
        case PP_GRAPHICS3DATTRIB_SWAP_BEHAVIOR:
            // TODO: save these values
            break;
        case PP_GRAPHICS3DATTRIB_NONE:
            attrib_list2[k2++] = PP_GRAPHICS3DATTRIB_NONE;
            done = 1;
            break;
        case PP_GRAPHICS3DATTRIB_ALPHA_SIZE:
        case PP_GRAPHICS3DATTRIB_BLUE_SIZE:
        case PP_GRAPHICS3DATTRIB_GREEN_SIZE:
        case PP_GRAPHICS3DATTRIB_RED_SIZE:
        case PP_GRAPHICS3DATTRIB_DEPTH_SIZE:
        case PP_GRAPHICS3DATTRIB_STENCIL_SIZE:
        case PP_GRAPHICS3DATTRIB_SAMPLES:
        case PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS:
            attrib_list2[k2++] = attrib_list[k1];
            attrib_list2[k2++] = attrib_list[k1 + 1];
            break;
        default:
            // skip unknown attribute
            break;
        }
        k1 += 2;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    {
        EGLint nc;
        EGLBoolean retval = eglChooseConfig(pp_i->egl_dpy, attrib_list2, &g3d->egl_config, 1, &nc);
        free(attrib_list2);
        assert(retval);
        assert(nc > 0);
    }

    {
        static const EGLint ctx_attrs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
        };
        // TODO: share context
        g3d->egl_ctx = eglCreateContext(pp_i->egl_dpy, g3d->egl_config, EGL_NO_CONTEXT, ctx_attrs);
        g3d->egl_dpy = pp_i->egl_dpy;
    }

    {
        EGLint pbuffer_attrs[] = {
            EGL_HEIGHT, g3d->height,
            EGL_WIDTH, g3d->width,
            EGL_NONE
        };
        g3d->egl_surf = eglCreatePbufferSurface(g3d->egl_dpy, g3d->egl_config, pbuffer_attrs);
        if (g3d->egl_surf == EGL_NO_SURFACE) {
            trace_error("%s, can't create EGL surface\n", __func__);
        }
    }

    pp_resource_release(context);
    return context;
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
    return 0;
}

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    return 0;
}


// trace wrappers
static
int32_t
trace_ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    trace_info("[PPB] {zilch} %s instance=%d, attribute=%d\n", __func__+6, instance, attribute);
    return ppb_graphics3d_get_attrib_max_value(instance, attribute, value);
}

static
PP_Resource
trace_ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context,
                            const int32_t attrib_list[])
{
    trace_info("[PPB] {part} %s intance=%d, share_context=%d, attrib_list={TODO}\n", __func__+6,
               instance, share_context);
    return ppb_graphics3d_create(instance, share_context, attrib_list);
}

static
PP_Bool
trace_ppb_graphics3d_is_graphics3d(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_graphics3d_is_graphics3d(resource);
}

static
int32_t
trace_ppb_graphics3d_get_attribs(PP_Resource context, int32_t attrib_list[])
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_graphics3d_get_attribs(context, attrib_list);
}

static
int32_t
trace_ppb_graphics3d_set_attribs(PP_Resource context, const int32_t attrib_list[])
{
    trace_info("[PPB] {zilch} %s context=%d, attrib_list={TODO}\n", __func__+6, context);
    return ppb_graphics3d_set_attribs(context, attrib_list);
}

static
int32_t
trace_ppb_graphics3d_get_error(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_graphics3d_get_error(context);
}

static
int32_t
trace_ppb_graphics3d_resize_buffers(PP_Resource context, int32_t width, int32_t height)
{
    trace_info("[PPB] {zilch} %s context=%d, width=%d, height=%d\n", __func__+6, context,
               width, height);
    return ppb_graphics3d_resize_buffers(context, width, height);
}

static
int32_t
trace_ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s context=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, context, callback.func, callback.user_data, callback.flags);
    return ppb_graphics3d_swap_buffers(context, callback);
}


const struct PPB_Graphics3D_1_0 ppb_graphics3d_interface_1_0 = {
    .GetAttribMaxValue =    trace_ppb_graphics3d_get_attrib_max_value,
    .Create =               trace_ppb_graphics3d_create,
    .IsGraphics3D =         trace_ppb_graphics3d_is_graphics3d,
    .GetAttribs =           trace_ppb_graphics3d_get_attribs,
    .SetAttribs =           trace_ppb_graphics3d_set_attribs,
    .GetError =             trace_ppb_graphics3d_get_error,
    .ResizeBuffers =        trace_ppb_graphics3d_resize_buffers,
    .SwapBuffers =          trace_ppb_graphics3d_swap_buffers,
};
