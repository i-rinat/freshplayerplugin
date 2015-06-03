/*
 * Copyright © 2013-2015  Rinat Ibragimov
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
#include <pthread.h>
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include <GLES2/gl2ext.h>
#if !HAVE_GLES2
#include "shader_translator.h"
#endif
#include "pp_interface.h"


#define PROLOGUE(g3d, escape_statement)                                                 \
    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D); \
    if (!g3d) {                                                                         \
        trace_error("%s, bad resource\n", __func__);                                    \
        escape_statement;                                                               \
    }                                                                                   \
    pthread_mutex_lock(&display.lock);                                                  \
    glXMakeCurrent(display.x, g3d->glx_pixmap, g3d->glc)

#define EPILOGUE()                                                                      \
    glXMakeCurrent(display.x, None, NULL);                                              \
    pthread_mutex_unlock(&display.lock);                                                \
    pp_resource_release(context)


#if !HAVE_GLES2
static GHashTable  *shader_type_ht = NULL;      // shader id -> shader type
static GHashTable  *shader_source_ht = NULL;    // shader id -> original shader source
#endif


static
void
__attribute__((destructor))
destructor_ppb_opengles2(void)
{
#if !HAVE_GLES2
    if (shader_type_ht)
        g_hash_table_unref(shader_type_ht);

    if (shader_source_ht)
        g_hash_table_unref(shader_source_ht);
#endif
}

void
ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
    PROLOGUE(g3d, return);
    glActiveTexture(texture);
    EPILOGUE();
}

void
ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
    PROLOGUE(g3d, return);
    glAttachShader(program, shader);
    EPILOGUE();
}

void
ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char *name)
{
    PROLOGUE(g3d, return);
    glBindAttribLocation(program, index, name);
    EPILOGUE();
}

void
ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
    PROLOGUE(g3d, return);
    glBindBuffer(target, buffer);
    EPILOGUE();
}

void
ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
    PROLOGUE(g3d, return);
    glBindFramebuffer(target, framebuffer);
    EPILOGUE();
}

void
ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
    PROLOGUE(g3d, return);
    glBindRenderbuffer(target, renderbuffer);
    EPILOGUE();
}

void
ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
    PROLOGUE(g3d, return);
    glBindTexture(target, texture);
    EPILOGUE();
}

void
ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    PROLOGUE(g3d, return);
    glBlendColor(red, green, blue, alpha);
    EPILOGUE();
}

void
ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
    PROLOGUE(g3d, return);
    glBlendEquation(mode);
    EPILOGUE();
}

void
ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
    PROLOGUE(g3d, return);
    glBlendEquationSeparate(modeRGB, modeAlpha);
    EPILOGUE();
}

void
ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
    PROLOGUE(g3d, return);
    glBlendFunc(sfactor, dfactor);
    EPILOGUE();
}

void
ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha)
{
    PROLOGUE(g3d, return);
    glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
    EPILOGUE();
}

void
ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void *data,
                         GLenum usage)
{
    PROLOGUE(g3d, return);
    glBufferData(target, size, data, usage);
    EPILOGUE();
}

void
ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void *data)
{
    PROLOGUE(g3d, return);
    glBufferSubData(target, offset, size, data);
    EPILOGUE();
}

GLenum
ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    PROLOGUE(g3d, return GL_FRAMEBUFFER_UNSUPPORTED);
    GLenum res = glCheckFramebufferStatus(target);
    EPILOGUE();
    return res;
}

void
ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
    PROLOGUE(g3d, return);
    glClear(mask);
    EPILOGUE();
}

void
ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    PROLOGUE(g3d, return);
    glClearColor(red, green, blue, alpha);
    EPILOGUE();
}

void
ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
    PROLOGUE(g3d, return);
    glClearDepthf(depth);
    EPILOGUE();
}

void
ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
    PROLOGUE(g3d, return);
    glClearStencil(s);
    EPILOGUE();
}

void
ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha)
{
    PROLOGUE(g3d, return);
    glColorMask(red, green, blue, alpha);
    EPILOGUE();
}

void
ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
    PROLOGUE(g3d, return);
    glCompileShader(shader);
    EPILOGUE();
}

void
ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void *data)
{
    PROLOGUE(g3d, return);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
    EPILOGUE();
}

void
ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void *data)
{
    PROLOGUE(g3d, return);
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize,
                              data);
    EPILOGUE();
}

void
ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    PROLOGUE(g3d, return);
    glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
    EPILOGUE();
}

void
ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    PROLOGUE(g3d, return);
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
    EPILOGUE();
}

GLuint
ppb_opengles2_CreateProgram(PP_Resource context)
{
    PROLOGUE(g3d, return 0);
    GLuint res = glCreateProgram();
    EPILOGUE();
    return res;
}

GLuint
ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    PROLOGUE(g3d, return 0);
    GLuint res = glCreateShader(type);
#if !HAVE_GLES2
    g_hash_table_insert(shader_type_ht, GSIZE_TO_POINTER(res), GSIZE_TO_POINTER(type));
#endif
    EPILOGUE();
    return res;
}

void
ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
    PROLOGUE(g3d, return);
    glCullFace(mode);
    EPILOGUE();
}

void
ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint *buffers)
{
    PROLOGUE(g3d, return);
    glDeleteBuffers(n, buffers);
    EPILOGUE();
}

void
ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint *framebuffers)
{
    PROLOGUE(g3d, return);
    glDeleteFramebuffers(n, framebuffers);
    EPILOGUE();
}

void
ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
    PROLOGUE(g3d, return);
    glDeleteProgram(program);
    EPILOGUE();
}

void
ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint *renderbuffers)
{
    PROLOGUE(g3d, return);
    glDeleteRenderbuffers(n, renderbuffers);
    EPILOGUE();
}

void
ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
    PROLOGUE(g3d, return);
    glDeleteShader(shader);

#if !HAVE_GLES2
    g_hash_table_remove(shader_source_ht, GSIZE_TO_POINTER(shader));
    g_hash_table_remove(shader_type_ht, GSIZE_TO_POINTER(shader));
#endif

    EPILOGUE();
}

void
ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint *textures)
{
    PROLOGUE(g3d, return);
    glDeleteTextures(n, textures);
    EPILOGUE();
}

void
ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
    PROLOGUE(g3d, return);
    glDepthFunc(func);
    EPILOGUE();
}

void
ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
    PROLOGUE(g3d, return);
    glDepthMask(flag);
    EPILOGUE();
}

void
ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
    PROLOGUE(g3d, return);
    glDepthRangef(zNear, zFar);
    EPILOGUE();
}

void
ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
    PROLOGUE(g3d, return);
    glDetachShader(program, shader);
    EPILOGUE();
}

void
ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
    PROLOGUE(g3d, return);
    glDisable(cap);
    EPILOGUE();
}

void
ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
    PROLOGUE(g3d, return);
    glDisableVertexAttribArray(index);
    EPILOGUE();
}

void
ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
    PROLOGUE(g3d, return);
    glDrawArrays(mode, first, count);
    EPILOGUE();
}

void
ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void *indices)
{
    PROLOGUE(g3d, return);
    glDrawElements(mode, count, type, indices);
    EPILOGUE();
}

void
ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
    PROLOGUE(g3d, return);
    glEnable(cap);
    EPILOGUE();
}

void
ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
    PROLOGUE(g3d, return);
    glEnableVertexAttribArray(index);
    EPILOGUE();
}

void
ppb_opengles2_Finish(PP_Resource context)
{
    PROLOGUE(g3d, return);
    glFinish();
    EPILOGUE();
}

void
ppb_opengles2_Flush(PP_Resource context)
{
    PROLOGUE(g3d, return);
    glFlush();
    EPILOGUE();
}

void
ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer)
{
    PROLOGUE(g3d, return);
    glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
    EPILOGUE();
}

void
ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level)
{
    PROLOGUE(g3d, return);
    glFramebufferTexture2D(target, attachment, textarget, texture, level);
    EPILOGUE();
}

void
ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
    PROLOGUE(g3d, return);
    glFrontFace(mode);
    EPILOGUE();
}

void
ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint *buffers)
{
    PROLOGUE(g3d, return);
    glGenBuffers(n, buffers);
    EPILOGUE();
}

void
ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
    PROLOGUE(g3d, return);
    glGenerateMipmap(target);
    EPILOGUE();
}

void
ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint *framebuffers)
{
    PROLOGUE(g3d, return);
    glGenFramebuffers(n, framebuffers);
    EPILOGUE();
}

void
ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint *renderbuffers)
{
    PROLOGUE(g3d, return);
    glGenRenderbuffers(n, renderbuffers);
    EPILOGUE();
}

void
ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint *textures)
{
    PROLOGUE(g3d, return);
    glGenTextures(n, textures);
    EPILOGUE();
}

void
ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei *length, GLint *size, GLenum *type, char *name)
{
    PROLOGUE(g3d, return);
    glGetActiveAttrib(program, index, bufsize, length, size, type, name);
    EPILOGUE();
}

void
ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei *length, GLint *size, GLenum *type, char *name)
{
    PROLOGUE(g3d, return);
    glGetActiveUniform(program, index, bufsize, length, size, type, name);
    EPILOGUE();
}

void
ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei *count, GLuint *shaders)
{
    PROLOGUE(g3d, return);
    glGetAttachedShaders(program, maxcount, count, shaders);
    EPILOGUE();
}

GLint
ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char *name)
{
    PROLOGUE(g3d, return 0);
    GLint res = glGetAttribLocation(program, name);
    EPILOGUE();
    return res;
}

void
ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean *params)
{
    PROLOGUE(g3d, return);
    glGetBooleanv(pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetBufferParameteriv(target, pname, params);
    EPILOGUE();
}

GLenum
ppb_opengles2_GetError(PP_Resource context)
{
    PROLOGUE(g3d, return GL_NO_ERROR);
    GLenum res = glGetError();
    EPILOGUE();
    return res;
}

void
ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat *params)
{
    PROLOGUE(g3d, return);
    glGetFloatv(pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetIntegerv(pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetProgramiv(program, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei *length, char *infolog)
{
    PROLOGUE(g3d, return);
    glGetProgramInfoLog(program, bufsize, length, infolog);
    EPILOGUE();
}

void
ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint *params)
{
    PROLOGUE(g3d, return);
    glGetRenderbufferParameteriv(target, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
#if HAVE_GLES2
    glGetShaderiv(shader, pname, params);
#else

    if (pname == GL_SHADER_SOURCE_LENGTH) {
        char *s = g_hash_table_lookup(shader_source_ht, GSIZE_TO_POINTER(shader));
        size_t len = s ? strlen(s) : 0;

        if (params)
            params[0] = len;
    } else {

        glGetShaderiv(shader, pname, params);
    }
#endif
    EPILOGUE();
}

void
ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                               char *infolog)
{
    PROLOGUE(g3d, return);
    glGetShaderInfoLog(shader, bufsize, length, infolog);
    EPILOGUE();
}

void
ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint *range, GLint *precision)
{
    PROLOGUE(g3d, return);
    glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
    EPILOGUE();
}

void
ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                              char *source)
{
    PROLOGUE(g3d, return);
#if HAVE_GLES2
    glGetShaderSource(shader, bufsize, length, source);
#else

    GLsizei  len;
    char    *s = g_hash_table_lookup(shader_source_ht, GSIZE_TO_POINTER(shader));
    if (!s) {
        len = 0;
        goto done;
    }

    len = strlen(s);

    // if shader source is too big, copy only what could be fit into a buffer
    if (len + 1 > bufsize)
        len = bufsize - 1;

    memcpy(source, s, len);
    source[len] = '\0';

done:
    if (length)
        *length = len;
#endif

    EPILOGUE();
}

const GLubyte *
ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    PROLOGUE(g3d, return (const GLubyte *)"");
    const GLubyte *res = glGetString(name);
    EPILOGUE();
    return res;
}

void
ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat *params)
{
    PROLOGUE(g3d, return);
    glGetTexParameterfv(target, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetTexParameteriv(target, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat *params)
{
    PROLOGUE(g3d, return);
    glGetUniformfv(program, location, params);
    EPILOGUE();
}

void
ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetUniformiv(program, location, params);
    EPILOGUE();
}

GLint
ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char *name)
{
    PROLOGUE(g3d, return 0);
    GLint res = glGetUniformLocation(program, name);
    EPILOGUE();
    return res;
}

void
ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat *params)
{
    PROLOGUE(g3d, return);
    glGetVertexAttribfv(index, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint *params)
{
    PROLOGUE(g3d, return);
    glGetVertexAttribiv(index, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void **pointer)
{
    PROLOGUE(g3d, return);
    glGetVertexAttribPointerv(index, pname, pointer);
    EPILOGUE();
}

void
ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
    PROLOGUE(g3d, return);
    glHint(target, mode);
    EPILOGUE();
}

GLboolean
ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsBuffer(buffer);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsEnabled(cap);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsFramebuffer(framebuffer);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsProgram(program);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsRenderbuffer(renderbuffer);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsShader(shader);
    EPILOGUE();
    return res;
}

GLboolean
ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    PROLOGUE(g3d, return GL_FALSE);
    GLboolean res = glIsTexture(texture);
    EPILOGUE();
    return res;
}

void
ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
    PROLOGUE(g3d, return);
    glLineWidth(width);
    EPILOGUE();
}

void
ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
    PROLOGUE(g3d, return);
    glLinkProgram(program);
    EPILOGUE();
}

void
ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
    PROLOGUE(g3d, return);
    glPixelStorei(pname, param);
    EPILOGUE();
}

void
ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
    PROLOGUE(g3d, return);
    glPolygonOffset(factor, units);
    EPILOGUE();
}

void
ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void *pixels)
{
    PROLOGUE(g3d, return);
    glReadPixels(x, y, width, height, format, type, pixels);
    EPILOGUE();
}

void
ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
    PROLOGUE(g3d, return);
    glReleaseShaderCompiler();
    EPILOGUE();
}

void
ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height)
{
    PROLOGUE(g3d, return);
    glRenderbufferStorage(target, internalformat, width, height);
    EPILOGUE();
}

void
ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
    PROLOGUE(g3d, return);
    glSampleCoverage(value, invert);
    EPILOGUE();
}

void
ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    PROLOGUE(g3d, return);
    glScissor(x, y, width, height);
    EPILOGUE();
}

void
ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint *shaders,
                           GLenum binaryformat, const void *binary, GLsizei length)
{
    PROLOGUE(g3d, return);
#if !HAVE_GLES2
    trace_error("%s, glShaderBinary is not supported yet, beware unexpected behavior\n", __func__);
#endif
    glShaderBinary(n, shaders, binaryformat, binary, length);
    EPILOGUE();
}

#if !HAVE_GLES2
static
char *
combine_shader_source_parts(GLsizei count, const char **str, const GLint *length)
{
    GString *res = g_string_new(NULL);

    for (GLsizei k = 0; k < count; k ++) {
        if (length)
            res = g_string_append_len(res, str[k], length[k]);
        else
            res = g_string_append(res, str[k]);
    }

    return g_string_free(res, FALSE);
}
#endif

void
ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char **str,
                           const GLint *length)
{
    PROLOGUE(g3d, return);
#if HAVE_GLES2
    glShaderSource(shader, count, str, length);
#else

    GLenum type = GPOINTER_TO_SIZE(g_hash_table_lookup(shader_type_ht, GSIZE_TO_POINTER(shader)));

    // save combined body
    char *body = combine_shader_source_parts(count, str, length);
    g_hash_table_insert(shader_source_ht, GSIZE_TO_POINTER(shader), body);

    // provide GL function with translated shader body
    char *translated_body = translate_shader(type, body);
    glShaderSource(shader, 1, (const char **)&translated_body, NULL);
    g_free(translated_body);
#endif

    EPILOGUE();
}

void
ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
    PROLOGUE(g3d, return);
    glStencilFunc(func, ref, mask);
    EPILOGUE();
}

void
ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask)
{
    PROLOGUE(g3d, return);
    glStencilFuncSeparate(face, func, ref, mask);
    EPILOGUE();
}

void
ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
    PROLOGUE(g3d, return);
    glStencilMask(mask);
    EPILOGUE();
}

void
ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
    PROLOGUE(g3d, return);
    glStencilMaskSeparate(face, mask);
    EPILOGUE();
}

void
ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
    PROLOGUE(g3d, return);
    glStencilOp(fail, zfail, zpass);
    EPILOGUE();
}

void
ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass)
{
    PROLOGUE(g3d, return);
    glStencilOpSeparate(face, fail, zfail, zpass);
    EPILOGUE();
}

void
ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void *pixels)
{
    PROLOGUE(g3d, return);
    glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
    EPILOGUE();
}

void
ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
    PROLOGUE(g3d, return);
    glTexParameterf(target, pname, param);
    EPILOGUE();
}

void
ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat *params)
{
    PROLOGUE(g3d, return);
    glTexParameterfv(target, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
    PROLOGUE(g3d, return);
    glTexParameteri(target, pname, param);
    EPILOGUE();
}

void
ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint *params)
{
    PROLOGUE(g3d, return);
    glTexParameteriv(target, pname, params);
    EPILOGUE();
}

void
ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void *pixels)
{
    PROLOGUE(g3d, return);
    glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
    EPILOGUE();
}

void
ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
    PROLOGUE(g3d, return);
    glUniform1f(location, x);
    EPILOGUE();
}

void
ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    PROLOGUE(g3d, return);
    glUniform1fv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
    PROLOGUE(g3d, return);
    glUniform1i(location, x);
    EPILOGUE();
}

void
ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    PROLOGUE(g3d, return);
    glUniform1iv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
    PROLOGUE(g3d, return);
    glUniform2f(location, x, y);
    EPILOGUE();
}

void
ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    PROLOGUE(g3d, return);
    glUniform2fv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
    PROLOGUE(g3d, return);
    glUniform2i(location, x, y);
    EPILOGUE();
}

void
ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    PROLOGUE(g3d, return);
    glUniform2iv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    PROLOGUE(g3d, return);
    glUniform3f(location, x, y, z);
    EPILOGUE();
}

void
ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    PROLOGUE(g3d, return);
    glUniform3fv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
    PROLOGUE(g3d, return);
    glUniform3i(location, x, y, z);
    EPILOGUE();
}

void
ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    PROLOGUE(g3d, return);
    glUniform3iv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w)
{
    PROLOGUE(g3d, return);
    glUniform4f(location, x, y, z, w);
    EPILOGUE();
}

void
ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    PROLOGUE(g3d, return);
    glUniform4fv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w)
{
    PROLOGUE(g3d, return);
    glUniform4i(location, x, y, z, w);
    EPILOGUE();
}

void
ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    PROLOGUE(g3d, return);
    glUniform4iv(location, count, v);
    EPILOGUE();
}

void
ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    PROLOGUE(g3d, return);
    glUniformMatrix2fv(location, count, transpose, value);
    EPILOGUE();
}

void
ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    PROLOGUE(g3d, return);
    glUniformMatrix3fv(location, count, transpose, value);
    EPILOGUE();
}

void
ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value)
{
    PROLOGUE(g3d, return);
    glUniformMatrix4fv(location, count, transpose, value);
    EPILOGUE();
}

void
ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
    PROLOGUE(g3d, return);
    glUseProgram(program);
    EPILOGUE();
}

void
ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
    PROLOGUE(g3d, return);
    glValidateProgram(program);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
    PROLOGUE(g3d, return);
    glVertexAttrib1f(indx, x);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    PROLOGUE(g3d, return);
    glVertexAttrib1fv(indx, values);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
    PROLOGUE(g3d, return);
    glVertexAttrib2f(indx, x, y);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    PROLOGUE(g3d, return);
    glVertexAttrib2fv(indx, values);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
    PROLOGUE(g3d, return);
    glVertexAttrib3f(indx, x, y, z);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    PROLOGUE(g3d, return);
    glVertexAttrib3fv(indx, values);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w)
{
    PROLOGUE(g3d, return);
    glVertexAttrib4f(indx, x, y, z, w);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    PROLOGUE(g3d, return);
    glVertexAttrib4fv(indx, values);
    EPILOGUE();
}

void
ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void *ptr)
{
    PROLOGUE(g3d, return);
    glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
    EPILOGUE();
}

void
ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    PROLOGUE(g3d, return);
    glViewport(x, y, width, height);
    EPILOGUE();
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
    if (target != GL_TEXTURE_2D || level != 0 || access != GL_WRITE_ONLY_OES) {
        trace_error("%s, bad arguments\n", __func__);
        return NULL;
    }

    struct pp_graphics3d_s *g3d = pp_resource_acquire(context, PP_RESOURCE_GRAPHICS3D);
    if (!g3d) {
        trace_error("%s, bad resource\n", __func__);
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
    PROLOGUE(g3d, return);
    struct tex_sub_mapping_param_s *mp = g_hash_table_lookup(g3d->sub_maps, mem);
    if (!mp) {
        trace_error("%s, memory was not mapped\n", __func__);
        goto err;
    }

    g_hash_table_remove(g3d->sub_maps, mem);
    glTexSubImage2D(GL_TEXTURE_2D, mp->level, mp->xoffset, mp->yoffset, mp->width, mp->height,
                    mp->format, mp->type, mem);
    g_slice_free(struct tex_sub_mapping_param_s, mp);
    free((void *)mem);

err:
    EPILOGUE();
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


// trace wrappers
TRACE_WRAPPER
void
trace_ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
    trace_info("[PPB] {full} %s context=%d, texture=%u(%s)\n", __func__+6, context,
               texture, reverse_gl_enum(texture));
    ppb_opengles2_ActiveTexture(context, texture);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, program=%d, shader=%d\n", __func__+6, context,
               program, shader);
    ppb_opengles2_AttachShader(context, program, shader);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                       const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%d, index=%d, name=%s\n", __func__+6,
               context, program, index, name);
    ppb_opengles2_BindAttribLocation(context, program, index, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), buffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), buffer);
    ppb_opengles2_BindBuffer(context, target, buffer);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), framebuffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), framebuffer);
    ppb_opengles2_BindFramebuffer(context, target, framebuffer);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), renderbuffer=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), renderbuffer);
    ppb_opengles2_BindRenderbuffer(context, target, renderbuffer);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), texture=%u\n", __func__+6, context,
               target, reverse_gl_enum(target), texture);
    ppb_opengles2_BindTexture(context, target, texture);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                               GLclampf alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%f, green=%f, blue=%f, alpha=%f\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_BlendColor(context, red, green, blue, alpha);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context, mode,
               reverse_gl_enum(mode));
    ppb_opengles2_BlendEquation(context, mode);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
    trace_info("[PPB] {full} %s context=%d, modeRGB=%u(%s), modeAlpha=%u(%s)\n", __func__+6,
               context, modeRGB, reverse_gl_enum(modeRGB), modeAlpha, reverse_gl_enum(modeAlpha));
    ppb_opengles2_BlendEquationSeparate(context, modeRGB, modeAlpha);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
    trace_info("[PPB] {full} %s context=%d, sfactor=%u(%s), dfactor=%u(%s)\n", __func__+6,
               context, sfactor, reverse_gl_enum(sfactor), dfactor, reverse_gl_enum(dfactor));
    ppb_opengles2_BlendFunc(context, sfactor, dfactor);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size,
                               const void *data, GLenum usage)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), size=%ld, data=%p, usage=%u(%s)\n",
               __func__+6, context, target, reverse_gl_enum(target), (long int)size, data,
               usage, reverse_gl_enum(usage));
    ppb_opengles2_BufferData(context, target, size, data, usage);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset,
                                  GLsizeiptr size, const void *data)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), offset=%ld, size=%ld, data=%p\n",
               __func__+6, context, target, reverse_gl_enum(target), (long)offset, (long)size,
               data);
    ppb_opengles2_BufferSubData(context, target, offset, size, data);
}

TRACE_WRAPPER
GLenum
trace_ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    return ppb_opengles2_CheckFramebufferStatus(context, target);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
    trace_info("[PPB] {full} %s context=%d, mask=0x%x\n", __func__+6, context, mask);
    ppb_opengles2_Clear(context, mask);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                               GLclampf alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%f, green=%f, blue=%f, alpha=%f\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_ClearColor(context, red, green, blue, alpha);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
    trace_info("[PPB] {full} %s context=%d, depth=%f\n", __func__+6, context, depth);
    ppb_opengles2_ClearDepthf(context, depth);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
    trace_info("[PPB] {full} %s context=%d, s=%d\n", __func__+6, context, s);
    ppb_opengles2_ClearStencil(context, s);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                              GLboolean alpha)
{
    trace_info("[PPB] {full} %s context=%d, red=%d, green=%d, blue=%d, alpha=%d\n", __func__+6,
               context, red, green, blue, alpha);
    ppb_opengles2_ColorMask(context, red, green, blue, alpha);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%d\n", __func__+6, context, shader);
    ppb_opengles2_CompileShader(context, shader);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
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

TRACE_WRAPPER
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

TRACE_WRAPPER
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

TRACE_WRAPPER
GLuint
trace_ppb_opengles2_CreateProgram(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_CreateProgram(context);
}

TRACE_WRAPPER
GLuint
trace_ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    trace_info("[PPB] {full} %s context=%d type=%u(%s)\n", __func__+6, context, type,
               reverse_gl_enum(type));
    return ppb_opengles2_CreateShader(context, type);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context, mode,
               reverse_gl_enum(mode));
    ppb_opengles2_CullFace(context, mode);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint *buffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, buffers=%p\n", __func__+6, context,
               n, buffers);
    ppb_opengles2_DeleteBuffers(context, n, buffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint *framebuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, framebuffers=%p\n", __func__+6, context,
               n, framebuffers);
    ppb_opengles2_DeleteFramebuffers(context, n, framebuffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_DeleteProgram(context, program);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint *renderbuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, renderbuffers=%p\n", __func__+6, context,
               n, renderbuffers);
    ppb_opengles2_DeleteRenderbuffers(context, n, renderbuffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u\n", __func__+6, context, shader);
    ppb_opengles2_DeleteShader(context, shader);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint *textures)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, textures=%p\n", __func__+6, context, n, textures);
    ppb_opengles2_DeleteTextures(context, n, textures);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
    trace_info("[PPB] {full} %s context=%d, func=%u(%s)\n", __func__+6, context, func,
               reverse_gl_enum(func));
    ppb_opengles2_DepthFunc(context, func);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
    trace_info("[PPB] {full} %s context=%d, flag=%d\n", __func__+6, context, flag);
    ppb_opengles2_DepthMask(context, flag);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
    trace_info("[PPB] {full} %s context=%d, zNear=%f, zFar=%f\n", __func__+6, context, zNear, zFar);
    ppb_opengles2_DepthRangef(context, zNear, zFar);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, shader=%u\n", __func__+6, context,
               program, shader);
    ppb_opengles2_DetachShader(context, program, shader);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    ppb_opengles2_Disable(context, cap);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {full} %s context=%d, index=%u\n", __func__+6, context, index);
    ppb_opengles2_DisableVertexAttribArray(context, index);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s), first=%d, count=%d\n", __func__+6,
               context, mode, reverse_gl_enum(mode), first, count);
    ppb_opengles2_DrawArrays(context, mode, first, count);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                                 const void *indices)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s), count=%d, type=%u(%s), indices=%p\n",
               __func__+6, context, mode, reverse_gl_enum(mode), count, type,
               reverse_gl_enum(type), indices);
    ppb_opengles2_DrawElements(context, mode, count, type, indices);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    ppb_opengles2_Enable(context, cap);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {full} %s context=%d, index=%u\n", __func__+6, context, index);
    ppb_opengles2_EnableVertexAttribArray(context, index);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Finish(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Finish(context);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Flush(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Flush(context);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, mode=%u(%s)\n", __func__+6, context,
               mode, reverse_gl_enum(mode));
    ppb_opengles2_FrontFace(context, mode);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint *buffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, buffers=%p\n", __func__+6, context, n, buffers);
    ppb_opengles2_GenBuffers(context, n, buffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    ppb_opengles2_GenerateMipmap(context, target);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint *framebuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, framebuffers=%p\n", __func__+6, context,
               n, framebuffers);
    ppb_opengles2_GenFramebuffers(context, n, framebuffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint *renderbuffers)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, renderbuffers=%p\n", __func__+6, context,
               n, renderbuffers);
    ppb_opengles2_GenRenderbuffers(context, n, renderbuffers);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint *textures)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, textures=%p\n", __func__+6, context, n, textures);
    ppb_opengles2_GenTextures(context, n, textures);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index,
                                    GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type,
                                    char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, index=%u, bufsize=%d\n", __func__+6,
               context, program, index, bufsize);
    ppb_opengles2_GetActiveAttrib(context, program, index, bufsize, length, size, type, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index,
                                     GLsizei bufsize, GLsizei *length, GLint *size, GLenum *type,
                                     char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, index=%u, bufsize=%d\n", __func__+6,
               context, program, index, bufsize);
    ppb_opengles2_GetActiveUniform(context, program, index, bufsize, length, size, type, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                       GLsizei *count, GLuint *shaders)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, maxcount=%d\n", __func__+6, context,
               program, maxcount);
    ppb_opengles2_GetAttachedShaders(context, program, maxcount, count, shaders);
}

TRACE_WRAPPER
GLint
trace_ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, name=%s\n", __func__+6, context,
               program, name);
    return ppb_opengles2_GetAttribLocation(context, program, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetBooleanv(context, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetBufferParameteriv(context, target, pname, params);
}

TRACE_WRAPPER
GLenum
trace_ppb_opengles2_GetError(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetError(context);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetFloatv(context, pname, params);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s)\n", __func__+6, context,
               pname, reverse_gl_enum(pname));
    ppb_opengles2_GetIntegerv(context, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, pname=%u(%s)\n", __func__+6, context,
               program, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetProgramiv(context, program, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                      GLsizei *length, char *infolog)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, bufsize=%d\n", __func__+6, context,
               program, bufsize);
    ppb_opengles2_GetProgramInfoLog(context, program, bufsize, length, infolog);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                               GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetRenderbufferParameteriv(context, target, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, pname=%u(%s)\n", __func__+6, context,
               shader, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetShaderiv(context, shader, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize,
                                     GLsizei *length, char *infolog)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, bufsize=%d\n", __func__+6, context,
               shader, bufsize);
    ppb_opengles2_GetShaderInfoLog(context, shader, bufsize, length, infolog);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype,
                                             GLenum precisiontype, GLint *range, GLint *precision)
{
    trace_info("[PPB] {full} %s context=%d, shadertype=%u(%s), precisiontype=%u(%s)\n", __func__+6,
               context, shadertype, reverse_gl_enum(shadertype), precisiontype,
               reverse_gl_enum(precisiontype));
    ppb_opengles2_GetShaderPrecisionFormat(context, shadertype, precisiontype, range, precision);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize,
                                    GLsizei *length, char *source)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u, bufsize=%d\n", __func__+6, context,
               shader, bufsize);
    ppb_opengles2_GetShaderSource(context, shader, bufsize, length, source);
}

TRACE_WRAPPER
const GLubyte *
trace_ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    trace_info("[PPB] {full} %s context=%d name=%u(%s)\n", __func__+6, context,
               name, reverse_gl_enum(name));
    return ppb_opengles2_GetString(context, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                                      GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetTexParameterfv(context, target, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                      GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), pname, reverse_gl_enum(pname));
    ppb_opengles2_GetTexParameteriv(context, target, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location,
                                 GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, location=%d\n", __func__+6, context,
               program, location);
    ppb_opengles2_GetUniformfv(context, program, location, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, location=%d\n", __func__+6, context,
               program, location);
    ppb_opengles2_GetUniformiv(context, program, location, params);
}

TRACE_WRAPPER
GLint
trace_ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char *name)
{
    trace_info("[PPB] {full} %s context=%d, program=%u, name=%s\n", __func__+6, context,
               program, name);
    return ppb_opengles2_GetUniformLocation(context, program, name);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname,
                                      GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribfv(context, index, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname,
                                      GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribiv(context, index, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                            void **pointer)
{
    trace_info("[PPB] {full} %s context=%d, index=%u, pname=%u(%s)\n", __func__+6, context,
               index, pname, reverse_gl_enum(pname));
    ppb_opengles2_GetVertexAttribPointerv(context, index, pname, pointer);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), mode=%u(%s)\n", __func__+6, context,
               target, reverse_gl_enum(target), mode, reverse_gl_enum(mode));
    ppb_opengles2_Hint(context, target, mode);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    trace_info("[PPB] {full} %s context=%d, buffer=%u\n", __func__+6, context, buffer);
    return ppb_opengles2_IsBuffer(context, buffer);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {full} %s context=%d, cap=%u(%s)\n", __func__+6, context,
               cap, reverse_gl_enum(cap));
    return ppb_opengles2_IsEnabled(context, cap);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    trace_info("[PPB] {full} %s context=%d, framebuffer=%u\n", __func__+6, context, framebuffer);
    return ppb_opengles2_IsFramebuffer(context, framebuffer);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    return ppb_opengles2_IsProgram(context, program);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    trace_info("[PPB] {full} %s context=%d, renderbuffer=%u\n", __func__+6, context, renderbuffer);
    return ppb_opengles2_IsRenderbuffer(context, renderbuffer);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {full} %s context=%d, shader=%u\n", __func__+6, context, shader);
    return ppb_opengles2_IsShader(context, shader);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    trace_info("[PPB] {full} %s context=%d, texture=%u\n", __func__+6, context, texture);
    return ppb_opengles2_IsTexture(context, texture);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
    trace_info("[PPB] {full} %s context=%d, width=%f\n", __func__+6, context, width);
    ppb_opengles2_LineWidth(context, width);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_LinkProgram(context, program);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
    trace_info("[PPB] {full} %s context=%d, pname=%u(%s), param=%d\n", __func__+6, context,
               pname, reverse_gl_enum(pname), param);
    ppb_opengles2_PixelStorei(context, pname, param);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
    trace_info("[PPB] {full} %s context=%d, factor=%f, units=%f\n", __func__+6, context,
               factor, units);
    ppb_opengles2_PolygonOffset(context, factor, units);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                               GLenum format, GLenum type, void *pixels)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d, format=%u(%s), "
               "type=%u(%s), pixels=%p\n", __func__+6, context, x, y, width, height, format,
               reverse_gl_enum(format), type, reverse_gl_enum(type), pixels);
    ppb_opengles2_ReadPixels(context, x, y, width, height, format, type, pixels);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
    trace_info("[PPB] {full} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ReleaseShaderCompiler(context);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                        GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), internalformat=%u(%s), width=%d, "
               "height=%d\n", __func__+6, context, target, reverse_gl_enum(target),
               internalformat, reverse_gl_enum(internalformat), width, height);
    ppb_opengles2_RenderbufferStorage(context, target, internalformat, width, height);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
    trace_info("[PPB] {full} %s context=%d, value=%f, invert=%d\n", __func__+6, context,
               value, invert);
    ppb_opengles2_SampleCoverage(context, value, invert);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d\n", __func__+6,
               context, x, y, width, height);
    ppb_opengles2_Scissor(context, x, y, width, height);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint *shaders,
                                 GLenum binaryformat, const void *binary, GLsizei length)
{
    trace_info("[PPB] {full} %s context=%d, n=%d, shaders=%p, binaryformat=%u(%s), binary=%p, "
               "length=%d\n", __func__+6, context, n, shaders, binaryformat,
               reverse_gl_enum(binaryformat), binary, length);
    ppb_opengles2_ShaderBinary(context, n, shaders, binaryformat, binary, length);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, func=%u(%s), ref=%d, mask=0x%x\n", __func__+6, context,
               func, reverse_gl_enum(func), ref, mask);
    ppb_opengles2_StencilFunc(context, func, ref, mask);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                        GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), func=%u(%s), ref=%d, mask=0x%x\n",
               __func__+6, context, face, reverse_gl_enum(face), func, reverse_gl_enum(func),
               ref, mask);
    ppb_opengles2_StencilFuncSeparate(context, face, func, ref, mask);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, mask=0x%x\n", __func__+6, context, mask);
    ppb_opengles2_StencilMask(context, mask);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), mask=0x%x\n", __func__+6, context,
               face, reverse_gl_enum(face), mask);
    ppb_opengles2_StencilMaskSeparate(context, face, mask);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
    trace_info("[PPB] {full} %s context=%d, fail=%u(%s), zfail=%u(%s), zpass=%u(%s)\n", __func__+6,
               context, fail, reverse_gl_enum(fail), zfail, reverse_gl_enum(zfail),
               zpass, reverse_gl_enum(zpass));
    ppb_opengles2_StencilOp(context, fail, zfail, zpass);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                      GLenum zpass)
{
    trace_info("[PPB] {full} %s context=%d, face=%u(%s), fail=%u(%s), zfail=%u(%s), zpass=%u(%s)\n",
               __func__+6, context, face, reverse_gl_enum(face), fail, reverse_gl_enum(fail),
               zfail, reverse_gl_enum(zfail), zpass, reverse_gl_enum(zpass));
    ppb_opengles2_StencilOpSeparate(context, face, fail, zfail, zpass);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), param=%f\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), param);
    ppb_opengles2_TexParameterf(context, target, pname, param);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                                   const GLfloat *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_TexParameterfv(context, target, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), param=%d\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), param);
    ppb_opengles2_TexParameteri(context, target, pname, param);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                   const GLint *params)
{
    trace_info("[PPB] {full} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_TexParameteriv(context, target, pname, params);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f\n", __func__+6, context, location, x);
    ppb_opengles2_Uniform1f(context, location, x);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform1fv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d\n", __func__+6, context, location, x);
    ppb_opengles2_Uniform1i(context, location, x);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform1iv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f\n", __func__+6, context,
               location, x, y);
    ppb_opengles2_Uniform2f(context, location, x, y);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform2fv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d\n", __func__+6, context,
               location, x, y);
    ppb_opengles2_Uniform2i(context, location, x, y);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform2iv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f, z=%f\n", __func__+6, context,
               location, x, y, z);
    ppb_opengles2_Uniform3f(context, location, x, y, z);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform3fv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d, z=%d\n", __func__+6, context,
               location, x, y, z);
    ppb_opengles2_Uniform3i(context, location, x, y, z);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform3iv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                              GLfloat w)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%f, y=%f, z=%f, w=%f\n", __func__+6,
               context, location, x, y, z, w);
    ppb_opengles2_Uniform4f(context, location, x, y, z, w);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform4fv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z,
                              GLint w)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, x=%d, y=%d, z=%d, w=%d\n", __func__+6,
               context, location, x, y, z, w);
    ppb_opengles2_Uniform4i(context, location, x, y, z, w);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint *v)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, v=%p\n", __func__+6, context,
               location, count, v);
    ppb_opengles2_Uniform4iv(context, location, count, v);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix2fv(context, location, count, transpose, value);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix3fv(context, location, count, transpose, value);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                                     GLboolean transpose, const GLfloat *value)
{
    trace_info("[PPB] {full} %s context=%d, location=%d, count=%d, transpose=%d, value=%p\n",
               __func__+6, context, location, count, transpose, value);
    ppb_opengles2_UniformMatrix4fv(context, location, count, transpose, value);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_UseProgram(context, program);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {full} %s context=%d, program=%u\n", __func__+6, context, program);
    ppb_opengles2_ValidateProgram(context, program);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f\n", __func__+6, context, indx, x);
    ppb_opengles2_VertexAttrib1f(context, indx, x);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib1fv(context, indx, values);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f\n", __func__+6, context,
               indx, x, y);
    ppb_opengles2_VertexAttrib2f(context, indx, x, y);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context, indx,
               values);
    ppb_opengles2_VertexAttrib2fv(context, indx, values);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y,
                                   GLfloat z)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f, z=%f\n", __func__+6, context,
               indx, x, y, z);
    ppb_opengles2_VertexAttrib3f(context, indx, x, y, z);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib3fv(context, indx, values);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y,
                                   GLfloat z, GLfloat w)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, x=%f, y=%f, z=%f, w=%f\n", __func__+6,
               context, indx, x, y, z, w);
    ppb_opengles2_VertexAttrib4f(context, indx, x, y, z, w);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat *values)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, values=%p\n", __func__+6, context,
               indx, values);
    ppb_opengles2_VertexAttrib4fv(context, indx, values);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                        GLboolean normalized, GLsizei stride, const void *ptr)
{
    trace_info("[PPB] {full} %s context=%d, indx=%u, size=%d, type=%u(%s), normalized=%d, "
               "stride=%d, ptr=%p\n", __func__+6, context, indx, size, type, reverse_gl_enum(type),
               normalized, stride, ptr);
    ppb_opengles2_VertexAttribPointer(context, indx, size, type, normalized, stride, ptr);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {full} %s context=%d, x=%d, y=%d, width=%d, height=%d\n", __func__+6,
               context, x, y, width, height);
    ppb_opengles2_Viewport(context, x, y, width, height);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_chromium_enable_feature_enable_feature_chromium(PP_Resource context,
                                                                    const char *feature)
{
    trace_info("[PPB] {zilch} %s feature=%s\n", __func__+6, feature);
    return ppb_opengles2_chromium_enable_feature_enable_feature_chromium(context, feature);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(PP_Resource context,
                                                                    const void *mem)
{
    trace_info("[PPB] {zilch} %s context=%d, mem=%p\n", __func__+6, context, mem);
    ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(context, mem);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                                     const void *mem)
{
    trace_info("[PPB] {full} %s context=%d, mem=%p\n", __func__+6, context, mem);
    ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(context, mem);
}

TRACE_WRAPPER
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

TRACE_WRAPPER
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

TRACE_WRAPPER
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


TRACE_WRAPPER
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

TRACE_WRAPPER
void
trace_ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(PP_Resource context, GLuint index,
                                                                 GLuint divisor)
{
    trace_info("[PPB] {zilch} %s countext=%d, index=%u, divisor=%u\n", __func__+6, context,
               index, divisor);
    ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(context, index, divisor);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_gen_queries_ext(PP_Resource context, GLsizei n, GLuint *queries)
{
    trace_info("[PPB] {zilch} %s context=%d, n=%d, queries=%p\n", __func__+6, context, n, queries);
    ppb_opengles2_query_gen_queries_ext(context, n, queries);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_delete_queries_ext(PP_Resource context, GLsizei n, const GLuint *queries)
{
    trace_info("[PPB] {zilch} %s context=%d, n=%d, queries=%p\n", __func__+6, context, n, queries);
    ppb_opengles2_query_delete_queries_ext(context, n, queries);
}

TRACE_WRAPPER
GLboolean
trace_ppb_opengles2_query_is_query_ext(PP_Resource context, GLuint id)
{
    trace_info("[PPB] {zilch} %s context=%d, id=%u\n", __func__+6, context, id);
    return ppb_opengles2_query_is_query_ext(context, id);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_begin_query_ext(PP_Resource context, GLenum target, GLuint id)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s), id=%u\n", __func__+6, context, target,
               reverse_gl_enum(target), id);
    ppb_opengles2_query_begin_query_ext(context, target, id);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_end_query_ext(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s)\n", __func__+6, context, target,
               reverse_gl_enum(target));
    ppb_opengles2_query_end_query_ext(context, target);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_get_queryiv_ext(PP_Resource context, GLenum target, GLenum pname,
                                          GLint *params)
{
    trace_info("[PPB] {zilch} %s context=%d, target=%u(%s), pname=%u(%s), params=%p\n", __func__+6,
               context, target, reverse_gl_enum(target), pname, reverse_gl_enum(pname), params);
    ppb_opengles2_query_get_queryiv_ext(context, target, pname, params);
}

TRACE_WRAPPER
void
trace_ppb_opengles2_query_get_query_objectuiv_ext(PP_Resource context, GLuint id, GLenum pname,
                                                  GLuint *params)
{
    trace_info("[PPB] {zilch} %s context=%d, id=%u, pname=%u(%s), params=%p\n", __func__+6,
               context, id, pname, reverse_gl_enum(pname), params);
    ppb_opengles2_query_get_query_objectuiv_ext(context, id, pname, params);
}


const struct PPB_OpenGLES2 ppb_opengles2_interface_1_0 = {
    .ActiveTexture =            TWRAPF(ppb_opengles2_ActiveTexture),
    .AttachShader =             TWRAPF(ppb_opengles2_AttachShader),
    .BindAttribLocation =       TWRAPF(ppb_opengles2_BindAttribLocation),
    .BindBuffer =               TWRAPF(ppb_opengles2_BindBuffer),
    .BindFramebuffer =          TWRAPF(ppb_opengles2_BindFramebuffer),
    .BindRenderbuffer =         TWRAPF(ppb_opengles2_BindRenderbuffer),
    .BindTexture =              TWRAPF(ppb_opengles2_BindTexture),
    .BlendColor =               TWRAPF(ppb_opengles2_BlendColor),
    .BlendEquation =            TWRAPF(ppb_opengles2_BlendEquation),
    .BlendEquationSeparate =    TWRAPF(ppb_opengles2_BlendEquationSeparate),
    .BlendFunc =                TWRAPF(ppb_opengles2_BlendFunc),
    .BlendFuncSeparate =        TWRAPF(ppb_opengles2_BlendFuncSeparate),
    .BufferData =               TWRAPF(ppb_opengles2_BufferData),
    .BufferSubData =            TWRAPF(ppb_opengles2_BufferSubData),
    .CheckFramebufferStatus =   TWRAPF(ppb_opengles2_CheckFramebufferStatus),
    .Clear =                    TWRAPF(ppb_opengles2_Clear),
    .ClearColor =               TWRAPF(ppb_opengles2_ClearColor),
    .ClearDepthf =              TWRAPF(ppb_opengles2_ClearDepthf),
    .ClearStencil =             TWRAPF(ppb_opengles2_ClearStencil),
    .ColorMask =                TWRAPF(ppb_opengles2_ColorMask),
    .CompileShader =            TWRAPF(ppb_opengles2_CompileShader),
    .CompressedTexImage2D =     TWRAPF(ppb_opengles2_CompressedTexImage2D),
    .CompressedTexSubImage2D =  TWRAPF(ppb_opengles2_CompressedTexSubImage2D),
    .CopyTexImage2D =           TWRAPF(ppb_opengles2_CopyTexImage2D),
    .CopyTexSubImage2D =        TWRAPF(ppb_opengles2_CopyTexSubImage2D),
    .CreateProgram =            TWRAPF(ppb_opengles2_CreateProgram),
    .CreateShader =             TWRAPF(ppb_opengles2_CreateShader),
    .CullFace =                 TWRAPF(ppb_opengles2_CullFace),
    .DeleteBuffers =            TWRAPF(ppb_opengles2_DeleteBuffers),
    .DeleteFramebuffers =       TWRAPF(ppb_opengles2_DeleteFramebuffers),
    .DeleteProgram =            TWRAPF(ppb_opengles2_DeleteProgram),
    .DeleteRenderbuffers =      TWRAPF(ppb_opengles2_DeleteRenderbuffers),
    .DeleteShader =             TWRAPF(ppb_opengles2_DeleteShader),
    .DeleteTextures =           TWRAPF(ppb_opengles2_DeleteTextures),
    .DepthFunc =                TWRAPF(ppb_opengles2_DepthFunc),
    .DepthMask =                TWRAPF(ppb_opengles2_DepthMask),
    .DepthRangef =              TWRAPF(ppb_opengles2_DepthRangef),
    .DetachShader =             TWRAPF(ppb_opengles2_DetachShader),
    .Disable =                  TWRAPF(ppb_opengles2_Disable),
    .DisableVertexAttribArray = TWRAPF(ppb_opengles2_DisableVertexAttribArray),
    .DrawArrays =               TWRAPF(ppb_opengles2_DrawArrays),
    .DrawElements =             TWRAPF(ppb_opengles2_DrawElements),
    .Enable =                   TWRAPF(ppb_opengles2_Enable),
    .EnableVertexAttribArray =  TWRAPF(ppb_opengles2_EnableVertexAttribArray),
    .Finish =                   TWRAPF(ppb_opengles2_Finish),
    .Flush =                    TWRAPF(ppb_opengles2_Flush),
    .FramebufferRenderbuffer =  TWRAPF(ppb_opengles2_FramebufferRenderbuffer),
    .FramebufferTexture2D =     TWRAPF(ppb_opengles2_FramebufferTexture2D),
    .FrontFace =                TWRAPF(ppb_opengles2_FrontFace),
    .GenBuffers =               TWRAPF(ppb_opengles2_GenBuffers),
    .GenerateMipmap =           TWRAPF(ppb_opengles2_GenerateMipmap),
    .GenFramebuffers =          TWRAPF(ppb_opengles2_GenFramebuffers),
    .GenRenderbuffers =         TWRAPF(ppb_opengles2_GenRenderbuffers),
    .GenTextures =              TWRAPF(ppb_opengles2_GenTextures),
    .GetActiveAttrib =          TWRAPF(ppb_opengles2_GetActiveAttrib),
    .GetActiveUniform =         TWRAPF(ppb_opengles2_GetActiveUniform),
    .GetAttachedShaders =       TWRAPF(ppb_opengles2_GetAttachedShaders),
    .GetAttribLocation =        TWRAPF(ppb_opengles2_GetAttribLocation),
    .GetBooleanv =              TWRAPF(ppb_opengles2_GetBooleanv),
    .GetBufferParameteriv =     TWRAPF(ppb_opengles2_GetBufferParameteriv),
    .GetError =                 TWRAPF(ppb_opengles2_GetError),
    .GetFloatv =                TWRAPF(ppb_opengles2_GetFloatv),
    .GetFramebufferAttachmentParameteriv = TWRAPF(ppb_opengles2_GetFramebufferAttachmentParameteriv),
    .GetIntegerv =              TWRAPF(ppb_opengles2_GetIntegerv),
    .GetProgramiv =             TWRAPF(ppb_opengles2_GetProgramiv),
    .GetProgramInfoLog =        TWRAPF(ppb_opengles2_GetProgramInfoLog),
    .GetRenderbufferParameteriv = TWRAPF(ppb_opengles2_GetRenderbufferParameteriv),
    .GetShaderiv =              TWRAPF(ppb_opengles2_GetShaderiv),
    .GetShaderInfoLog =         TWRAPF(ppb_opengles2_GetShaderInfoLog),
    .GetShaderPrecisionFormat = TWRAPF(ppb_opengles2_GetShaderPrecisionFormat),
    .GetShaderSource =          TWRAPF(ppb_opengles2_GetShaderSource),
    .GetString =                TWRAPF(ppb_opengles2_GetString),
    .GetTexParameterfv =        TWRAPF(ppb_opengles2_GetTexParameterfv),
    .GetTexParameteriv =        TWRAPF(ppb_opengles2_GetTexParameteriv),
    .GetUniformfv =             TWRAPF(ppb_opengles2_GetUniformfv),
    .GetUniformiv =             TWRAPF(ppb_opengles2_GetUniformiv),
    .GetUniformLocation =       TWRAPF(ppb_opengles2_GetUniformLocation),
    .GetVertexAttribfv =        TWRAPF(ppb_opengles2_GetVertexAttribfv),
    .GetVertexAttribiv =        TWRAPF(ppb_opengles2_GetVertexAttribiv),
    .GetVertexAttribPointerv =  TWRAPF(ppb_opengles2_GetVertexAttribPointerv),
    .Hint =                     TWRAPF(ppb_opengles2_Hint),
    .IsBuffer =                 TWRAPF(ppb_opengles2_IsBuffer),
    .IsEnabled =                TWRAPF(ppb_opengles2_IsEnabled),
    .IsFramebuffer =            TWRAPF(ppb_opengles2_IsFramebuffer),
    .IsProgram =                TWRAPF(ppb_opengles2_IsProgram),
    .IsRenderbuffer =           TWRAPF(ppb_opengles2_IsRenderbuffer),
    .IsShader =                 TWRAPF(ppb_opengles2_IsShader),
    .IsTexture =                TWRAPF(ppb_opengles2_IsTexture),
    .LineWidth =                TWRAPF(ppb_opengles2_LineWidth),
    .LinkProgram =              TWRAPF(ppb_opengles2_LinkProgram),
    .PixelStorei =              TWRAPF(ppb_opengles2_PixelStorei),
    .PolygonOffset =            TWRAPF(ppb_opengles2_PolygonOffset),
    .ReadPixels =               TWRAPF(ppb_opengles2_ReadPixels),
    .ReleaseShaderCompiler =    TWRAPF(ppb_opengles2_ReleaseShaderCompiler),
    .RenderbufferStorage =      TWRAPF(ppb_opengles2_RenderbufferStorage),
    .SampleCoverage =           TWRAPF(ppb_opengles2_SampleCoverage),
    .Scissor =                  TWRAPF(ppb_opengles2_Scissor),
    .ShaderBinary =             TWRAPF(ppb_opengles2_ShaderBinary),
    .ShaderSource =             TWRAPF(ppb_opengles2_ShaderSource),
    .StencilFunc =              TWRAPF(ppb_opengles2_StencilFunc),
    .StencilFuncSeparate =      TWRAPF(ppb_opengles2_StencilFuncSeparate),
    .StencilMask =              TWRAPF(ppb_opengles2_StencilMask),
    .StencilMaskSeparate =      TWRAPF(ppb_opengles2_StencilMaskSeparate),
    .StencilOp =                TWRAPF(ppb_opengles2_StencilOp),
    .StencilOpSeparate =        TWRAPF(ppb_opengles2_StencilOpSeparate),
    .TexImage2D =               TWRAPF(ppb_opengles2_TexImage2D),
    .TexParameterf =            TWRAPF(ppb_opengles2_TexParameterf),
    .TexParameterfv =           TWRAPF(ppb_opengles2_TexParameterfv),
    .TexParameteri =            TWRAPF(ppb_opengles2_TexParameteri),
    .TexParameteriv =           TWRAPF(ppb_opengles2_TexParameteriv),
    .TexSubImage2D =            TWRAPF(ppb_opengles2_TexSubImage2D),
    .Uniform1f =                TWRAPF(ppb_opengles2_Uniform1f),
    .Uniform1fv =               TWRAPF(ppb_opengles2_Uniform1fv),
    .Uniform1i =                TWRAPF(ppb_opengles2_Uniform1i),
    .Uniform1iv =               TWRAPF(ppb_opengles2_Uniform1iv),
    .Uniform2f =                TWRAPF(ppb_opengles2_Uniform2f),
    .Uniform2fv =               TWRAPF(ppb_opengles2_Uniform2fv),
    .Uniform2i =                TWRAPF(ppb_opengles2_Uniform2i),
    .Uniform2iv =               TWRAPF(ppb_opengles2_Uniform2iv),
    .Uniform3f =                TWRAPF(ppb_opengles2_Uniform3f),
    .Uniform3fv =               TWRAPF(ppb_opengles2_Uniform3fv),
    .Uniform3i =                TWRAPF(ppb_opengles2_Uniform3i),
    .Uniform3iv =               TWRAPF(ppb_opengles2_Uniform3iv),
    .Uniform4f =                TWRAPF(ppb_opengles2_Uniform4f),
    .Uniform4fv =               TWRAPF(ppb_opengles2_Uniform4fv),
    .Uniform4i =                TWRAPF(ppb_opengles2_Uniform4i),
    .Uniform4iv =               TWRAPF(ppb_opengles2_Uniform4iv),
    .UniformMatrix2fv =         TWRAPF(ppb_opengles2_UniformMatrix2fv),
    .UniformMatrix3fv =         TWRAPF(ppb_opengles2_UniformMatrix3fv),
    .UniformMatrix4fv =         TWRAPF(ppb_opengles2_UniformMatrix4fv),
    .UseProgram =               TWRAPF(ppb_opengles2_UseProgram),
    .ValidateProgram =          TWRAPF(ppb_opengles2_ValidateProgram),
    .VertexAttrib1f =           TWRAPF(ppb_opengles2_VertexAttrib1f),
    .VertexAttrib1fv =          TWRAPF(ppb_opengles2_VertexAttrib1fv),
    .VertexAttrib2f =           TWRAPF(ppb_opengles2_VertexAttrib2f),
    .VertexAttrib2fv =          TWRAPF(ppb_opengles2_VertexAttrib2fv),
    .VertexAttrib3f =           TWRAPF(ppb_opengles2_VertexAttrib3f),
    .VertexAttrib3fv =          TWRAPF(ppb_opengles2_VertexAttrib3fv),
    .VertexAttrib4f =           TWRAPF(ppb_opengles2_VertexAttrib4f),
    .VertexAttrib4fv =          TWRAPF(ppb_opengles2_VertexAttrib4fv),
    .VertexAttribPointer =      TWRAPF(ppb_opengles2_VertexAttribPointer),
    .Viewport =                 TWRAPF(ppb_opengles2_Viewport),
};

const struct PPB_OpenGLES2ChromiumEnableFeature ppb_opengles2_chromium_enable_feature_interface_1_0 = {
    .EnableFeatureCHROMIUM = TWRAPZ(ppb_opengles2_chromium_enable_feature_enable_feature_chromium),
};

const struct PPB_OpenGLES2ChromiumMapSub ppb_opengles2_chromium_map_sub_interface_1_0 = {
    .MapBufferSubDataCHROMIUM =   TWRAPZ(ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium),
    .UnmapBufferSubDataCHROMIUM = TWRAPZ(ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium),
    .MapTexSubImage2DCHROMIUM =   TWRAPF(ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium),
    .UnmapTexSubImage2DCHROMIUM = TWRAPF(ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium),
};

const struct PPB_OpenGLES2FramebufferBlit ppb_opengles2_framebuffer_blit_interface_1_0 = {
    .BlitFramebufferEXT = TWRAPZ(ppb_opengles2_framebuffer_blit_blit_framebuffer_ext),
};

const struct PPB_OpenGLES2FramebufferMultisample ppb_opengles2_framebuffer_multisample_interface_1_0 = {
    .RenderbufferStorageMultisampleEXT =
                TWRAPZ(ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext),
};

const struct PPB_OpenGLES2InstancedArrays ppb_opengles2_instanced_arrays_interface_1_0 = {
    .DrawArraysInstancedANGLE = TWRAPZ(ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle),
    .DrawElementsInstancedANGLE =TWRAPZ(ppb_opengles2_instanced_arrays_draw_elements_instanced_angle),
    .VertexAttribDivisorANGLE = TWRAPZ(ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle),

};

const struct PPB_OpenGLES2Query ppb_opengles2_query_interface_1_0 = {
    .GenQueriesEXT =        TWRAPZ(ppb_opengles2_query_gen_queries_ext),
    .DeleteQueriesEXT =     TWRAPZ(ppb_opengles2_query_delete_queries_ext),
    .IsQueryEXT =           TWRAPZ(ppb_opengles2_query_is_query_ext),
    .BeginQueryEXT =        TWRAPZ(ppb_opengles2_query_begin_query_ext),
    .EndQueryEXT =          TWRAPZ(ppb_opengles2_query_end_query_ext),
    .GetQueryivEXT =        TWRAPZ(ppb_opengles2_query_get_queryiv_ext),
    .GetQueryObjectuivEXT = TWRAPZ(ppb_opengles2_query_get_query_objectuiv_ext),
};

static
void
__attribute__((constructor))
constructor_ppb_opengles2(void)
{
#if !HAVE_GLES2
    shader_type_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    shader_source_ht = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, g_free);
#endif

    register_interface(PPB_OPENGLES2_CHROMIUMENABLEFEATURE_INTERFACE_1_0,
                       &ppb_opengles2_chromium_enable_feature_interface_1_0);
    register_interface(PPB_OPENGLES2_CHROMIUMMAPSUB_INTERFACE_1_0,
                       &ppb_opengles2_chromium_map_sub_interface_1_0);
    register_interface(PPB_OPENGLES2_FRAMEBUFFERBLIT_INTERFACE_1_0,
                       &ppb_opengles2_framebuffer_blit_interface_1_0);
    register_interface(PPB_OPENGLES2_FRAMEBUFFERMULTISAMPLE_INTERFACE_1_0,
                       &ppb_opengles2_framebuffer_multisample_interface_1_0);
    register_interface(PPB_OPENGLES2_INSTANCEDARRAYS_INTERFACE_1_0,
                       &ppb_opengles2_instanced_arrays_interface_1_0);
    register_interface(PPB_OPENGLES2_INTERFACE_1_0, &ppb_opengles2_interface_1_0);
    register_interface(PPB_OPENGLES2_QUERY_INTERFACE_1_0, &ppb_opengles2_query_interface_1_0);
}
