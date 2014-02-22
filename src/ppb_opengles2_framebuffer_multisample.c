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

#include <ppapi/c/ppb_opengles2.h>
#include <stddef.h>
#include "trace.h"


void
ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext
                    (PP_Resource context, GLenum target, GLsizei samples, GLenum internalformat,
                     GLsizei width, GLsizei height)
{
}

static
void
trace_ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext
                    (PP_Resource context, GLenum target, GLsizei samples, GLenum internalformat,
                     GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext(context, target,
                                                        samples, internalformat, width, height);
}


const struct PPB_OpenGLES2FramebufferMultisample ppb_opengles2_framebuffer_multisample_interface_1_0 = {
    .RenderbufferStorageMultisampleEXT =
                trace_ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext,
};
