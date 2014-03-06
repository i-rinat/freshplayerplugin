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
#include <stddef.h>
#include "trace.h"


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
    return NULL;
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
    trace_info("[PPB] {zilch} %s\n", __func__);
    return ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(context, target, offset,
                                                                       size, access);
}

static
void
trace_ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(PP_Resource context,
                                                                    const void *mem)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
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
    trace_info("[PPB] {zilch} %s\n", __func__);
    return ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(context, target, level,
                                        xoffset, yoffset, width, height, format, type, access);
}

static
void
trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                                     const void *mem)
{
    trace_info("[PPB] {zilch} %s\n", __func__);
    ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(context, mem);
}


const struct PPB_OpenGLES2ChromiumMapSub ppb_opengles2_chromium_map_sub_interface_1_0 = {
    .MapBufferSubDataCHROMIUM =   trace_ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium,
    .UnmapBufferSubDataCHROMIUM = trace_ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium,
    .MapTexSubImage2DCHROMIUM =   trace_ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium,
    .UnmapTexSubImage2DCHROMIUM = trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium,
};
