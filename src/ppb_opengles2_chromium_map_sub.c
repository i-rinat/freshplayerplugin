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

#include "ppb_opengles2.h"
#include <stdlib.h>
#include <GL/gl.h>
#include "trace.h"
#include "pp_resource.h"


void *
ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(PP_Resource context, GLuint target,
                                                            GLintptr offset, GLsizeiptr size,
                                                            GLenum access)
{
    return NULL;
}

void
ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(PP_Resource context, const void *mem)
{
}

void *
ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(PP_Resource context, GLenum target,
                                                             GLint level, GLint xoffset,
                                                             GLint yoffset, GLsizei width,
                                                             GLsizei height, GLenum format,
                                                             GLenum type, GLenum access)
{
    if (target != GL_TEXTURE_2D || level != 0 || access != GL_WRITE_ONLY) {
        trace_warning("%s, wrong arguments\n", __func__);
        return NULL;
    }

    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_warning("%s, wrong context\n", __func__);
        return NULL;
    }

    const uintptr_t bytes_per_pixel = (GL_RGB == format) ? 3 : 4;
    void *res = malloc(width * height * bytes_per_pixel);
    g3d->sub_map_xoffset = xoffset;
    g3d->sub_map_yoffset = yoffset;
    g3d->sub_map_width = width;
    g3d->sub_map_height = height;
    pp_resource_release(context);

    return res;
}

void
ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                               const void *mem)
{
}

// trace wrappers
static
void *
trace_ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(PP_Resource context,
                                                                  GLuint target, GLintptr offset,
                                                                  GLsizeiptr size, GLenum access)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u, offset=%d, size=%u, access=%d\n",
               __func__+6, context, target, (int)offset, (unsigned int)size, access);
    return ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(context, target, offset,
                                                                       size, access);
}

static
void
trace_ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(PP_Resource context,
                                                                    const void *mem)
{
    trace_info("[PPB] {zilch} %s context=%d, mem=%p\n", __func__+6, context, mem);
    ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(context, mem);
}

static
void *
trace_ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(PP_Resource context,
                                                                   GLenum target, GLint level,
                                                                   GLint xoffset, GLint yoffset,
                                                                   GLsizei width, GLsizei height,
                                                                   GLenum format, GLenum type,
                                                                   GLenum access)
{
    trace_info("[PPB] {full} %s context=%d, target=%d, level=%d, xoffset=%d, yoffset=%d, "
               "width=%u, height=%u, format=%d, type=%d, access=%d\n", __func__+6, context, target,
               level, xoffset, yoffset, width, height, format, type, access);
    return ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(context, target, level,
                                        xoffset, yoffset, width, height, format, type, access);
}

static
void
trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                                     const void *mem)
{
    trace_info("[PPB] {zilch} %s context=%d, mem=%p\n", __func__+6, context, mem);
    ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(context, mem);
}


const struct PPB_OpenGLES2ChromiumMapSub ppb_opengles2_chromium_map_sub_interface_1_0 = {
    .MapBufferSubDataCHROMIUM =   trace_ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium,
    .UnmapBufferSubDataCHROMIUM = trace_ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium,
    .MapTexSubImage2DCHROMIUM =   trace_ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium,
    .UnmapTexSubImage2DCHROMIUM = trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium,
};
