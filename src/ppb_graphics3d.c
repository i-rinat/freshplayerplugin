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

#define GL_GLEXT_PROTOTYPES
#include <assert.h>
#include "ppb_graphics3d.h"
#include <stdlib.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>
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

    PP_Resource context = pp_resource_allocate(PP_RESOURCE_GRAPHICS3D, instance);
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

    int *fb_attribute_list = calloc(attrib_len + 3 * 2, sizeof(int));
    int done = 0, k1 = 0, k2 = 0;
    fb_attribute_list[k2++] = GLX_DRAWABLE_TYPE;
    fb_attribute_list[k2++] = GLX_PIXMAP_BIT;
    fb_attribute_list[k2++] = GLX_RENDER_TYPE;
    fb_attribute_list[k2++] = GLX_RGBA_BIT;

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
            fb_attribute_list[k2++] = None;
            done = 1;
            break;
        case PP_GRAPHICS3DATTRIB_ALPHA_SIZE:
            fb_attribute_list[k2++] = GLX_ALPHA_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_BLUE_SIZE:
            fb_attribute_list[k2++] = GLX_BLUE_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GREEN_SIZE:
            fb_attribute_list[k2++] = GLX_GREEN_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_RED_SIZE:
            fb_attribute_list[k2++] = GLX_RED_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_DEPTH_SIZE:
            fb_attribute_list[k2++] = GLX_DEPTH_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_STENCIL_SIZE:
            fb_attribute_list[k2++] = GLX_STENCIL_SIZE;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLES:
            fb_attribute_list[k2++] = GLX_SAMPLES;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS:
            fb_attribute_list[k2++] = GLX_SAMPLE_BUFFERS;
            fb_attribute_list[k2++] = attrib_list[k1 + 1];
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
    GLXFBConfig *fb_configs = glXChooseFBConfig(pp_i->dpy, 0, fb_attribute_list, &nconfigs);
    if (!fb_configs || nconfigs == 0) {
        trace_error("%s, glXChooseFBConfig returned NULL\n", __func__);
        goto err;
    }

    g3d->fb_config = fb_configs[0];
    g3d->rendering_glc = glXCreateNewContext(pp_i->dpy, g3d->fb_config, GLX_RGBA_TYPE, NULL, True);
    if (!g3d->rendering_glc) {
        trace_error("%s, glXCreateNewContext returned NULL\n", __func__);
        goto err;
    }

    // rendering context will be used by ppb_opengles2. Its state can be changed by client
    // code so we need "clean" context for presentation purposes only
    g3d->presentation_glc = glXCreateNewContext(pp_i->dpy, g3d->fb_config, GLX_RGBA_TYPE,
                                                g3d->rendering_glc, True);
    if (!g3d->presentation_glc) {
        trace_error("%s, glXCreateNewContext returned NULL\n", __func__);
        goto err;
    }

    g3d->pixmap = XCreatePixmap(pp_i->dpy, DefaultRootWindow(pp_i->dpy), g3d->width, g3d->height,
                                DefaultDepth(pp_i->dpy, 0));

    g3d->glx_pixmap = glXCreatePixmap(pp_i->dpy, fb_configs[0], g3d->pixmap, NULL);
    g3d->dpy = pp_i->dpy;

    Bool ret = glXMakeCurrent(g3d->dpy, g3d->glx_pixmap, g3d->rendering_glc);
    if (!ret) {
        trace_error("%s, glXMakeCurrent failed\n", __func__);
        goto err;
    }

    // create framebuffer object backed by texture
    glGenTextures(1, &g3d->tex_id);
    glBindTexture(GL_TEXTURE_2D, g3d->tex_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g3d->width, g3d->height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 NULL);

    glGenFramebuffers(1, &g3d->fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, g3d->fbo_id);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g3d->tex_id, 0);
    GLenum gl_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (GL_FRAMEBUFFER_COMPLETE != gl_status) {
        trace_error("%s, framebuffer not ready, %d, %s\n", __func__, gl_status,
                    gluErrorString(gl_status));
        goto err;
    }

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
    g_hash_table_destroy(g3d->sub_maps);

    glXMakeCurrent(g3d->dpy, g3d->glx_pixmap, g3d->rendering_glc);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &g3d->fbo_id);
    glDeleteTextures(1, &g3d->tex_id);

    glXDestroyPixmap(g3d->dpy, g3d->glx_pixmap);
    XFreePixmap(g3d->dpy, g3d->pixmap);

    // TODO: restore previous context
    glXDestroyContext(g3d->dpy, g3d->rendering_glc);
    glXDestroyContext(g3d->dpy, g3d->presentation_glc);
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

    g3d->width = width;
    g3d->height = height;

    glXDestroyPixmap(g3d->dpy, g3d->glx_pixmap);
    XFreePixmap(g3d->dpy, g3d->pixmap);

    g3d->pixmap = XCreatePixmap(g3d->dpy, DefaultRootWindow(g3d->dpy), g3d->width, g3d->height,
                                DefaultDepth(g3d->dpy, 0));
    g3d->glx_pixmap = glXCreatePixmap(g3d->dpy, g3d->fb_config, g3d->pixmap, NULL);

    glXMakeCurrent(g3d->dpy, g3d->glx_pixmap, g3d->presentation_glc);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g3d->width, g3d->height, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                 NULL);

    pp_resource_release(context);
    return PP_OK;
}

void
_swap_buffers_comt(void *p)
{
    struct PP_CompletionCallback *ccb = p;

    if (ccb->func)
        ccb->func(ccb->user_data, PP_OK);

    g_slice_free(struct PP_CompletionCallback, ccb);
}

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d)
        return PP_ERROR_BADRESOURCE;

    struct pp_instance_s *pp_i = tables_get_pp_instance(g3d->_.instance);

    if (pp_i->graphics != context) {
        // Other context bound, do nothing.
        pp_resource_release(context);
        return PP_OK;
    }

    if (pp_i->is_fullscreen) {
        XGraphicsExposeEvent ev = {
            .type = GraphicsExpose,
            .drawable = pp_i->fs_wnd,
            .width = pp_i->width,
            .height = pp_i->height
        };

        XSendEvent(pp_i->dpy, pp_i->fs_wnd, True, ExposureMask, (void *)&ev);
        XFlush(pp_i->dpy);
    } else {
        NPRect npr = {.top = 0, .left = 0, .bottom = g3d->height, .right = g3d->width};
        npn.invalidaterect(pp_i->npp, &npr);
        npn.forceredraw(pp_i->npp);
    }
    pp_resource_release(context);

    struct PP_CompletionCallback *ccb = g_slice_alloc(sizeof(*ccb));
    *ccb = callback;
    npn.pluginthreadasynccall(pp_i->npp, _swap_buffers_comt, ccb);

    return PP_OK;
}


#ifndef NDEBUG
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
    trace_info("[PPB] {full} %s context=%d, width=%d, height=%d\n", __func__+6, context,
               width, height);
    return ppb_graphics3d_resize_buffers(context, width, height);
}

static
int32_t
trace_ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s context=%d, callback={.func=%p, .user_data=%p, .flags=%d}\n",
               __func__+6, context, callback.func, callback.user_data, callback.flags);
    return ppb_graphics3d_swap_buffers(context, callback);
}
#endif // NDEBUG


const struct PPB_Graphics3D_1_0 ppb_graphics3d_interface_1_0 = {
    .GetAttribMaxValue =    TWRAP(ppb_graphics3d_get_attrib_max_value),
    .Create =               TWRAP(ppb_graphics3d_create),
    .IsGraphics3D =         TWRAP(ppb_graphics3d_is_graphics3d),
    .GetAttribs =           TWRAP(ppb_graphics3d_get_attribs),
    .SetAttribs =           TWRAP(ppb_graphics3d_set_attribs),
    .GetError =             TWRAP(ppb_graphics3d_get_error),
    .ResizeBuffers =        TWRAP(ppb_graphics3d_resize_buffers),
    .SwapBuffers =          TWRAP(ppb_graphics3d_swap_buffers),
};
