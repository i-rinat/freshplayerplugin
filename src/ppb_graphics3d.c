/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "compat_glx_defines.h"
#include "pp_interface.h"
#include "pp_resource.h"
#include "ppb_core.h"
#include "ppb_graphics3d.h"
#include "ppb_instance.h"
#include "ppb_message_loop.h"
#include "reverse_constant.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include "trace_helpers.h"
#include <GL/glx.h>
#include <GLES2/gl2.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <ppapi/c/pp_errors.h>
#include <pthread.h>
#include <stdlib.h>

STATIC_ASSERT(sizeof(struct pp_graphics3d_s) <= LARGEST_RESOURCE_SIZE);

int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    return 0;
}

GLXContext
peek_gl_context(PP_Resource graphics3d)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(graphics3d, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad resource\n", __func__);
        return NULL;
    }

    GLXContext glc = g3d->glc;
    pp_resource_release(graphics3d);
    return glc;
}

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[])
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    GLXContext share_glc = (share_context == 0) ? NULL
                                                : peek_gl_context(share_context);
    // check for required GLX extensions
#if HAVE_GLES2
    if (!display.glx_arb_create_context || !display.glx_arb_create_context_profile ||
        !display.glx_ext_create_context_es2_profile)
    {
        trace_warning("%s, some of GLX_ARB_create_context, GLX_ARB_create_context_profile, "
                      "GLX_EXT_create_context_es2_profile missing\n", __func__);
        return 0;
    }

    if (!display.glXCreateContextAttribsARB) {
        trace_warning("%s, no glXCreateContextAttribsARB found\n", __func__);
        return 0;
    }
#endif

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

    const int max_attrib_count = attrib_len + 3 * 2;
    int *cfg_attrs = calloc(max_attrib_count, sizeof(int));
    int k2 = 0;
    cfg_attrs[k2++] = GLX_X_RENDERABLE;
    cfg_attrs[k2++] = True;
    cfg_attrs[k2++] = GLX_DRAWABLE_TYPE;
    cfg_attrs[k2++] = GLX_WINDOW_BIT | GLX_PIXMAP_BIT;

    int done = 0;
    int k1 = 0;
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
            cfg_attrs[k2++] = None;
            done = 1;
            break;
        case PP_GRAPHICS3DATTRIB_ALPHA_SIZE:
            cfg_attrs[k2++] = GLX_ALPHA_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_BLUE_SIZE:
            cfg_attrs[k2++] = GLX_BLUE_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_GREEN_SIZE:
            cfg_attrs[k2++] = GLX_GREEN_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_RED_SIZE:
            cfg_attrs[k2++] = GLX_RED_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_DEPTH_SIZE:
            cfg_attrs[k2++] = GLX_DEPTH_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_STENCIL_SIZE:
            cfg_attrs[k2++] = GLX_STENCIL_SIZE;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLES:
            cfg_attrs[k2++] = GLX_SAMPLES;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS:
            cfg_attrs[k2++] = GLX_SAMPLE_BUFFERS;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case GLX_Y_INVERTED_EXT:
            cfg_attrs[k2++] = GLX_Y_INVERTED_EXT;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        case GLX_BIND_TO_TEXTURE_RGBA_EXT:
            cfg_attrs[k2++] = GLX_BIND_TO_TEXTURE_RGBA_EXT;
            cfg_attrs[k2++] = attrib_list[k1 + 1];
            k1 += 2;
            break;
        default:
            // skip unknown attribute
            trace_error("%s, unknown attribute 0x%x\n", __func__, attrib_list[k1]);
            k1 += 1;
            break;
        }
    }

    assert(k2 <= max_attrib_count);

    pthread_mutex_lock(&display.lock);
    int screen = DefaultScreen(display.x);
    int nconfigs = 0;
    GLXFBConfig *fb_cfgs = glXChooseFBConfig(display.x, screen, cfg_attrs, &nconfigs);
    free(cfg_attrs);

    if (!fb_cfgs) {
        trace_error("%s, glXChooseFBConfig returned NULL\n", __func__);
        goto err;
    }

    trace_info_f("%s, glXChooseFBConfig returned %d configs, choosing first one\n", __func__,
                 nconfigs);
    g3d->fb_config = fb_cfgs[0];
    XFree(fb_cfgs);

#if HAVE_GLES2
    // create context implementing OpenGL ES 2.0
    const int ctx_attrs[] = {
        GLX_RENDER_TYPE,                GLX_RGBA_TYPE,
        GLX_CONTEXT_MAJOR_VERSION_ARB,  2,
        GLX_CONTEXT_MINOR_VERSION_ARB,  0,
        GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        None,
    };
#else
    // create context implementing OpenGL 2.0
    // OpenGL ES 2.0 will be emulated with help of shader translator
    const int ctx_attrs[] = {
        GLX_RENDER_TYPE,                GLX_RGBA_TYPE,
        GLX_CONTEXT_MAJOR_VERSION_ARB,  2,
        GLX_CONTEXT_MINOR_VERSION_ARB,  0,
        None,
    };
#endif

    if (display.glXCreateContextAttribsARB) {
        g3d->glc = display.glXCreateContextAttribsARB(display.x, g3d->fb_config, share_glc, True,
                                                      ctx_attrs);
        if (!g3d->glc)
            trace_warning("%s, glXCreateContextAttribsARB returned NULL\n", __func__);

    } else {
        g3d->glc = NULL;
    }

    if (!g3d->glc) {
        // if glXCreateContextAttribsARB is not present or returned NULL,
        // request any GL context
        g3d->glc = glXCreateNewContext(display.x, g3d->fb_config, GLX_RGBA_TYPE, share_glc, True);
        if (!g3d->glc) {
            trace_error("%s, glXCreateNewContext returned NULL\n", __func__);
            goto err;
        }
    }

    g3d->depth = pp_i->is_transparent ? 32 : DefaultDepth(display.x, screen);
    switch (g3d->depth) {
    case 24:
        g3d->xr_pictfmt = display.pictfmt_rgb24;
        break;
    case 32:
        g3d->xr_pictfmt = display.pictfmt_argb32;
        break;
    default:
        trace_error("%s, unsupported g3d->depth (%d)\n", __func__, g3d->depth);
        goto err;
    }

    // Creating two X pixmaps. First one is for drawing into, with a GLX Pixmap associated.
    // Second one is for double buffering.
    g3d->pixmap[0] = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                   g3d->depth);
    g3d->pixmap[1] = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                   g3d->depth);
    g3d->glx_pixmap = glXCreatePixmap(display.x, g3d->fb_config, g3d->pixmap[0], NULL);
    if (g3d->glx_pixmap == None) {
        trace_error("%s, failed to create GLX pixmap\n", __func__);
        goto err;
    }

    XFlush(display.x);
    if (display.have_xrender) {
        g3d->xr_pict[0] = XRenderCreatePicture(display.x, g3d->pixmap[0], g3d->xr_pictfmt, 0, 0);
        g3d->xr_pict[1] = XRenderCreatePicture(display.x, g3d->pixmap[1], g3d->xr_pictfmt, 0, 0);
    }

    int ret = glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    if (!ret) {
        trace_error("%s, glXMakeCurrent failed\n", __func__);
        goto err;
    }

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glXMakeCurrent(display.x, None, NULL);

    g3d->sub_maps = g_hash_table_new(g_direct_hash, g_direct_equal);
    pthread_mutex_unlock(&display.lock);

    pp_resource_release(context);
    return context;
err:
    pthread_mutex_unlock(&display.lock);
    pp_resource_release(context);
    pp_resource_expunge(context);
    return 0;
}

static
void
ppb_graphics3d_destroy(void *p)
{
    struct pp_graphics3d_s *g3d = p;

    g_hash_table_destroy(g3d->sub_maps);
    pthread_mutex_lock(&display.lock);

    // bringing context to current thread releases it from any others
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    // free it here, to be able to destroy X Pixmap
    glXMakeCurrent(display.x, None, NULL);

    glXDestroyPixmap(display.x, g3d->glx_pixmap);

    if (display.have_xrender) {
        XRenderFreePicture(display.x, g3d->xr_pict[0]);
        XRenderFreePicture(display.x, g3d->xr_pict[1]);
    }

    XFreePixmap(display.x, g3d->pixmap[0]);
    XFreePixmap(display.x, g3d->pixmap[1]);

    glXDestroyContext(display.x, g3d->glc);
    pthread_mutex_unlock(&display.lock);
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
    if (width < 0 || height < 0) {
        trace_error("%s, width or height are negative\n", __func__);
        return PP_ERROR_BADARGUMENT;
    }

    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    g3d->width = width;
    g3d->height = height;

    GLXPixmap old_glx_pixmap = g3d->glx_pixmap;
    Pixmap    old_pixmap[2] = { g3d->pixmap[0], g3d->pixmap[1] };
    Picture   old_pict[2] = { g3d->xr_pict[0], g3d->xr_pict[1] };

    // release possibly bound to other thread g3d->glx_pixmap and bind it to the current one
    pthread_mutex_lock(&display.lock);
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    g3d->pixmap[0] = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                   g3d->depth);
    g3d->pixmap[1] = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                   g3d->depth);
    g3d->glx_pixmap = glXCreatePixmap(display.x, g3d->fb_config, g3d->pixmap[0], NULL);
    XFlush(display.x);
    if (display.have_xrender) {
        g3d->xr_pict[0] = XRenderCreatePicture(display.x, g3d->pixmap[0], g3d->xr_pictfmt, 0, 0);
        g3d->xr_pict[1] = XRenderCreatePicture(display.x, g3d->pixmap[1], g3d->xr_pictfmt, 0, 0);
    }

    // make new g3d->glx_pixmap current to the current thread to allow releasing old_glx_pixmap
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // destroy previous glx and x pixmaps
    glXDestroyPixmap(display.x, old_glx_pixmap);

    if (display.have_xrender) {
        XRenderFreePicture(display.x, old_pict[0]);
        XRenderFreePicture(display.x, old_pict[1]);
    }

    XFreePixmap(display.x, old_pixmap[0]);
    XFreePixmap(display.x, old_pixmap[1]);

    pthread_mutex_unlock(&display.lock);
    pp_resource_release(context);
    return PP_OK;
}

static
void
call_forceredraw_ptac(void *param)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(GPOINTER_TO_SIZE(param));
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return;
    }

    pthread_mutex_lock(&display.lock);
    if (pp_i->is_fullscreen || pp_i->windowed_mode) {
        XEvent ev = {
            .xgraphicsexpose = {
                .type =     GraphicsExpose,
                .drawable = pp_i->is_fullscreen ? pp_i->fs_wnd : pp_i->wnd,
                .width =    pp_i->is_fullscreen ? pp_i->fs_width : pp_i->width,
                .height =   pp_i->is_fullscreen ? pp_i->fs_height : pp_i->height,
            }
        };

        XSendEvent(display.x, ev.xgraphicsexpose.drawable, True, ExposureMask, &ev);
        XFlush(display.x);
        pthread_mutex_unlock(&display.lock);
    } else {
        pthread_mutex_unlock(&display.lock);
        NPRect npr = {.top = 0, .left = 0, .bottom = pp_i->height, .right = pp_i->width};
        npn.invalidaterect(pp_i->npp, &npr);
        npn.forceredraw(pp_i->npp);
    }
}

int32_t
ppb_graphics3d_swap_buffers(PP_Resource context, struct PP_CompletionCallback callback)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct pp_instance_s *pp_i = g3d->instance;

    pthread_mutex_lock(&display.lock);
    if (pp_i->graphics != context) {
        // Other context bound, do nothing.
        pp_resource_release(context);
        pthread_mutex_unlock(&display.lock);
        return PP_ERROR_FAILED;
    }

    if (pp_i->graphics_in_progress) {
        pp_resource_release(context);
        pthread_mutex_unlock(&display.lock);
        return PP_ERROR_INPROGRESS;
    }

    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    glFinish();  // ensure painting is done
    glXMakeCurrent(display.x, None, NULL);

    // a round-trip to an X server is required here to be sure that drawing is completed
    XSync(display.x, False);

    // copy from pixmap[0] to pixmap[1]
    if (display.have_xrender) {
        XRenderComposite(display.x, PictOpSrc, g3d->xr_pict[0], None, g3d->xr_pict[1],
                         0, 0, 0, 0, 0, 0, g3d->width, g3d->height);

    } else {
        const int screen = DefaultScreen(display.x);
        const GC gc = DefaultGC(display.x, screen);
        XCopyArea(display.x, g3d->pixmap[0], g3d->pixmap[1], gc, 0, 0, g3d->width, g3d->height,
                  0, 0);
    }

    // a round-trip to an X server is required here to ensure that copying is completed, and further
    // GL drawing in this thread into pixmap[0] will not affect pixmap[1] which will be used in
    // another, browser thread
    XSync(display.x, False);

    pp_resource_release(context);

    pp_i->graphics_ccb = callback;
    pp_i->graphics_ccb_ml = ppb_message_loop_get_current();
    pp_i->graphics_in_progress = 1;
    pthread_mutex_unlock(&display.lock);

    ppb_core_call_on_browser_thread(pp_i->id, call_forceredraw_ptac, GSIZE_TO_POINTER(pp_i->id));

    if (callback.func)
        return PP_OK_COMPLETIONPENDING;

    trace_error("%s, callback.func==NULL branch not implemented\n", __func__);
    return PP_OK;
}


// trace wrappers
TRACE_WRAPPER
int32_t
trace_ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    trace_info("[PPB] {zilch} %s instance=%d, attribute=%s(0x%04x)\n", __func__+6, instance,
               reverse_graphics3d_attribute(attribute), attribute);
    return ppb_graphics3d_get_attrib_max_value(instance, attribute, value);
}

TRACE_WRAPPER
PP_Resource
trace_ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context,
                            const int32_t attrib_list[])
{
    gchar *s_attrib_list = trace_graphics3d_attributes_as_string(attrib_list);
    trace_info("[PPB] {full} %s intance=%d, share_context=%d, attrib_list=%s\n", __func__+6,
               instance, share_context, s_attrib_list);
    g_free(s_attrib_list);
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
    gchar *s_attrib_list = trace_graphics3d_attributes_as_string(attrib_list);
    trace_info("[PPB] {zilch} %s context=%d, attrib_list=%s\n", __func__+6, context, s_attrib_list);
    g_free(s_attrib_list);
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

static
void
__attribute__((constructor))
constructor_ppb_graphics3d(void)
{
    register_interface(PPB_GRAPHICS_3D_INTERFACE_1_0, &ppb_graphics3d_interface_1_0);
    register_resource(PP_RESOURCE_GRAPHICS3D, ppb_graphics3d_destroy);
}
