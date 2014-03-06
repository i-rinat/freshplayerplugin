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

void
ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle(PP_Resource context, GLenum mode,
                                                           GLint first, GLsizei count,
                                                           GLsizei primcount)
{
}

void
ppb_opengles2_instanced_arrays_draw_elements_instanced_angle(PP_Resource context, GLenum mode,
                                                             GLsizei count, GLenum type,
                                                             const void *indices, GLsizei primcount)
{
}

void
ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(PP_Resource context, GLuint index,
                                                           GLuint divisor)
{
}


// trace wrappers
static
void
trace_ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle(PP_Resource context, GLenum mode,
                                                                 GLint first, GLsizei count,
                                                                 GLsizei primcount)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle(context, mode, first, count,
                                                               primcount);
}


static
void
trace_ppb_opengles2_instanced_arrays_draw_elements_instanced_angle(PP_Resource context, GLenum mode,
                                                                   GLsizei count, GLenum type,
                                                                   const void *indices,
                                                                   GLsizei primcount)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_instanced_arrays_draw_elements_instanced_angle(context, mode, count, type,
                                                                 indices, primcount);
}

static
void
trace_ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(PP_Resource context, GLuint index,
                                                                 GLuint divisor)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(context, index, divisor);
}



const struct PPB_OpenGLES2InstancedArrays ppb_opengles2_instanced_arrays_interface_1_0 = {
    .DrawArraysInstancedANGLE = trace_ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle,
    .DrawElementsInstancedANGLE =trace_ppb_opengles2_instanced_arrays_draw_elements_instanced_angle,
    .VertexAttribDivisorANGLE = trace_ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle,

};
