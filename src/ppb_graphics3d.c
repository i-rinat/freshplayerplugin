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

#include "pp_resource.h"
#include <assert.h>
#include "ppb_graphics3d.h"
#include <stdlib.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GLES2/gl2.h>
#include "trace.h"
#include "tables.h"
#include <ppapi/c/pp_errors.h>
#include "ppb_core.h"
#include "ppb_opengles2.h"
#include "config.h"


int32_t
ppb_graphics3d_get_attrib_max_value(PP_Resource instance, int32_t attribute, int32_t *value)
{
    return 0;
}

static
void
report_shader_compile_error(const char *fname, GLuint shader, const char *shader_body)
{
    char *log;
    GLint log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    log = malloc(log_length + 1);
    if (!log) {
        trace_error("%s, can't allocate memory\n", __func__);
        return;
    }
    glGetShaderInfoLog(shader, log_length + 1, NULL, log);
    trace_error("%s, compilation of shader\n%s\nfailed with error: %s\n", fname, shader_body, log);
    free(log);
}

// creates GL context for transparency rendering
static
int
create_presentation_glx_context(struct pp_graphics3d_s *g3d)
{
    int cfg_attrs[] = { GLX_ALPHA_SIZE,     8,
                        GLX_BLUE_SIZE,      8,
                        GLX_GREEN_SIZE,     8,
                        GLX_RED_SIZE,       8,
                        GLX_X_RENDERABLE,   True,
                        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT | GLX_PIXMAP_BIT,
                        None };
    int nconfigs = 0;
    int screen = DefaultScreen(display.x);
    GLXFBConfig *fb_cfgs = glXChooseFBConfig(display.x, screen, cfg_attrs, &nconfigs);
    if (!fb_cfgs) {
        trace_error("%s, glXChooseFBConfig returned NULL\n", __func__);
        goto err_1;
    }

    trace_info_f("%s, glXChooseFBConfig returned %d configs, choosing first one\n", __func__,
                 nconfigs);
    g3d->fb_config_t = fb_cfgs[0];
    XFree(fb_cfgs);

    // create context implementing OpenGL (no GL ES here)
    const int ctx_attrs[] = {
        GLX_RENDER_TYPE,                GLX_RGBA_TYPE,
        None,
    };
    g3d->glc_t = display.glXCreateContextAttribsARB(display.x, g3d->fb_config_t, g3d->glc, True,
                                                    ctx_attrs);
    if (!g3d->glc_t) {
        trace_error("%s, glXCreateContextAttribsARB returned NULL\n", __func__);
        goto err_1;
    }

    int ret = glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc_t);
    if (!ret) {
        trace_error("%s, glXMakeCurrent failed\n", __func__);
        goto err_2;
    }

    glGenTextures(1, &g3d->tex_back);
    glBindTexture(GL_TEXTURE_2D, g3d->tex_back);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Create alpha blending GL program
    const char *vert_shader_body =
        "#version 100\n"
        "attribute vec4 pos;\n"
        "varying vec2 tex_coord;\n"
        "void main() {\n"
        "    gl_Position = vec4(2.0 * pos.x - 1.0, 2.0 * pos.y - 1.0, 0.0, 1.0);\n"
        "    tex_coord = pos.xy;\n"
        "}\n";
    const char *frag_shader_body =
        "#version 100\n"
        "varying highp vec2 tex_coord;\n"
        "uniform sampler2D tex_front;\n"
        "uniform sampler2D tex_back;\n"
        "void main() {\n"
        "    highp vec4 c_front = texture2D(tex_front, tex_coord);\n"
        "    highp vec4 c_back = texture2D(tex_back, tex_coord);\n"
        "    gl_FragColor = mix(c_back, c_front, c_front.a);\n"
        "}\n";

    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    GLint status = 0;

    glShaderSource(vert_shader, 1, &vert_shader_body, NULL);
    glCompileShader(vert_shader);
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        report_shader_compile_error(__func__, vert_shader, vert_shader_body);
        goto err_3;
    }

    glShaderSource(frag_shader, 1, &frag_shader_body, NULL);
    glCompileShader(frag_shader);
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        report_shader_compile_error(__func__, frag_shader, frag_shader_body);
        goto err_3;
    }

    g3d->prog.id = glCreateProgram();
    glAttachShader(g3d->prog.id, vert_shader);
    glAttachShader(g3d->prog.id, frag_shader);
    g3d->prog.attrib_pos = 0;
    glBindAttribLocation(g3d->prog.id, g3d->prog.attrib_pos, "pos");

    glLinkProgram(g3d->prog.id);
    glGetProgramiv(g3d->prog.id, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        trace_error("%s, GL program link failed\n", __func__);
        goto err_4;
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);

    g3d->prog.uniform_tex_back = glGetUniformLocation(g3d->prog.id, "tex_back");
    g3d->prog.uniform_tex_front = glGetUniformLocation(g3d->prog.id, "tex_front");

    return 0;

err_4:
    glDeleteProgram(g3d->prog.id);
err_3:
    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);
err_2:
    glXDestroyContext(display.x, g3d->glc_t);
err_1:
    return 1;
}

PP_Resource
ppb_graphics3d_create(PP_Instance instance, PP_Resource share_context, const int32_t attrib_list[])
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }

    // pretend we have no 3d to avoid pixmap rendering issue for transparent instances
    if (pp_i->is_transparent)
        if (config.enable_3d_transparent == 0)
            return 0;

    // check for required GLX extensions
    if (!display.glx_arb_create_context || !display.glx_arb_create_context_profile ||
        !display.glx_ext_create_context_es2_profile)
    {
        trace_warning("%s, some of GLX_ARB_create_context, GLX_ARB_create_context_profile, "
                      "GLX_EXT_create_context_es2_profile missing\n", __func__);
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

    int *cfg_attrs = calloc(attrib_len + 3 * 2, sizeof(int));
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
        default:
            // skip unknown attribute
            trace_error("%s, unknown attribute 0x%x\n", __func__, attrib_list[k1]);
            k1 += 1;
            break;
        }
    }

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

    // create context implementing OpenGL ES 2.0
    const int ctx_attrs[] = {
        GLX_RENDER_TYPE,                GLX_RGBA_TYPE,
        GLX_CONTEXT_MAJOR_VERSION_ARB,  2,
        GLX_CONTEXT_MINOR_VERSION_ARB,  0,
        GLX_CONTEXT_PROFILE_MASK_ARB,   GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
        None,
    };
    // TODO: support shared_context
    g3d->glc = display.glXCreateContextAttribsARB(display.x, g3d->fb_config, NULL, True, ctx_attrs);
    if (!g3d->glc) {
        trace_error("%s, glXCreateContextAttribsARB returned NULL\n", __func__);
        goto err;
    }

    g3d->pixmap = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                DefaultDepth(display.x, DefaultScreen(display.x)));
    g3d->glx_pixmap = glXCreatePixmap(display.x, g3d->fb_config, g3d->pixmap, NULL);
    if (g3d->glx_pixmap == None) {
        trace_error("%s, failed to create GLX pixmap\n", __func__);
        goto err;
    }

    int ret = glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    if (!ret) {
        trace_error("%s, glXMakeCurrent failed\n", __func__);
        goto err;
    }

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (pp_i->is_transparent) {
        // create texture for plugin content
        glGenTextures(1, &g3d->tex_front);
        glBindTexture(GL_TEXTURE_2D, g3d->tex_front);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        if (create_presentation_glx_context(g3d) != 0) {
            trace_error("%s, can't create GL context for transparency processing\n", __func__);
            goto err;
        }
    }

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

void
ppb_graphics3d_destroy(void *p)
{
    struct pp_graphics3d_s *g3d = p;
    struct pp_instance_s *pp_i = g3d->instance;
    g_hash_table_destroy(g3d->sub_maps);

    pthread_mutex_lock(&display.lock);

    // bringing context to current thread releases it from any others
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    // free it here, to be able to destroy X Pixmap
    glXMakeCurrent(display.x, None, NULL);

    glXDestroyPixmap(display.x, g3d->glx_pixmap);
    XFreePixmap(display.x, g3d->pixmap);

    if (pp_i->is_transparent) {
        glDeleteTextures(1, &g3d->tex_back);
        glDeleteTextures(1, &g3d->tex_front);
        glDeleteProgram(g3d->prog.id);
        glXDestroyContext(display.x, g3d->glc_t);
    }

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
    Pixmap old_pixmap = g3d->pixmap;

    // release possibly bound to other thread g3d->glx_pixmap and bind it to the current one
    pthread_mutex_lock(&display.lock);
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);
    g3d->pixmap = XCreatePixmap(display.x, DefaultRootWindow(display.x), g3d->width, g3d->height,
                                DefaultDepth(display.x, DefaultScreen(display.x)));
    g3d->glx_pixmap = glXCreatePixmap(display.x, g3d->fb_config, g3d->pixmap, NULL);

    // make new g3d->glx_pixmap current to the current thread to allow releasing old_glx_pixmap
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc);

    // clear surface
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // destroy previous glx and x pixmaps
    glXDestroyPixmap(display.x, old_glx_pixmap);
    XFreePixmap(display.x, old_pixmap);

    pthread_mutex_unlock(&display.lock);
    pp_resource_release(context);
    return PP_OK;
}

static
void
call_invalidaterect_ptac(void *param)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(GPOINTER_TO_SIZE(param));
    if (!pp_i)
        return;
    NPRect npr = {.top = 0, .left = 0, .bottom = pp_i->height, .right = pp_i->width};

    npn.invalidaterect(pp_i->npp, &npr);
    npn.forceredraw(pp_i->npp);
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
    if (pp_i->is_transparent) {
        glBindTexture(GL_TEXTURE_2D, g3d->tex_front);
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, g3d->width, g3d->height, 0);
    }
    glFinish();  // ensure painting is done
    glXMakeCurrent(display.x, None, NULL);

    pp_resource_release(context);

    pp_i->graphics_ccb = callback;
    pp_i->graphics_in_progress = 1;
    if (!callback.func)
        pthread_barrier_init(&pp_i->graphics_barrier, NULL, 2);

    if (pp_i->is_fullscreen) {
        XGraphicsExposeEvent ev = {
            .type = GraphicsExpose,
            .drawable = pp_i->fs_wnd,
            .width =    pp_i->fs_width,
            .height =   pp_i->fs_height,
        };

        XSendEvent(display.x, pp_i->fs_wnd, True, ExposureMask, (void *)&ev);
        XFlush(display.x);
        pthread_mutex_unlock(&display.lock);
    } else {
        pthread_mutex_unlock(&display.lock);
        ppb_core_call_on_browser_thread(call_invalidaterect_ptac, GSIZE_TO_POINTER(pp_i->id));
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
