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
ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(PP_Resource context, GLint srcX0, GLint srcY0,
                                                    GLint srcX1, GLint srcY1, GLint dstX0,
                                                    GLint dstY0, GLint dstX1, GLint dstY1,
                                                    GLbitfield mask, GLenum filter)
{
}

void
trace_ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(PP_Resource context, GLint srcX0,
                                                          GLint srcY0, GLint srcX1, GLint srcY1,
                                                          GLint dstX0, GLint dstY0, GLint dstX1,
                                                          GLint dstY1, GLbitfield mask,
                                                          GLenum filter)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(context, srcX0, srcY0, srcX1, srcY1,
                                                               dstX0, dstY0, dstX1, dstY1, mask,
                                                               filter);
}


const struct PPB_OpenGLES2FramebufferBlit ppb_opengles2_framebuffer_blit_interface_1_0 = {
    .BlitFramebufferEXT = trace_ppb_opengles2_framebuffer_blit_blit_framebuffer_ext,
};
