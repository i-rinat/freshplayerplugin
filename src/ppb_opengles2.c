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
#include "pp_resource.h"
#include "reverse_constant.h"


static
void
_setup_glx_ctx(struct pp_graphics3d_s *g3d)
{
    glXMakeContextCurrent(g3d->dpy, g3d->glx_pixmap, g3d->glx_pixmap, g3d->glc);
}

static
void
setup_ctx(PP_Resource context)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    glXMakeContextCurrent(g3d->dpy, g3d->glx_pixmap, g3d->glx_pixmap, g3d->glc);
    pp_resource_release(context);
}

void
ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
    setup_ctx(context);
    glActiveTexture(texture);
}

void
ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
    setup_ctx(context);
    glAttachShader(program, shader);
}

void
ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char *name)
{
    setup_ctx(context);
    glBindAttribLocation(program, index, name);
}

void
ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
    setup_ctx(context);
    glBindBuffer(target, buffer);
}

void
ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
    setup_ctx(context);
    glBindFramebuffer(target, framebuffer);
}

void
ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
    setup_ctx(context);
    glBindRenderbuffer(target, renderbuffer);
}

void
ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
    setup_ctx(context);
    glBindTexture(target, texture);
}

void
ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    setup_ctx(context);
    glBlendColor(red, green, blue, alpha);
}

void
ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
    setup_ctx(context);
    glBlendEquation(mode);
}

void
ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
    setup_ctx(context);
    glBlendEquationSeparate(modeRGB, modeAlpha);
}

void
ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
    setup_ctx(context);
    glBlendFunc(sfactor, dfactor);
}

void
ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha)
{
    setup_ctx(context);
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
}

void
ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void *data,
                         GLenum usage)
{
    setup_ctx(context);
    glBufferData(target, size, data, usage);
}

void
ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void *data)
{
    setup_ctx(context);
    glBufferSubData(target, offset, size, data);
}

GLenum
ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    setup_ctx(context);
    return glCheckFramebufferStatus(target);
}

void
ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
    setup_ctx(context);
    glClear(mask);
}

void
ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    setup_ctx(context);
    glClearColor(red, green, blue, alpha);
}

void
ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
    setup_ctx(context);
    glClearDepthf(depth);
}

void
ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
    setup_ctx(context);
    glClearStencil(s);
}

void
ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha)
{
    setup_ctx(context);
    glColorMask(red, green, blue, alpha);
}

void
ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
    setup_ctx(context);
    glCompileShader(shader);
}

void
ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void *data)
{
    setup_ctx(context);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

void
ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void *data)
{
    setup_ctx(context);
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize,
                              data);
}

void
ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    setup_ctx(context);
    glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

void
ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    setup_ctx(context);
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

GLuint
ppb_opengles2_CreateProgram(PP_Resource context)
{
    setup_ctx(context);
    return glCreateProgram();
}

GLuint
ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    setup_ctx(context);
    return glCreateShader(type);
}

void
ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
    setup_ctx(context);
    glCullFace(mode);
}

void
ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint *buffers)
{
    setup_ctx(context);
    glDeleteBuffers(n, buffers);
}

void
ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint *framebuffers)
{
    setup_ctx(context);
    glDeleteFramebuffers(n, framebuffers);
}

void
ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
    setup_ctx(context);
    glDeleteProgram(program);
}

void
ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint *renderbuffers)
{
    setup_ctx(context);
    glDeleteRenderbuffers(n, renderbuffers);
}

void
ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
    setup_ctx(context);
    glDeleteShader(shader);
}

void
ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint *textures)
{
    setup_ctx(context);
    glDeleteTextures(n, textures);
}

void
ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
    setup_ctx(context);
    glDepthFunc(func);
}

void
ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
    setup_ctx(context);
    glDepthMask(flag);
}

void
ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
    setup_ctx(context);
    glDepthRangef(zNear, zFar);
}

void
ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
    setup_ctx(context);
    glDetachShader(program, shader);
}

void
ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
    setup_ctx(context);
    glDisable(cap);
}

void
ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
    setup_ctx(context);
    glDisableVertexAttribArray(index);
}

void
ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
    setup_ctx(context);
    glDrawArrays(mode, first, count);
}

void
ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void *indices)
{
    setup_ctx(context);
    glDrawElements(mode, count, type, indices);
}

void
ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
    setup_ctx(context);
    glEnable(cap);
}

void
ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
    setup_ctx(context);
    glEnableVertexAttribArray(index);
}

void
ppb_opengles2_Finish(PP_Resource context)
{
    setup_ctx(context);
    glFinish();
}

void
ppb_opengles2_Flush(PP_Resource context)
{
    setup_ctx(context);
    glFlush();
}

void
ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer)
{
    setup_ctx(context);
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
}

void
ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level)
{
    setup_ctx(context);
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
}

void
ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
    setup_ctx(context);
    glFrontFace(mode);
}

void
ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint *buffers)
{
    setup_ctx(context);
    glGenBuffers(n, buffers);
}

void
ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
    setup_ctx(context);
    glGenerateMipmap(target);
}

void
ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint *framebuffers)
{
    setup_ctx(context);
    glGenFramebuffers(n, framebuffers);
}

void
ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint *renderbuffers)
{
    setup_ctx(context);
    glGenRenderbuffers(n, renderbuffers);
}

void
ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint *textures)
{
    setup_ctx(context);
    glGenTextures(n, textures);
}

void
ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei *length, GLint *size, GLenum *type, char *name)
{
    setup_ctx(context);
    glGetActiveAttrib(program, index, bufsize, length, size, type, name);
}

void
ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei *length, GLint *size, GLenum *type, char *name)
{
    setup_ctx(context);
    glGetActiveUniform(program, index, bufsize, length, size, type, name);
}

void
ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei *count, GLuint *shaders)
{
    setup_ctx(context);
    glGetAttachedShaders(program, maxcount, count, shaders);
}

GLint
ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char *name)
{
    setup_ctx(context);
    return glGetAttribLocation(program, name);
}

void
ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean *params)
{
    setup_ctx(context);
    glGetBooleanv(pname, params);
}

void
ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetBufferParameteriv(target, pname, params);
}

GLenum
ppb_opengles2_GetError(PP_Resource context)
{
    setup_ctx(context);
    return glGetError();
}

void
ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat *params)
{
    setup_ctx(context);
    glGetFloatv(pname, params);
}

void
ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
}

void
ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetIntegerv(pname, params);
}

void
ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetProgramiv(program, pname, params);
}

void
ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei *length, char *infolog)
{
    setup_ctx(context);
    glGetProgramInfoLog(program, bufsize, length, infolog);
}

void
ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint *params)
{
    setup_ctx(context);
    glGetRenderbufferParameteriv(target, pname, params);
}

void
ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetShaderiv(shader, pname, params);
}

void
ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                               char *infolog)
{
    setup_ctx(context);
    glGetShaderInfoLog(shader, bufsize, length, infolog);
}

void
ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint *range, GLint *precision)
{
    setup_ctx(context);
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
}

void
ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                              char *source)
{
    setup_ctx(context);
    glGetShaderSource(shader, bufsize, length, source);
}

const GLubyte *
ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    setup_ctx(context);
    return glGetString(name);
}

void
ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat *params)
{
    setup_ctx(context);
    glGetTexParameterfv(target, pname, params);
}

void
ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetTexParameteriv(target, pname, params);
}

void
ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat *params)
{
    setup_ctx(context);
    glGetUniformfv(program, location, params);
}

void
ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint *params)
{
    setup_ctx(context);
    glGetUniformiv(program, location, params);
}

GLint
ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char *name)
{
    setup_ctx(context);
    return glGetUniformLocation(program, name);
}

void
ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat *params)
{
    setup_ctx(context);
    glGetVertexAttribfv(index, pname, params);
}

void
ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint *params)
{
    setup_ctx(context);
    glGetVertexAttribiv(index, pname, params);
}

void
ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void **pointer)
{
    setup_ctx(context);
    glGetVertexAttribPointerv(index, pname, pointer);
}

void
ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
    setup_ctx(context);
    glHint(target, mode);
}

GLboolean
ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    setup_ctx(context);
    return glIsBuffer(buffer);
}

GLboolean
ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    setup_ctx(context);
    return glIsEnabled(cap);
}

GLboolean
ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    setup_ctx(context);
    return glIsFramebuffer(framebuffer);
}

GLboolean
ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    setup_ctx(context);
    return glIsProgram(program);
}

GLboolean
ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    setup_ctx(context);
    return glIsRenderbuffer(renderbuffer);
}

GLboolean
ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    setup_ctx(context);
    return glIsShader(shader);
}

GLboolean
ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    setup_ctx(context);
    return glIsTexture(texture);
}

void
ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
    setup_ctx(context);
    glLineWidth(width);
}

void
ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
    setup_ctx(context);
    glLinkProgram(program);
}

void
ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
    setup_ctx(context);
    glPixelStorei(pname, param);
}

void
ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
    setup_ctx(context);
    glPolygonOffset(factor, units);
}

void
ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void *pixels)
{
    setup_ctx(context);
    glReadPixels(x, y, width, height, format, type, pixels);
}

void
ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
    setup_ctx(context);
    glReleaseShaderCompiler();
}

void
ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height)
{
    setup_ctx(context);
    glRenderbufferStorage(target, internalformat, width, height);
}

void
ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
    setup_ctx(context);
    glSampleCoverage(value, invert);
}

void
ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    setup_ctx(context);
    glScissor(x, y, width, height);
}

void
ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint *shaders,
                           GLenum binaryformat, const void *binary, GLsizei length)
{
    setup_ctx(context);
    glShaderBinary(n, shaders, binaryformat, binary, length);
}

void
ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char **str,
                           const GLint *length)
{
    setup_ctx(context);
    const char *v100 = "#version 100\n";
    const unsigned int v100_len = strlen(v100);

    // prepend "#version 100" line to all shaders
    // This enables shader compatibility mode introduced by GL_ARB_ES2_compatibility
    char **str2 = malloc(count * sizeof(char *));
    for (intptr_t k = 0; k < count; k ++) {
        size_t shader_len = strlen(str[k]);
        str2[k] = malloc(shader_len + 1 + v100_len);
        memcpy(str2[k], v100, v100_len); // first line
        memcpy(str2[k] + v100_len, str[k], shader_len + 1); // shader source with trailing \0
    }

    glShaderSource(shader, count, (const char **)str2, length);

    for (intptr_t k = 0; k < count; k ++)
        free(str2[k]);
    free(str2);
}

void
ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
    setup_ctx(context);
    glStencilFunc(func, ref, mask);
}

void
ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask)
{
    setup_ctx(context);
    glStencilFuncSeparate(face, func, ref, mask);
}

void
ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
    setup_ctx(context);
    glStencilMask(mask);
}

void
ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
    setup_ctx(context);
    glStencilMaskSeparate(face, mask);
}

void
ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
    setup_ctx(context);
    glStencilOp(fail, zfail, zpass);
}

void
ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass)
{
    setup_ctx(context);
    glStencilOpSeparate(face, fail, zfail, zpass);
}

void
ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void *pixels)
{
    setup_ctx(context);
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
}

void
ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
    setup_ctx(context);
    glTexParameterf(target, pname, param);
}

void
ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat *params)
{
    setup_ctx(context);
    glTexParameterfv(target, pname, params);
}

void
ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
    setup_ctx(context);
    glTexParameteri(target, pname, param);
}

void
ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint *params)
{
    setup_ctx(context);
    glTexParameteriv(target, pname, params);
}

void
ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void *pixels)
{
    setup_ctx(context);
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

void
ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
    setup_ctx(context);
    glUniform1f(location, x);
}

void
ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    setup_ctx(context);
    glUniform1fv(location, count, v);
}

void
ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
    setup_ctx(context);
    glUniform1i(location, x);
}

void
ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    setup_ctx(context);
    glUniform1iv(location, count, v);
}

void
ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
    setup_ctx(context);
    glUniform2f(location, x, y);
}

void
ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    setup_ctx(context);
    glUniform2fv(location, count, v);
}

void
ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
    setup_ctx(context);
    glUniform2i(location, x, y);
}

void
ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    setup_ctx(context);
    glUniform2iv(location, count, v);
}

void
ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    setup_ctx(context);
    glUniform3f(location, x, y, z);
}

void
ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    setup_ctx(context);
    glUniform3fv(location, count, v);
}

void
ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
    setup_ctx(context);
    glUniform3i(location, x, y, z);
}

void
ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    setup_ctx(context);
    glUniform3iv(location, count, v);
}

void
ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w)
{
    setup_ctx(context);
    glUniform4f(location, x, y, z, w);
}

void
ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    setup_ctx(context);
    glUniform4fv(location, count, v);
}

void
ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w)
{
    setup_ctx(context);
    glUniform4i(location, x, y, z, w);
}

void
ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    setup_ctx(context);
    glUniform4iv(location, count, v);
}

void
ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    setup_ctx(context);
    glUniformMatrix2fv(location, count, transpose, value);
}

void
ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    setup_ctx(context);
    glUniformMatrix3fv(location, count, transpose, value);
}

void
ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    setup_ctx(context);
    glUniformMatrix4fv(location, count, transpose, value);
}

void
ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
    setup_ctx(context);
    glUseProgram(program);
}

void
ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
    setup_ctx(context);
    glValidateProgram(program);
}

void
ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
    setup_ctx(context);
    glVertexAttrib1f(indx, x);
}

void
ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    setup_ctx(context);
    glVertexAttrib1fv(indx, values);
}

void
ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
    setup_ctx(context);
    glVertexAttrib2f(indx, x, y);
}

void
ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    setup_ctx(context);
    glVertexAttrib2fv(indx, values);
}

void
ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
    setup_ctx(context);
    glVertexAttrib3f(indx, x, y, z);
}

void
ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    setup_ctx(context);
    glVertexAttrib3fv(indx, values);
}

void
ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w)
{
    setup_ctx(context);
    glVertexAttrib4f(indx, x, y, z, w);
}

void
ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    setup_ctx(context);
    glVertexAttrib4fv(indx, values);
}

void
ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void *ptr)
{
    setup_ctx(context);
    glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
}

void
ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    setup_ctx(context);
    glViewport(x, y, width, height);
}

GLboolean
ppb_opengles2_chromium_enable_feature_enable_feature_chromium(PP_Resource context,
                                                              const char *feature)
{
    return PP_FALSE;
}

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

struct tex_sub_mapping_param_s {
    GLint           level;
    GLint           xoffset;
    GLint           yoffset;
    GLsizei         width;
    GLsizei         height;
    GLenum          format;
    GLenum          type;
    GLenum          access;
};

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

    struct tex_sub_mapping_param_s *map_params = g_slice_alloc(sizeof(*map_params));
    map_params->level = level;
    map_params->xoffset = xoffset;
    map_params->yoffset = yoffset;
    map_params->width = width;
    map_params->height = height;
    map_params->format = format;
    map_params->type = type;
    map_params->access = access;

    int bytes_per_pixel = (GL_RGB == format) ? 3 : 4;
    void *res = malloc(width * height * bytes_per_pixel);

    g_hash_table_insert(g3d->sub_maps, res, map_params);

    pp_resource_release(context);
    return res;
}

void
ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                               const void *mem)
{
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_warning("%s, wrong context\n", __func__);
        return;
    }

    struct tex_sub_mapping_param_s *mp = g_hash_table_lookup(g3d->sub_maps, mem);
    if (!mp) {
        trace_error("%s, memory was not mapped\n", __func__);
        pp_resource_release(context);
        return;
    }

    g_hash_table_remove(g3d->sub_maps, mem);
    _setup_glx_ctx(g3d);
    glTexSubImage2D(GL_TEXTURE_2D, mp->level, mp->xoffset, mp->yoffset, mp->width, mp->height,
                    mp->format, mp->type, mem);
    g_slice_free(struct tex_sub_mapping_param_s, mp);

    pp_resource_release(context);
    free((void *)mem);
}

void
ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(PP_Resource context, GLint srcX0, GLint srcY0,
                                                    GLint srcX1, GLint srcY1, GLint dstX0,
                                                    GLint dstY0, GLint dstX1, GLint dstY1,
                                                    GLbitfield mask, GLenum filter)
{
}

void
ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext
                    (PP_Resource context, GLenum target, GLsizei samples, GLenum internalformat,
                     GLsizei width, GLsizei height)
{
}

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

void
ppb_opengles2_query_gen_queries_ext(PP_Resource context, GLsizei n, GLuint *queries)
{
}

void
ppb_opengles2_query_delete_queries_ext(PP_Resource context, GLsizei n, const GLuint *queries)
{
}

GLboolean
ppb_opengles2_query_is_query_ext(PP_Resource context, GLuint id)
{
    return PP_TRUE;
}

void
ppb_opengles2_query_begin_query_ext(PP_Resource context, GLenum target, GLuint id)
{
}

void
ppb_opengles2_query_end_query_ext(PP_Resource context, GLenum target)
{
}

void
ppb_opengles2_query_get_queryiv_ext(PP_Resource context, GLenum target, GLenum pname, GLint *params)
{
}

void
ppb_opengles2_query_get_query_objectuiv_ext(PP_Resource context, GLuint id, GLenum pname,
                                            GLuint *params)
{
}


#ifndef NDEBUG
// trace wrappers
static
void
trace_ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
    trace_info("[PPB] {full} %s context=%d, texture=%u(%s)\n", __func__+6, context,
               texture, reverse_gl_enum(texture));
    ppb_opengles2_ActiveTexture(context, texture);
}

static
void
trace_ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, program=%d, shader=%d\n", __func__+6, context,
               program, shader);
    ppb_opengles2_AttachShader(context, program, shader);
}

static
void
trace_ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                       const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%d, index=%d, name=%s\n", __func__+6,
               context, program, index, name);
    ppb_opengles2_BindAttribLocation(context, program, index, name);
}

static
void
trace_ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), buffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), buffer);
    ppb_opengles2_BindBuffer(context, target, buffer);
}

static
void
trace_ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), framebuffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), framebuffer);
    ppb_opengles2_BindFramebuffer(context, target, framebuffer);
}

static
void
trace_ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), renderbuffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), renderbuffer);
    ppb_opengles2_BindRenderbuffer(context, target, renderbuffer);
}

static
void
trace_ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), texture=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), texture);
    ppb_opengles2_BindTexture(context, target, texture);
}

static
void
trace_ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                               GLclampf alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%f, green=%f, blue=%f, alpha=%f\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_BlendColor(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context, mode,
               reverse_gl_enum(mode));
    ppb_opengles2_BlendEquation(context, mode);
}

static
void
trace_ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
    trace_info("[PPB] {full} %s context=%d, modeRGB=%u(%s), modeAlpha=%u(%s)\n", __func__+6,
               context, modeRGB, reverse_gl_enum(modeRGB), modeAlpha, reverse_gl_enum(modeAlpha));
    ppb_opengles2_BlendEquationSeparate(context, modeRGB, modeAlpha);
}

static
void
trace_ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
    trace_info("[PPB] {full} %s context=%d, sfactor=%u(%s), dfactor=%u(%s)\n", __func__+6,
               context, sfactor, reverse_gl_enum(sfactor), dfactor, reverse_gl_enum(dfactor));
    ppb_opengles2_BlendFunc(context, sfactor, dfactor);
}

static
void
trace_ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB,
                                      GLenum srcAlpha, GLenum dstAlpha)
{
    trace_info("[PPB] {full} %s context=%d, srcRGB=%u(%s), dstRGB=%u(%s), srcAlpha=%u(%s), "
               "dstAlpha=%u(%s)\n", __func__+6, context, srcRGB, reverse_gl_enum(srcRGB),
               dstRGB, reverse_gl_enum(dstRGB), srcAlpha, reverse_gl_enum(srcAlpha), dstAlpha,
               reverse_gl_enum(dstAlpha));
    ppb_opengles2_BlendFuncSeparate(context, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

static
void
trace_ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size,
                               const void *data, GLenum usage)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), size=%ld, data=%p, usage=%u(%s)\n",
               __func__+6, context, target, reverse_gl_enum(target), (long int)size, data,
               usage, reverse_gl_enum(usage));
    ppb_opengles2_BufferData(context, target, size, data, usage);
}

static
void
trace_ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset,
                                  GLsizeiptr size, const void *data)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), offset=%ld, size=%ld, data=%p\n",
               __func__+6, context, target, reverse_gl_enum(target), (long)offset, (long)size,
               data);
    ppb_opengles2_BufferSubData(context, target, offset, size, data);
}

static
GLenum
trace_ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    return ppb_opengles2_CheckFramebufferStatus(context, target);
}

static
void
trace_ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
    trace_info("[PPB] {full} %s context=%d, mask=0x%x\n", __func__+6, context, mask);
    ppb_opengles2_Clear(context, mask);
}

static
void
trace_ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                               GLclampf alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%f, green=%f, blue=%f, alpha=%f\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_ClearColor(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
    trace_info("[PPB] {full} %s context=%d, depth=%f\n", __func__+6, context, depth);
    ppb_opengles2_ClearDepthf(context, depth);
}

static
void
trace_ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
    trace_info("[PPB] {full} %s context=%d, s=%d\n", __func__+6, context, s);
    ppb_opengles2_ClearStencil(context, s);
}

static
void
trace_ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                              GLboolean alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%d, green=%d, blue=%d, alpha=%d\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_ColorMask(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%d\n", __func__+6, context, shader);
    ppb_opengles2_CompileShader(context, shader);
}

static
void
trace_ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                         GLenum internalformat, GLsizei width, GLsizei height,
                                         GLint border, GLsizei imageSize, const void *data)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, internalformat=%u(%s), "
               "width=%d, height=%d, border=%d, imageSize=%d, data=%p\n", __func__+6, context,
               target, reverse_gl_enum(target), level, internalformat,
               reverse_gl_enum(internalformat), width, height, border, imageSize, data);
    ppb_opengles2_CompressedTexImage2D(context, target, level, internalformat, width, height,
                                       border, imageSize, data);
}

static
void
trace_ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                            GLint xoffset, GLint yoffset, GLsizei width,
                                            GLsizei height, GLenum format, GLsizei imageSize,
                                            const void *data)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, xoffset=%d, yoffset=%d, "
               "width=%d, height=%d, format=%u(%s), imageSize=%d, data=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), level, xoffset, yoffset,
               width, height, format, reverse_gl_enum(format), imageSize, data);
    ppb_opengles2_CompressedTexSubImage2D(context, target, level, xoffset, yoffset, width, height,
                                          format, imageSize, data);
}

static
void
trace_ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLint x, GLint y, GLsizei width,
                                   GLsizei height, GLint border)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, internalformat=%u(%s), x=%d, "
               "y=%d, width=%d, height=%d, border=%d\n", __func__+6, context, target,
               reverse_gl_enum(target), level, internalformat, reverse_gl_enum(internalformat),
               x, y, width, height, border);
    ppb_opengles2_CopyTexImage2D(context, target, level, internalformat, x, y, width, height,
                                 border);
}

static
void
trace_ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width,
                                      GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, xoffset=%d, yoffset=%d, "
               "x=%d, y=%d, width=%d, height=%d\n", __func__+6, context, target,
               reverse_gl_enum(target), level, xoffset, yoffset, x, y, width, height);
    ppb_opengles2_CopyTexSubImage2D(context, target, level, xoffset, yoffset, x, y, width, height);
}

static
GLuint
trace_ppb_opengles2_CreateProgram(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_CreateProgram(context);
}

static
GLuint
trace_ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    trace_info("[PPB] {full} %s context=%d type=%u(%s)\n", __func__+6, context, type,
               reverse_gl_enum(type));
    return ppb_opengles2_CreateShader(context, type);
}

static
void
trace_ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context, mode,
               reverse_gl_enum(mode));
    ppb_opengles2_CullFace(context, mode);
}

static
void
trace_ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint *buffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, buffers=%p\n", __func__+6, context,
               n, buffers);
    ppb_opengles2_DeleteBuffers(context, n, buffers);
}

static
void
trace_ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint *framebuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, framebuffers=%p\n", __func__+6, context,
               n, framebuffers);
    ppb_opengles2_DeleteFramebuffers(context, n, framebuffers);
}

static
void
trace_ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_DeleteProgram(context, program);
}

static
void
trace_ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint *renderbuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, renderbuffers=%p\n", __func__+6, context,
               n, renderbuffers);
    ppb_opengles2_DeleteRenderbuffers(context, n, renderbuffers);
}

static
void
trace_ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u\n", __func__+6, context, shader);
    ppb_opengles2_DeleteShader(context, shader);
}

static
void
trace_ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint *textures)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, textures=%p\n", __func__+6, context, n, textures);
    ppb_opengles2_DeleteTextures(context, n, textures);
}

static
void
trace_ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
    trace_info("[PPB] {full} %s context=%d, func=%u(%s)\n", __func__+6, context, func,
               reverse_gl_enum(func));
    ppb_opengles2_DepthFunc(context, func);
}

static
void
trace_ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
    trace_info("[PPB] {full} %s context=%d, flag=%d\n", __func__+6, context, flag);
    ppb_opengles2_DepthMask(context, flag);
}

static
void
trace_ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
    trace_info("[PPB] {full} %s context=%d, zNear=%f, zFar=%f\n", __func__+6, context, zNear, zFar);
    ppb_opengles2_DepthRangef(context, zNear, zFar);
}

static
void
trace_ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, shader=%u\n", __func__+6, context,
               program, shader);
    ppb_opengles2_DetachShader(context, program, shader);
}

static
void
trace_ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    ppb_opengles2_Disable(context, cap);
}

static
void
trace_ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {full} %s context=%d, index=%u\n", __func__+6, context, index);
    ppb_opengles2_DisableVertexAttribArray(context, index);
}

static
void
trace_ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s), first=%d, count=%d\n", __func__+6,
               context, mode, reverse_gl_enum(mode), first, count);
    ppb_opengles2_DrawArrays(context, mode, first, count);
}

static
void
trace_ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                                 const void *indices)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s), count=%d, type=%u(%s), indices=%p\n",
               __func__+6, context, mode, reverse_gl_enum(mode), count, type,
               reverse_gl_enum(type), indices);
    ppb_opengles2_DrawElements(context, mode, count, type, indices);
}

static
void
trace_ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    ppb_opengles2_Enable(context, cap);
}

static
void
trace_ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {full} %s context=%d, index=%u\n", __func__+6, context, index);
    ppb_opengles2_EnableVertexAttribArray(context, index);
}

static
void
trace_ppb_opengles2_Finish(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Finish(context);
}

static
void
trace_ppb_opengles2_Flush(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Flush(context);
}

static
void
trace_ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                            GLenum renderbuffertarget, GLuint renderbuffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), attachment=%u(%s), renderbuffertarget="
               "%u(%s), renderbuffer=%u\n", __func__+6, context, target, reverse_gl_enum(target),
               attachment, reverse_gl_enum(attachment), renderbuffertarget,
               reverse_gl_enum(renderbuffertarget), renderbuffer);
    ppb_opengles2_FramebufferRenderbuffer(context, target, attachment, renderbuffertarget,
                                          renderbuffer);
}

static
void
trace_ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                         GLenum textarget, GLuint texture, GLint level)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), attachment=%u(%s), textarget=%u(%s), "
               "texture=%u, level=%d\n", __func__+6, context, target, reverse_gl_enum(target),
               attachment, reverse_gl_enum(attachment), textarget, reverse_gl_enum(textarget),
               texture, level);
    ppb_opengles2_FramebufferTexture2D(context, target, attachment, textarget, texture, level);
}

static
void
trace_ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context,
               mode, reverse_gl_enum(mode));
    ppb_opengles2_FrontFace(context, mode);
}

static
void
trace_ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint *buffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, buffers=%p\n", __func__+6, context, n, buffers);
    ppb_opengles2_GenBuffers(context, n, buffers);
}

static
void
trace_ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    ppb_opengles2_GenerateMipmap(context, target);
}

static
void
trace_ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint *framebuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, framebuffers=%p\n", __func__+6, context,
               n, framebuffers);
    ppb_opengles2_GenFramebuffers(context, n, framebuffers);
}

static
void
trace_ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint *renderbuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, renderbuffers=%p\n", __func__+6, context,
               n, renderbuffers);
    ppb_opengles2_GenRenderbuffers(context, n, renderbuffers);
}

static
void
trace_ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint *textures)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, textures=%p\n", __func__+6, context, n, textures);
    ppb_opengles2_GenTextures(context, n, textures);
}

static
void
trace_ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index,
                                    GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type,
                                    char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, index=%u, bufsize=%d\n", __func__+6,
               context, program, index, bufsize);
    ppb_opengles2_GetActiveAttrib(context, program, index, bufsize, length, size, type, name);
}

static
void
trace_ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index,
                                     GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type,
                                     char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, index=%u, bufsize=%d\n", __func__+6,
               context, program, index, bufsize);
    ppb_opengles2_GetActiveUniform(context, program, index, bufsize, length, size, type, name);
}

static
void
trace_ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                       GLsizei *count, GLuint *shaders)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, maxcount=%d\n", __func__+6, context,
               program, maxcount);
    ppb_opengles2_GetAttachedShaders(context, program, maxcount, count, shaders);
}

static
GLint
trace_ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, name=%s\n", __func__+6, context,
               program, name);
    return ppb_opengles2_GetAttribLocation(context, program, name);
}

static
void
trace_ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetBooleanv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetBufferParameteriv(context, target, pname, params);
}

static
GLenum
trace_ppb_opengles2_GetError(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetError(context);
}

static
void
trace_ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetFloatv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                        GLenum attachment, GLenum pname,
                                                        GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), attachment=%u(%s), pname=%u(%s)\n",
               __func__+6, context, target, reverse_gl_enum(target), attachment,
               reverse_gl_enum(attachment), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetFramebufferAttachmentParameteriv(context, target, attachment, pname, params);
}

static
void
trace_ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetIntegerv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, pname=%u(%s)\n", __func__+6, context,
               program, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetProgramiv(context, program, pname, params);
}

static
void
trace_ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                      GLsizei *length, char *infolog)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, bufsize=%d\n", __func__+6, context,
               program, bufsize);
    ppb_opengles2_GetProgramInfoLog(context, program, bufsize, length, infolog);
}

static
void
trace_ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                               GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetRenderbufferParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, pname=%u(%s)\n", __func__+6, context,
               shader, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetShaderiv(context, shader, pname, params);
}

static
void
trace_ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize,
                                     GLsizei *length, char *infolog)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, bufsize=%d\n", __func__+6, context,
               shader, bufsize);
    ppb_opengles2_GetShaderInfoLog(context, shader, bufsize, length, infolog);
}

static
void
trace_ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype,
                                             GLenum precisiontype, GLint *range, GLint *precision)
{
    trace_info("[PPB] {full} %s context=%d, shadertype=%u(%s), precisiontype=%u(%s)\n", __func__+6,
               context, shadertype, reverse_gl_enum(shadertype), precisiontype,
               reverse_gl_enum(precisiontype));
    ppb_opengles2_GetShaderPrecisionFormat(context, shadertype, precisiontype, range, precision);
}

static
void
trace_ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize,
                                    GLsizei *length, char *source)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, bufsize=%d\n", __func__+6, context,
               shader, bufsize);
    ppb_opengles2_GetShaderSource(context, shader, bufsize, length, source);
}

static
const GLubyte*
trace_ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    trace_info("[PPB] {full} %s context=%d name=%u(%s)\n", __func__+6, context,
               name, reverse_gl_enum(name));
    return ppb_opengles2_GetString(context, name);
}

static
void
trace_ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                                      GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetTexParameterfv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                      GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetTexParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location,
                                 GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, location=%d\n", __func__+6, context,
               program, location);
    ppb_opengles2_GetUniformfv(context, program, location, params);
}

static
void
trace_ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, location=%d\n", __func__+6, context,
               program, location);
    ppb_opengles2_GetUniformiv(context, program, location, params);
}

static
GLint
trace_ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, name=%s\n", __func__+6, context,
               program, name);
    return ppb_opengles2_GetUniformLocation(context, program, name);
}

static
void
trace_ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname,
                                      GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribfv(context, index, pname, params);
}

static
void
trace_ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname,
                                      GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribiv(context, index, pname, params);
}

static
void
trace_ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                            void **pointer)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribPointerv(context, index, pname, pointer);
}

static
void
trace_ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), mode=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), mode, reverse_gl_enum(mode));
    ppb_opengles2_Hint(context, target, mode);
}

static
GLboolean
trace_ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    trace_info("[PPB] {full} %s context=%d, buffer=%u\n", __func__+6, context, buffer);
    return ppb_opengles2_IsBuffer(context, buffer);
}

static
GLboolean
trace_ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    return ppb_opengles2_IsEnabled(context, cap);
}

static
GLboolean
trace_ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    trace_info("[PPB] {full} %s context=%d, framebuffer=%u\n", __func__+6, context, framebuffer);
    return ppb_opengles2_IsFramebuffer(context, framebuffer);
}

static
GLboolean
trace_ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    return ppb_opengles2_IsProgram(context, program);
}

static
GLboolean
trace_ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    trace_info("[PPB] {full} %s context=%d, renderbuffer=%u\n", __func__+6, context, renderbuffer);
    return ppb_opengles2_IsRenderbuffer(context, renderbuffer);
}

static
GLboolean
trace_ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u\n", __func__+6, context, shader);
    return ppb_opengles2_IsShader(context, shader);
}

static
GLboolean
trace_ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    trace_info("[PPB] {full} %s context=%d, texture=%u\n", __func__+6, context, texture);
    return ppb_opengles2_IsTexture(context, texture);
}

static
void
trace_ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
    trace_info("[PPB] {full} %s context=%d, width=%f\n", __func__+6, context, width);
    ppb_opengles2_LineWidth(context, width);
}

static
void
trace_ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_LinkProgram(context, program);
}

static
void
trace_ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s), param=%d\n", __func__+6, context,
               pname, reverse_gl_enum(pname), param);
    ppb_opengles2_PixelStorei(context, pname, param);
}

static
void
trace_ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
    trace_info("[PPB] {full} %s context=%d, factor=%f, units=%f\n", __func__+6, context,
               factor, units);
    ppb_opengles2_PolygonOffset(context, factor, units);
}

static
void
trace_ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                               GLenum format, GLenum type, void *pixels)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d, format=%u(%s), "
               "type=%u(%s), pixels=%p\n", __func__+6, context, x, y, width, height, format,
               reverse_gl_enum(format), type, reverse_gl_enum(type), pixels);
    ppb_opengles2_ReadPixels(context, x, y, width, height, format, type, pixels);
}

static
void
trace_ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ReleaseShaderCompiler(context);
}

static
void
trace_ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                        GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), internalformat=%u(%s), width=%d, "
               "height=%d\n", __func__+6, context, target, reverse_gl_enum(target),
               internalformat, reverse_gl_enum(internalformat), width, height);
    ppb_opengles2_RenderbufferStorage(context, target, internalformat, width, height);
}

static
void
trace_ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
    trace_info("[PPB] {full} %s context=%d, value=%f, invert=%d\n", __func__+6, context,
               value, invert);
    ppb_opengles2_SampleCoverage(context, value, invert);
}

static
void
trace_ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d\n", __func__+6,
               context, x, y, width, height);
    ppb_opengles2_Scissor(context, x, y, width, height);
}

static
void
trace_ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint *shaders,
                                 GLenum binaryformat, const void *binary, GLsizei length)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, shaders=%p, binaryformat=%u(%s), binary=%p, "
               "length=%d\n", __func__+6, context, n, shaders, binaryformat,
               reverse_gl_enum(binaryformat), binary, length);
    ppb_opengles2_ShaderBinary(context, n, shaders, binaryformat, binary, length);
}

static
void
trace_ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count,
                                 const char **str, const GLint *length)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, count=%d, str=%p, length=%p\n",
               __func__+6, context, shader, count, str, length);
    for (intptr_t k = 0; k < count; k++) {
        trace_info("             shader_source[%u] = \n%s\n", (unsigned)k, str[k]);
    }
    ppb_opengles2_ShaderSource(context, shader, count, str, length);
}

static
void
trace_ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, func=%u(%s), ref=%d, mask=0x%x\n", __func__+6, context,
               func, reverse_gl_enum(func), ref, mask);
    ppb_opengles2_StencilFunc(context, func, ref, mask);
}

static
void
trace_ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                        GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), func=%u(%s), ref=%d, mask=0x%x\n",
               __func__+6, context, face, reverse_gl_enum(face), func, reverse_gl_enum(func),
               ref, mask);
    ppb_opengles2_StencilFuncSeparate(context, face, func, ref, mask);
}

static
void
trace_ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, mask=0x%x\n", __func__+6, context, mask);
    ppb_opengles2_StencilMask(context, mask);
}

static
void
trace_ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), mask=0x%x\n", __func__+6, context,
               face, reverse_gl_enum(face), mask);
    ppb_opengles2_StencilMaskSeparate(context, face, mask);
}

static
void
trace_ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
    trace_info("[PPB] {full} %s context=%d, fail=%u(%s), zfail=%u(%s), zpass=%u(%s)\n", __func__+6,
               context, fail, reverse_gl_enum(fail), zfail, reverse_gl_enum(zfail),
               zpass, reverse_gl_enum(zpass));
    ppb_opengles2_StencilOp(context, fail, zfail, zpass);
}

static
void
trace_ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                      GLenum zpass)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), fail=%u(%s), zfail=%u(%s), zpass=%u(%s)\n",
               __func__+6, context, face, reverse_gl_enum(face), fail, reverse_gl_enum(fail),
               zfail, reverse_gl_enum(zfail), zpass, reverse_gl_enum(zpass));
    ppb_opengles2_StencilOpSeparate(context, face, fail, zfail, zpass);
}

static
void
trace_ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level,
                               GLint internalformat, GLsizei width, GLsizei height, GLint border,
                               GLenum format, GLenum type, const void *pixels)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, internalformat=%u(%s), "
               "width=%d, height=%d, border=%d, format=%u(%s), type=%u(%s), pixels=%p\n",
               __func__+6, context, target, reverse_gl_enum(target), level, internalformat,
               reverse_gl_enum(internalformat), width, height, border, format,
               reverse_gl_enum(format), type, reverse_gl_enum(format), pixels);
    ppb_opengles2_TexImage2D(context, target, level, internalformat, width, height, border, format,
                             type, pixels);
}

static
void
trace_ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), param=%f\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), param);
    ppb_opengles2_TexParameterf(context, target, pname, param);
}

static
void
trace_ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                                   const GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_TexParameterfv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), param=%d\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), param);
    ppb_opengles2_TexParameteri(context, target, pname, param);
}

static
void
trace_ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                   const GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_TexParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                  GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                                  GLenum type, const void *pixels)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, xoffset=%d, yoffset=%d, "
               "width=%d, height=%d, format=%u(%s), type=%u(%s), pixels=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), level, xoffset, yoffset,
               width, height, format, reverse_gl_enum(format), type, reverse_gl_enum(type), pixels);
    ppb_opengles2_TexSubImage2D(context, target, level, xoffset, yoffset, width, height, format,
                                type, pixels);
}

static
void
trace_ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f\n", __func__+6, context, location, x);
    ppb_opengles2_Uniform1f(context, location, x);
}

static
void
trace_ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform1fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d\n", __func__+6, context, location, x);
    ppb_opengles2_Uniform1i(context, location, x);
}

static
void
trace_ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform1iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f\n", __func__+6, context,
               location, x, y);
    ppb_opengles2_Uniform2f(context, location, x, y);
}

static
void
trace_ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform2fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d\n", __func__+6, context,
               location, x, y);
    ppb_opengles2_Uniform2i(context, location, x, y);
}

static
void
trace_ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform2iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f, z=%f\n", __func__+6, context,
               location, x, y, z);
    ppb_opengles2_Uniform3f(context, location, x, y, z);
}

static
void
trace_ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform3fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d, z=%d\n", __func__+6, context,
               location, x, y, z);
    ppb_opengles2_Uniform3i(context, location, x, y, z);
}

static
void
trace_ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform3iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                              GLfloat w)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f, z=%f, w=%f\n", __func__+6,
               context, location, x, y, z, w);
    ppb_opengles2_Uniform4f(context, location, x, y, z, w);
}

static
void
trace_ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform4fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z,
                              GLint w)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d, z=%d, w=%d\n", __func__+6,
               context, location, x, y, z, w);
    ppb_opengles2_Uniform4i(context, location, x, y, z, w);
}

static
void
trace_ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform4iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix2fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix3fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix4fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_UseProgram(context, program);
}

static
void
trace_ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_ValidateProgram(context, program);
}

static
void
trace_ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f\n", __func__+6, context, indx, x);
    ppb_opengles2_VertexAttrib1f(context, indx, x);
}

static
void
trace_ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib1fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f\n", __func__+6, context,
               indx, x, y);
    ppb_opengles2_VertexAttrib2f(context, indx, x, y);
}

static
void
trace_ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context, indx,
               values);
    ppb_opengles2_VertexAttrib2fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y,
                                   GLfloat z)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f, z=%f\n", __func__+6, context,
               indx, x, y, z);
    ppb_opengles2_VertexAttrib3f(context, indx, x, y, z);
}

static
void
trace_ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib3fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y,
                                   GLfloat z, GLfloat w)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f, z=%f, w=%f\n", __func__+6,
               context, indx, x, y, z, w);
    ppb_opengles2_VertexAttrib4f(context, indx, x, y, z, w);
}

static
void
trace_ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib4fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                        GLboolean normalized, GLsizei stride, const void *ptr)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, size=%d, type=%u(%s), normalized=%d, "
               "stride=%d, ptr=%p\n", __func__+6, context, indx, size, type, reverse_gl_enum(type),
               normalized, stride, ptr);
    ppb_opengles2_VertexAttribPointer(context, indx, size, type, normalized, stride, ptr);
}

static
void
trace_ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d\n", __func__+6,
               context, x, y, width, height);
    ppb_opengles2_Viewport(context, x, y, width, height);
}

static
GLboolean
trace_ppb_opengles2_chromium_enable_feature_enable_feature_chromium(PP_Resource context,
                                                                    const char *feature)
{
    trace_info("[PPB] {zilch} %s feature=%s\n", __func__+6, feature);
    return ppb_opengles2_chromium_enable_feature_enable_feature_chromium(context, feature);
}

static
void *
trace_ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(PP_Resource context,
                                                                  GLuint target, GLintptr offset,
                                                                  GLsizeiptr size, GLenum access)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u, offset=%ld, size=%lu, access=%u(%s)\n",
               __func__+6, context, target, (long)offset, (unsigned long)size, access,
               reverse_gl_enum(access));
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
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), level=%d, xoffset=%d, yoffset=%d, "
               "width=%d, height=%d, format=%u(%s), type=%u(%s), access=%u(%s)\n", __func__+6,
               context, target, reverse_gl_enum(target), level, xoffset, yoffset, width, height,
               format, reverse_gl_enum(format), type, reverse_gl_enum(type), access,
               reverse_gl_enum(access));
    return ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(context, target, level,
                                        xoffset, yoffset, width, height, format, type, access);
}

static
void
trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                                     const void *mem)
{
    trace_info("[PPB] {full} %s context=%d, mem=%p\n", __func__+6, context, mem);
    ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(context, mem);
}

static
void
trace_ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(PP_Resource context, GLint srcX0,
                                                          GLint srcY0, GLint srcX1, GLint srcY1,
                                                          GLint dstX0, GLint dstY0, GLint dstX1,
                                                          GLint dstY1, GLbitfield mask,
                                                          GLenum filter)
{
    trace_info("[PPB] {zilch} %s context=%d, srcX0=%d, srcY0=%d, srcX1=%d, srcY1=%d, dstX0=%d, "
               "dstY0=%d, dstX1=%d, dstY1=%d, mask=0x%x, filter=%u(%s)\n", __func__+6, context,
               srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter,
               reverse_gl_enum(filter));
    return ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(context, srcX0, srcY0, srcX1, srcY1,
                                                               dstX0, dstY0, dstX1, dstY1, mask,
                                                               filter);
}

static
void
trace_ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext
                    (PP_Resource context, GLenum target, GLsizei samples, GLenum internalformat,
                     GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s), samples=%d, internalformat=%u(%s), "
               "width=%d, height=%d\n", __func__+6, context, target, reverse_gl_enum(target),
               samples, internalformat, reverse_gl_enum(internalformat), width, height);
    ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext(context, target,
                                                        samples, internalformat, width, height);
}

static
void
trace_ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle(PP_Resource context, GLenum mode,
                                                                 GLint first, GLsizei count,
                                                                 GLsizei primcount)
{
    trace_info("[PPB] {zilch} %s context=%d, mode=%u(%s), first=%d, count=%d, primcount=%d\n",
               __func__+6, context, mode, reverse_gl_enum(mode), first, count, primcount);
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
    trace_info("[PPB] {zilch} %s context=%d, mode=%u(%s), count=%d, type=%u(%s), indices=%p, "
               "primcount=%d\n", __func__+6, context, mode, reverse_gl_enum(mode), count, type,
               reverse_gl_enum(type), indices, primcount);
    ppb_opengles2_instanced_arrays_draw_elements_instanced_angle(context, mode, count, type,
                                                                 indices, primcount);
}

static
void
trace_ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(PP_Resource context, GLuint index,
                                                                 GLuint divisor)
{
    trace_info("[PPB] {zilch} %s countext=%d, index=%u, divisor=%u\n", __func__+6, context,
               index, divisor);
    ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(context, index, divisor);
}

static
void
trace_ppb_opengles2_query_gen_queries_ext(PP_Resource context, GLsizei n, GLuint *queries)
{
    trace_info("[PPB] {zilch} %s context=%d, n=%d, queries=%p\n", __func__+6, context, n, queries);
    ppb_opengles2_query_gen_queries_ext(context, n, queries);
}

static
void
trace_ppb_opengles2_query_delete_queries_ext(PP_Resource context, GLsizei n, const GLuint *queries)
{
    trace_info("[PPB] {zilch} %s context=%d, n=%d, queries=%p\n", __func__+6, context, n, queries);
    ppb_opengles2_query_delete_queries_ext(context, n, queries);
}

static
GLboolean
trace_ppb_opengles2_query_is_query_ext(PP_Resource context, GLuint id)
{
    trace_info("[PPB] {zilch} %s context=%d, id=%u\n", __func__+6, context, id);
    return ppb_opengles2_query_is_query_ext(context, id);
}

static
void
trace_ppb_opengles2_query_begin_query_ext(PP_Resource context, GLenum target, GLuint id)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s), id=%u\n", __func__+6, context, target,
               reverse_gl_enum(target), id);
    ppb_opengles2_query_begin_query_ext(context, target, id);
}

static
void
trace_ppb_opengles2_query_end_query_ext(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    ppb_opengles2_query_end_query_ext(context, target);
}

static
void
trace_ppb_opengles2_query_get_queryiv_ext(PP_Resource context, GLenum target, GLenum pname,
                                          GLint *params)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_query_get_queryiv_ext(context, target, pname, params);
}

static
void
trace_ppb_opengles2_query_get_query_objectuiv_ext(PP_Resource context, GLuint id, GLenum pname,
                                                  GLuint *params)
{
    trace_info("[PPB] {zilch} %s context=%d, id=%u, pname=%u(%s), params=%p\n", __func__+6,
               context, id, pname, reverse_gl_enum(pname), params);
    ppb_opengles2_query_get_query_objectuiv_ext(context, id, pname, params);
}
#endif // NDEBUG


const struct PPB_OpenGLES2 ppb_opengles2_interface_1_0 = {
    .ActiveTexture =            TWRAP(ppb_opengles2_ActiveTexture),
    .AttachShader =             TWRAP(ppb_opengles2_AttachShader),
    .BindAttribLocation =       TWRAP(ppb_opengles2_BindAttribLocation),
    .BindBuffer =               TWRAP(ppb_opengles2_BindBuffer),
    .BindFramebuffer =          TWRAP(ppb_opengles2_BindFramebuffer),
    .BindRenderbuffer =         TWRAP(ppb_opengles2_BindRenderbuffer),
    .BindTexture =              TWRAP(ppb_opengles2_BindTexture),
    .BlendColor =               TWRAP(ppb_opengles2_BlendColor),
    .BlendEquation =            TWRAP(ppb_opengles2_BlendEquation),
    .BlendEquationSeparate =    TWRAP(ppb_opengles2_BlendEquationSeparate),
    .BlendFunc =                TWRAP(ppb_opengles2_BlendFunc),
    .BlendFuncSeparate =        TWRAP(ppb_opengles2_BlendFuncSeparate),
    .BufferData =               TWRAP(ppb_opengles2_BufferData),
    .BufferSubData =            TWRAP(ppb_opengles2_BufferSubData),
    .CheckFramebufferStatus =   TWRAP(ppb_opengles2_CheckFramebufferStatus),
    .Clear =                    TWRAP(ppb_opengles2_Clear),
    .ClearColor =               TWRAP(ppb_opengles2_ClearColor),
    .ClearDepthf =              TWRAP(ppb_opengles2_ClearDepthf),
    .ClearStencil =             TWRAP(ppb_opengles2_ClearStencil),
    .ColorMask =                TWRAP(ppb_opengles2_ColorMask),
    .CompileShader =            TWRAP(ppb_opengles2_CompileShader),
    .CompressedTexImage2D =     TWRAP(ppb_opengles2_CompressedTexImage2D),
    .CompressedTexSubImage2D =  TWRAP(ppb_opengles2_CompressedTexSubImage2D),
    .CopyTexImage2D =           TWRAP(ppb_opengles2_CopyTexImage2D),
    .CopyTexSubImage2D =        TWRAP(ppb_opengles2_CopyTexSubImage2D),
    .CreateProgram =            TWRAP(ppb_opengles2_CreateProgram),
    .CreateShader =             TWRAP(ppb_opengles2_CreateShader),
    .CullFace =                 TWRAP(ppb_opengles2_CullFace),
    .DeleteBuffers =            TWRAP(ppb_opengles2_DeleteBuffers),
    .DeleteFramebuffers =       TWRAP(ppb_opengles2_DeleteFramebuffers),
    .DeleteProgram =            TWRAP(ppb_opengles2_DeleteProgram),
    .DeleteRenderbuffers =      TWRAP(ppb_opengles2_DeleteRenderbuffers),
    .DeleteShader =             TWRAP(ppb_opengles2_DeleteShader),
    .DeleteTextures =           TWRAP(ppb_opengles2_DeleteTextures),
    .DepthFunc =                TWRAP(ppb_opengles2_DepthFunc),
    .DepthMask =                TWRAP(ppb_opengles2_DepthMask),
    .DepthRangef =              TWRAP(ppb_opengles2_DepthRangef),
    .DetachShader =             TWRAP(ppb_opengles2_DetachShader),
    .Disable =                  TWRAP(ppb_opengles2_Disable),
    .DisableVertexAttribArray = TWRAP(ppb_opengles2_DisableVertexAttribArray),
    .DrawArrays =               TWRAP(ppb_opengles2_DrawArrays),
    .DrawElements =             TWRAP(ppb_opengles2_DrawElements),
    .Enable =                   TWRAP(ppb_opengles2_Enable),
    .EnableVertexAttribArray =  TWRAP(ppb_opengles2_EnableVertexAttribArray),
    .Finish =                   TWRAP(ppb_opengles2_Finish),
    .Flush =                    TWRAP(ppb_opengles2_Flush),
    .FramebufferRenderbuffer =  TWRAP(ppb_opengles2_FramebufferRenderbuffer),
    .FramebufferTexture2D =     TWRAP(ppb_opengles2_FramebufferTexture2D),
    .FrontFace =                TWRAP(ppb_opengles2_FrontFace),
    .GenBuffers =               TWRAP(ppb_opengles2_GenBuffers),
    .GenerateMipmap =           TWRAP(ppb_opengles2_GenerateMipmap),
    .GenFramebuffers =          TWRAP(ppb_opengles2_GenFramebuffers),
    .GenRenderbuffers =         TWRAP(ppb_opengles2_GenRenderbuffers),
    .GenTextures =              TWRAP(ppb_opengles2_GenTextures),
    .GetActiveAttrib =          TWRAP(ppb_opengles2_GetActiveAttrib),
    .GetActiveUniform =         TWRAP(ppb_opengles2_GetActiveUniform),
    .GetAttachedShaders =       TWRAP(ppb_opengles2_GetAttachedShaders),
    .GetAttribLocation =        TWRAP(ppb_opengles2_GetAttribLocation),
    .GetBooleanv =              TWRAP(ppb_opengles2_GetBooleanv),
    .GetBufferParameteriv =     TWRAP(ppb_opengles2_GetBufferParameteriv),
    .GetError =                 TWRAP(ppb_opengles2_GetError),
    .GetFloatv =                TWRAP(ppb_opengles2_GetFloatv),
    .GetFramebufferAttachmentParameteriv = TWRAP(ppb_opengles2_GetFramebufferAttachmentParameteriv),
    .GetIntegerv =              TWRAP(ppb_opengles2_GetIntegerv),
    .GetProgramiv =             TWRAP(ppb_opengles2_GetProgramiv),
    .GetProgramInfoLog =        TWRAP(ppb_opengles2_GetProgramInfoLog),
    .GetRenderbufferParameteriv = TWRAP(ppb_opengles2_GetRenderbufferParameteriv),
    .GetShaderiv =              TWRAP(ppb_opengles2_GetShaderiv),
    .GetShaderInfoLog =         TWRAP(ppb_opengles2_GetShaderInfoLog),
    .GetShaderPrecisionFormat = TWRAP(ppb_opengles2_GetShaderPrecisionFormat),
    .GetShaderSource =          TWRAP(ppb_opengles2_GetShaderSource),
    .GetString =                TWRAP(ppb_opengles2_GetString),
    .GetTexParameterfv =        TWRAP(ppb_opengles2_GetTexParameterfv),
    .GetTexParameteriv =        TWRAP(ppb_opengles2_GetTexParameteriv),
    .GetUniformfv =             TWRAP(ppb_opengles2_GetUniformfv),
    .GetUniformiv =             TWRAP(ppb_opengles2_GetUniformiv),
    .GetUniformLocation =       TWRAP(ppb_opengles2_GetUniformLocation),
    .GetVertexAttribfv =        TWRAP(ppb_opengles2_GetVertexAttribfv),
    .GetVertexAttribiv =        TWRAP(ppb_opengles2_GetVertexAttribiv),
    .GetVertexAttribPointerv =  TWRAP(ppb_opengles2_GetVertexAttribPointerv),
    .Hint =                     TWRAP(ppb_opengles2_Hint),
    .IsBuffer =                 TWRAP(ppb_opengles2_IsBuffer),
    .IsEnabled =                TWRAP(ppb_opengles2_IsEnabled),
    .IsFramebuffer =            TWRAP(ppb_opengles2_IsFramebuffer),
    .IsProgram =                TWRAP(ppb_opengles2_IsProgram),
    .IsRenderbuffer =           TWRAP(ppb_opengles2_IsRenderbuffer),
    .IsShader =                 TWRAP(ppb_opengles2_IsShader),
    .IsTexture =                TWRAP(ppb_opengles2_IsTexture),
    .LineWidth =                TWRAP(ppb_opengles2_LineWidth),
    .LinkProgram =              TWRAP(ppb_opengles2_LinkProgram),
    .PixelStorei =              TWRAP(ppb_opengles2_PixelStorei),
    .PolygonOffset =            TWRAP(ppb_opengles2_PolygonOffset),
    .ReadPixels =               TWRAP(ppb_opengles2_ReadPixels),
    .ReleaseShaderCompiler =    TWRAP(ppb_opengles2_ReleaseShaderCompiler),
    .RenderbufferStorage =      TWRAP(ppb_opengles2_RenderbufferStorage),
    .SampleCoverage =           TWRAP(ppb_opengles2_SampleCoverage),
    .Scissor =                  TWRAP(ppb_opengles2_Scissor),
    .ShaderBinary =             TWRAP(ppb_opengles2_ShaderBinary),
    .ShaderSource =             TWRAP(ppb_opengles2_ShaderSource),
    .StencilFunc =              TWRAP(ppb_opengles2_StencilFunc),
    .StencilFuncSeparate =      TWRAP(ppb_opengles2_StencilFuncSeparate),
    .StencilMask =              TWRAP(ppb_opengles2_StencilMask),
    .StencilMaskSeparate =      TWRAP(ppb_opengles2_StencilMaskSeparate),
    .StencilOp =                TWRAP(ppb_opengles2_StencilOp),
    .StencilOpSeparate =        TWRAP(ppb_opengles2_StencilOpSeparate),
    .TexImage2D =               TWRAP(ppb_opengles2_TexImage2D),
    .TexParameterf =            TWRAP(ppb_opengles2_TexParameterf),
    .TexParameterfv =           TWRAP(ppb_opengles2_TexParameterfv),
    .TexParameteri =            TWRAP(ppb_opengles2_TexParameteri),
    .TexParameteriv =           TWRAP(ppb_opengles2_TexParameteriv),
    .TexSubImage2D =            TWRAP(ppb_opengles2_TexSubImage2D),
    .Uniform1f =                TWRAP(ppb_opengles2_Uniform1f),
    .Uniform1fv =               TWRAP(ppb_opengles2_Uniform1fv),
    .Uniform1i =                TWRAP(ppb_opengles2_Uniform1i),
    .Uniform1iv =               TWRAP(ppb_opengles2_Uniform1iv),
    .Uniform2f =                TWRAP(ppb_opengles2_Uniform2f),
    .Uniform2fv =               TWRAP(ppb_opengles2_Uniform2fv),
    .Uniform2i =                TWRAP(ppb_opengles2_Uniform2i),
    .Uniform2iv =               TWRAP(ppb_opengles2_Uniform2iv),
    .Uniform3f =                TWRAP(ppb_opengles2_Uniform3f),
    .Uniform3fv =               TWRAP(ppb_opengles2_Uniform3fv),
    .Uniform3i =                TWRAP(ppb_opengles2_Uniform3i),
    .Uniform3iv =               TWRAP(ppb_opengles2_Uniform3iv),
    .Uniform4f =                TWRAP(ppb_opengles2_Uniform4f),
    .Uniform4fv =               TWRAP(ppb_opengles2_Uniform4fv),
    .Uniform4i =                TWRAP(ppb_opengles2_Uniform4i),
    .Uniform4iv =               TWRAP(ppb_opengles2_Uniform4iv),
    .UniformMatrix2fv =         TWRAP(ppb_opengles2_UniformMatrix2fv),
    .UniformMatrix3fv =         TWRAP(ppb_opengles2_UniformMatrix3fv),
    .UniformMatrix4fv =         TWRAP(ppb_opengles2_UniformMatrix4fv),
    .UseProgram =               TWRAP(ppb_opengles2_UseProgram),
    .ValidateProgram =          TWRAP(ppb_opengles2_ValidateProgram),
    .VertexAttrib1f =           TWRAP(ppb_opengles2_VertexAttrib1f),
    .VertexAttrib1fv =          TWRAP(ppb_opengles2_VertexAttrib1fv),
    .VertexAttrib2f =           TWRAP(ppb_opengles2_VertexAttrib2f),
    .VertexAttrib2fv =          TWRAP(ppb_opengles2_VertexAttrib2fv),
    .VertexAttrib3f =           TWRAP(ppb_opengles2_VertexAttrib3f),
    .VertexAttrib3fv =          TWRAP(ppb_opengles2_VertexAttrib3fv),
    .VertexAttrib4f =           TWRAP(ppb_opengles2_VertexAttrib4f),
    .VertexAttrib4fv =          TWRAP(ppb_opengles2_VertexAttrib4fv),
    .VertexAttribPointer =      TWRAP(ppb_opengles2_VertexAttribPointer),
    .Viewport =                 TWRAP(ppb_opengles2_Viewport),
};

const struct PPB_OpenGLES2ChromiumEnableFeature ppb_opengles2_chromium_enable_feature_interface_1_0 = {
    .EnableFeatureCHROMIUM = TWRAP(ppb_opengles2_chromium_enable_feature_enable_feature_chromium),
};

const struct PPB_OpenGLES2ChromiumMapSub ppb_opengles2_chromium_map_sub_interface_1_0 = {
    .MapBufferSubDataCHROMIUM =   TWRAP(ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium),
    .UnmapBufferSubDataCHROMIUM = TWRAP(ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium),
    .MapTexSubImage2DCHROMIUM =   TWRAP(ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium),
    .UnmapTexSubImage2DCHROMIUM = TWRAP(ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium),
};

const struct PPB_OpenGLES2FramebufferBlit ppb_opengles2_framebuffer_blit_interface_1_0 = {
    .BlitFramebufferEXT = TWRAP(ppb_opengles2_framebuffer_blit_blit_framebuffer_ext),
};

const struct PPB_OpenGLES2FramebufferMultisample ppb_opengles2_framebuffer_multisample_interface_1_0 = {
    .RenderbufferStorageMultisampleEXT =
                TWRAP(ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext),
};

const struct PPB_OpenGLES2InstancedArrays ppb_opengles2_instanced_arrays_interface_1_0 = {
    .DrawArraysInstancedANGLE = TWRAP(ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle),
    .DrawElementsInstancedANGLE =TWRAP(ppb_opengles2_instanced_arrays_draw_elements_instanced_angle),
    .VertexAttribDivisorANGLE = TWRAP(ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle),

};

const struct PPB_OpenGLES2Query ppb_opengles2_query_interface_1_0 = {
    .GenQueriesEXT =        TWRAP(ppb_opengles2_query_gen_queries_ext),
    .DeleteQueriesEXT =     TWRAP(ppb_opengles2_query_delete_queries_ext),
    .IsQueryEXT =           TWRAP(ppb_opengles2_query_is_query_ext),
    .BeginQueryEXT =        TWRAP(ppb_opengles2_query_begin_query_ext),
    .EndQueryEXT =          TWRAP(ppb_opengles2_query_end_query_ext),
    .GetQueryivEXT =        TWRAP(ppb_opengles2_query_get_queryiv_ext),
    .GetQueryObjectuivEXT = TWRAP(ppb_opengles2_query_get_query_objectuiv_ext),
};
