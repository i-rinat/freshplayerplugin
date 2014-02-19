#include <ppapi/c/ppb_opengles2.h>
#include <stddef.h>
#include "trace.h"


void
ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
}

void
ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
}

void
ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char* name)
{
}

void
ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
}

void
ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
}

void
ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
}

void
ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
}

void
ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
}

void
ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
}

void
ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
}

void
ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
}

void
ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha)
{
}

void
ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void* data,
                         GLenum usage)
{
}

void
ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void* data)
{
}

GLenum
ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    return 0;
}

void
ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
}

void
ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
}

void
ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
}

void
ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
}

void
ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha)
{
}

void
ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
}

void
ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void* data)
{
}

void
ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void* data)
{
}

void
ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
}

void
ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
}

GLuint
ppb_opengles2_CreateProgram(PP_Resource context)
{
    return 0;
}

GLuint
ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    return 0;
}

void
ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
}

void
ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint* buffers)
{
}

void
ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint* framebuffers)
{
}

void
ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
}

void
ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint* renderbuffers)
{
}

void
ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
}

void
ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint* textures)
{
}

void
ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
}

void
ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
}

void
ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
}

void
ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
}

void
ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
}

void
ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
}

void
ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
}

void
ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void* indices)
{
}

void
ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
}

void
ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
}

void
ppb_opengles2_Finish(PP_Resource context)
{
}

void
ppb_opengles2_Flush(PP_Resource context)
{
}

void
ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer)
{
}

void
ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level)
{
}

void
ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
}

void
ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint* buffers)
{
}

void
ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
}

void
ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint* framebuffers)
{
}

void
ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint* renderbuffers)
{
}

void
ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint* textures)
{
}

void
ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei* length, GLint* size, GLenum* type, char* name)
{
}

void
ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei* length, GLint* size, GLenum* type, char* name)
{
}

void
ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei* count, GLuint* shaders)
{
}

GLint
ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char* name)
{
    return 0;
}

void
ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean* params)
{
}

void
ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params)
{
}

GLenum
ppb_opengles2_GetError(PP_Resource context)
{
    return 0;
}

void
ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat* params)
{
}

void
ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei* length, char* infolog)
{
}

void
ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint* params)
{
}

void
ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                               char* infolog)
{
}

void
ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint* range, GLint* precision)
{
}

void
ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                              char* source)
{
}

const GLubyte*
ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    return (const GLubyte*)"";
}

void
ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat* params)
{
}

void
ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat* params)
{
}

void
ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint* params)
{
}

GLint
ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char* name)
{
    return 0;
}

void
ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat* params)
{
}

void
ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint* params)
{
}

void
ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void** pointer)
{
}

void
ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
}

GLboolean
ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    return 1;
}

GLboolean
ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    return 1;
}

GLboolean
ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    return 1;
}

GLboolean
ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    return 1;
}

GLboolean
ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    return 1;
}

GLboolean
ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    return 1;
}

GLboolean
ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    return 1;
}

void
ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
}

void
ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
}

void
ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
}

void
ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
}

void
ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void* pixels)
{
}

void
ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
}

void
ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height)
{
}

void
ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
}

void
ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
}

void
ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint* shaders,
                           GLenum binaryformat, const void* binary, GLsizei length)
{
}

void
ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char** str,
                           const GLint* length)
{
}

void
ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
}

void
ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask)
{
}

void
ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
}

void
ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
}

void
ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
}

void
ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass)
{
}

void
ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void* pixels)
{
}

void
ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
}

void
ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat* params)
{
}

void
ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
}

void
ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint* params)
{
}

void
ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void* pixels)
{
}

void
ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
}

void
ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
}

void
ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
}

void
ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
}

void
ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
}

void
ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
}

void
ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
}

void
ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
}

void
ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
}

void
ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
}

void
ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
}

void
ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
}

void
ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w)
{
}

void
ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
}

void
ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w)
{
}

void
ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
}

void
ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
}

void
ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
}

void
ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
}

void
ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
}

void
ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
}

void
ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
}

void
ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
}

void
ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
}

void
ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
}

void
ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
}

void
ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
}

void
ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w)
{
}

void
ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
}

void
ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void* ptr)
{
}

void
ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
}


// trace wrappers
static
void
trace_ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ActiveTexture(context, texture);
}

static
void
trace_ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_AttachShader(context, program, shader);
}

static
void
trace_ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char* name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BindAttribLocation(context, program, index, name);
}

static
void
trace_ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BindBuffer(context, target, buffer);
}

static
void
trace_ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BindFramebuffer(context, target, framebuffer);
}

static
void
trace_ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BindRenderbuffer(context, target, renderbuffer);
}

static
void
trace_ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BindTexture(context, target, texture);
}

static
void
trace_ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BlendColor(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BlendEquation(context, mode);
}

static
void
trace_ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BlendEquationSeparate(context, modeRGB, modeAlpha);
}

static
void
trace_ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BlendFunc(context, sfactor, dfactor);
}

static
void
trace_ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BlendFuncSeparate(context, srcRGB, dstRGB, srcAlpha, dstAlpha);
}

static
void
trace_ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void* data,
                         GLenum usage)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BufferData(context, target, size, data, usage);
}

static
void
trace_ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void* data)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_BufferSubData(context, target, offset, size, data);
}

static
GLenum
trace_ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_CheckFramebufferStatus(context, target);
}

static
void
trace_ppb_opengles2_Clear(PP_Resource context, GLbitfield mask)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Clear(context, mask);
}

static
void
trace_ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ClearColor(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ClearDepthf(context, depth);
}

static
void
trace_ppb_opengles2_ClearStencil(PP_Resource context, GLint s)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ClearStencil(context, s);
}

static
void
trace_ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ColorMask(context, red, green, blue, alpha);
}

static
void
trace_ppb_opengles2_CompileShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CompileShader(context, shader);
}

static
void
trace_ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void* data)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CompressedTexImage2D(context, target, level, internalformat, width, height,
                                       border, imageSize, data);
}

static
void
trace_ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void* data)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CompressedTexSubImage2D(context, target, level, xoffset, yoffset, width, height,
                                          format, imageSize, data);
}

static
void
trace_ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CopyTexImage2D(context, target, level, internalformat, x, y, width, height,
                                 border);
}

static
void
trace_ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CopyTexSubImage2D(context, target, level, xoffset, yoffset, x, y, width, height);
}

static
GLuint
trace_ppb_opengles2_CreateProgram(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_CreateProgram(context);
}

static
GLuint
trace_ppb_opengles2_CreateShader(PP_Resource context, GLenum type)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_CreateShader(context, type);
}

static
void
trace_ppb_opengles2_CullFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_CullFace(context, mode);
}

static
void
trace_ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint* buffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteBuffers(context, n, buffers);
}

static
void
trace_ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint* framebuffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteFramebuffers(context, n, framebuffers);
}

static
void
trace_ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteProgram(context, program);
}

static
void
trace_ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint* renderbuffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteRenderbuffers(context, n, renderbuffers);
}

static
void
trace_ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteShader(context, shader);
}

static
void
trace_ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint* textures)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DeleteTextures(context, n, textures);
}

static
void
trace_ppb_opengles2_DepthFunc(PP_Resource context, GLenum func)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DepthFunc(context, func);
}

static
void
trace_ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DepthMask(context, flag);
}

static
void
trace_ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DepthRangef(context, zNear, zFar);
}

static
void
trace_ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DetachShader(context, program, shader);
}

static
void
trace_ppb_opengles2_Disable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Disable(context, cap);
}

static
void
trace_ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DisableVertexAttribArray(context, index);
}

static
void
trace_ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DrawArrays(context, mode, first, count);
}

static
void
trace_ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void* indices)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_DrawElements(context, mode, count, type, indices);
}

static
void
trace_ppb_opengles2_Enable(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Enable(context, cap);
}

static
void
trace_ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_EnableVertexAttribArray(context, index);
}

static
void
trace_ppb_opengles2_Finish(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Finish(context);
}

static
void
trace_ppb_opengles2_Flush(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Flush(context);
}

static
void
trace_ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_FramebufferRenderbuffer(context, target, attachment, renderbuffertarget,
                                          renderbuffer);
}

static
void
trace_ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_FramebufferTexture2D(context, target, attachment, textarget, texture, level);
}

static
void
trace_ppb_opengles2_FrontFace(PP_Resource context, GLenum mode)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_FrontFace(context, mode);
}

static
void
trace_ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint* buffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GenBuffers(context, n, buffers);
}

static
void
trace_ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GenerateMipmap(context, target);
}

static
void
trace_ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint* framebuffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GenFramebuffers(context, n, framebuffers);
}

static
void
trace_ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint* renderbuffers)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GenRenderbuffers(context, n, renderbuffers);
}

static
void
trace_ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint* textures)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GenTextures(context, n, textures);
}

static
void
trace_ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei* length, GLint* size, GLenum* type, char* name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetActiveAttrib(context, program, index, bufsize, length, size, type, name);
}

static
void
trace_ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei* length, GLint* size, GLenum* type, char* name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetActiveUniform(context, program, index, bufsize, length, size, type, name);
}

static
void
trace_ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei* count, GLuint* shaders)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetAttachedShaders(context, program, maxcount, count, shaders);
}

static
GLint
trace_ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char* name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetAttribLocation(context, program, name);
}

static
void
trace_ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetBooleanv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetBufferParameteriv(context, target, pname, params);
}

static
GLenum
trace_ppb_opengles2_GetError(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetError(context);
}

static
void
trace_ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetFloatv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetFramebufferAttachmentParameteriv(context, target, attachment, pname, params);
}

static
void
trace_ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetIntegerv(context, pname, params);
}

static
void
trace_ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetProgramiv(context, program, pname, params);
}

static
void
trace_ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei* length, char* infolog)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetProgramInfoLog(context, program, bufsize, length, infolog);
}

static
void
trace_ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetRenderbufferParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetShaderiv(context, shader, pname, params);
}

static
void
trace_ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                               char* infolog)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetShaderInfoLog(context, shader, bufsize, length, infolog);
}

static
void
trace_ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint* range, GLint* precision)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetShaderPrecisionFormat(context, shadertype, precisiontype, range, precision);
}

static
void
trace_ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei* length,
                              char* source)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetShaderSource(context, shader, bufsize, length, source);
}

static
const GLubyte*
trace_ppb_opengles2_GetString(PP_Resource context, GLenum name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetString(context, name);
}

static
void
trace_ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetTexParameterfv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetTexParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetUniformfv(context, program, location, params);
}

static
void
trace_ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetUniformiv(context, program, location, params);
}

static
GLint
trace_ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char* name)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_GetUniformLocation(context, program, name);
}

static
void
trace_ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetVertexAttribfv(context, index, pname, params);
}

static
void
trace_ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetVertexAttribiv(context, index, pname, params);
}

static
void
trace_ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void** pointer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_GetVertexAttribPointerv(context, index, pname, pointer);
}

static
void
trace_ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Hint(context, target, mode);
}

static
GLboolean
trace_ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsBuffer(context, buffer);
}

static
GLboolean
trace_ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsEnabled(context, cap);
}

static
GLboolean
trace_ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsFramebuffer(context, framebuffer);
}

static
GLboolean
trace_ppb_opengles2_IsProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsProgram(context, program);
}

static
GLboolean
trace_ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsRenderbuffer(context, renderbuffer);
}

static
GLboolean
trace_ppb_opengles2_IsShader(PP_Resource context, GLuint shader)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsShader(context, shader);
}

static
GLboolean
trace_ppb_opengles2_IsTexture(PP_Resource context, GLuint texture)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    return ppb_opengles2_IsTexture(context, texture);
}

static
void
trace_ppb_opengles2_LineWidth(PP_Resource context, GLfloat width)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_LineWidth(context, width);
}

static
void
trace_ppb_opengles2_LinkProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_LinkProgram(context, program);
}

static
void
trace_ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_PixelStorei(context, pname, param);
}

static
void
trace_ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_PolygonOffset(context, factor, units);
}

static
void
trace_ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void* pixels)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ReadPixels(context, x, y, width, height, format, type, pixels);
}

static
void
trace_ppb_opengles2_ReleaseShaderCompiler(PP_Resource context)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ReleaseShaderCompiler(context);
}

static
void
trace_ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_RenderbufferStorage(context, target, internalformat, width, height);
}

static
void
trace_ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_SampleCoverage(context, value, invert);
}

static
void
trace_ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Scissor(context, x, y, width, height);
}

static
void
trace_ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint* shaders,
                           GLenum binaryformat, const void* binary, GLsizei length)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ShaderBinary(context, n, shaders, binaryformat, binary, length);
}

static
void
trace_ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char** str,
                           const GLint* length)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ShaderSource(context, shader, count, str, length);
}

static
void
trace_ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilFunc(context, func, ref, mask);
}

static
void
trace_ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilFuncSeparate(context, face, func, ref, mask);
}

static
void
trace_ppb_opengles2_StencilMask(PP_Resource context, GLuint mask)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilMask(context, mask);
}

static
void
trace_ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilMaskSeparate(context, face, mask);
}

static
void
trace_ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilOp(context, fail, zfail, zpass);
}

static
void
trace_ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_StencilOpSeparate(context, face, fail, zfail, zpass);
}

static
void
trace_ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void* pixels)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexImage2D(context, target, level, internalformat, width, height, border, format,
                             type, pixels);
}

static
void
trace_ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexParameterf(context, target, pname, param);
}

static
void
trace_ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexParameterfv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexParameteri(context, target, pname, param);
}

static
void
trace_ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint* params)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexParameteriv(context, target, pname, params);
}

static
void
trace_ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void* pixels)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_TexSubImage2D(context, target, level, xoffset, yoffset, width, height, format,
                                type, pixels);
}

static
void
trace_ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform1f(context, location, x);
}

static
void
trace_ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform1fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform1i(context, location, x);
}

static
void
trace_ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform1iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform2f(context, location, x, y);
}

static
void
trace_ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform2fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform2i(context, location, x, y);
}

static
void
trace_ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform2iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform3f(context, location, x, y, z);
}

static
void
trace_ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform3fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform3i(context, location, x, y, z);
}

static
void
trace_ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform3iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform4f(context, location, x, y, z, w);
}

static
void
trace_ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform4fv(context, location, count, v);
}

static
void
trace_ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform4i(context, location, x, y, z, w);
}

static
void
trace_ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint* v)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Uniform4iv(context, location, count, v);
}

static
void
trace_ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_UniformMatrix2fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_UniformMatrix3fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat* value)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_UniformMatrix4fv(context, location, count, transpose, value);
}

static
void
trace_ppb_opengles2_UseProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_UseProgram(context, program);
}

static
void
trace_ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_ValidateProgram(context, program);
}

static
void
trace_ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib1f(context, indx, x);
}

static
void
trace_ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib1fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib2f(context, indx, x, y);
}

static
void
trace_ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib2fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib3f(context, indx, x, y, z);
}

static
void
trace_ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib3fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib4f(context, indx, x, y, z, w);
}

static
void
trace_ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat* values)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttrib4fv(context, indx, values);
}

static
void
trace_ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void* ptr)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_VertexAttribPointer(context, indx, size, type, normalized, stride, ptr);
}

static
void
trace_ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height)
{
    trace_info("[PPB] {zilch} %s context=%d\n", __func__+6, context);
    ppb_opengles2_Viewport(context, x, y, width, height);
}


const struct PPB_OpenGLES2 ppb_opengles2_interface_1_0 = {
    .ActiveTexture =            trace_ppb_opengles2_ActiveTexture,
    .AttachShader =             trace_ppb_opengles2_AttachShader,
    .BindAttribLocation =       trace_ppb_opengles2_BindAttribLocation,
    .BindBuffer =               trace_ppb_opengles2_BindBuffer,
    .BindFramebuffer =          trace_ppb_opengles2_BindFramebuffer,
    .BindRenderbuffer =         trace_ppb_opengles2_BindRenderbuffer,
    .BindTexture =              trace_ppb_opengles2_BindTexture,
    .BlendColor =               trace_ppb_opengles2_BlendColor,
    .BlendEquation =            trace_ppb_opengles2_BlendEquation,
    .BlendEquationSeparate =    trace_ppb_opengles2_BlendEquationSeparate,
    .BlendFunc =                trace_ppb_opengles2_BlendFunc,
    .BlendFuncSeparate =        trace_ppb_opengles2_BlendFuncSeparate,
    .BufferData =               trace_ppb_opengles2_BufferData,
    .BufferSubData =            trace_ppb_opengles2_BufferSubData,
    .CheckFramebufferStatus =   trace_ppb_opengles2_CheckFramebufferStatus,
    .Clear =                    trace_ppb_opengles2_Clear,
    .ClearColor =               trace_ppb_opengles2_ClearColor,
    .ClearDepthf =              trace_ppb_opengles2_ClearDepthf,
    .ClearStencil =             trace_ppb_opengles2_ClearStencil,
    .ColorMask =                trace_ppb_opengles2_ColorMask,
    .CompileShader =            trace_ppb_opengles2_CompileShader,
    .CompressedTexImage2D =     trace_ppb_opengles2_CompressedTexImage2D,
    .CompressedTexSubImage2D =  trace_ppb_opengles2_CompressedTexSubImage2D,
    .CopyTexImage2D =           trace_ppb_opengles2_CopyTexImage2D,
    .CopyTexSubImage2D =        trace_ppb_opengles2_CopyTexSubImage2D,
    .CreateProgram =            trace_ppb_opengles2_CreateProgram,
    .CreateShader =             trace_ppb_opengles2_CreateShader,
    .CullFace =                 trace_ppb_opengles2_CullFace,
    .DeleteBuffers =            trace_ppb_opengles2_DeleteBuffers,
    .DeleteFramebuffers =       trace_ppb_opengles2_DeleteFramebuffers,
    .DeleteProgram =            trace_ppb_opengles2_DeleteProgram,
    .DeleteRenderbuffers =      trace_ppb_opengles2_DeleteRenderbuffers,
    .DeleteShader =             trace_ppb_opengles2_DeleteShader,
    .DeleteTextures =           trace_ppb_opengles2_DeleteTextures,
    .DepthFunc =                trace_ppb_opengles2_DepthFunc,
    .DepthMask =                trace_ppb_opengles2_DepthMask,
    .DepthRangef =              trace_ppb_opengles2_DepthRangef,
    .DetachShader =             trace_ppb_opengles2_DetachShader,
    .Disable =                  trace_ppb_opengles2_Disable,
    .DisableVertexAttribArray = trace_ppb_opengles2_DisableVertexAttribArray,
    .DrawArrays =               trace_ppb_opengles2_DrawArrays,
    .DrawElements =             trace_ppb_opengles2_DrawElements,
    .Enable =                   trace_ppb_opengles2_Enable,
    .EnableVertexAttribArray =  trace_ppb_opengles2_EnableVertexAttribArray,
    .Finish =                   trace_ppb_opengles2_Finish,
    .Flush =                    trace_ppb_opengles2_Flush,
    .FramebufferRenderbuffer =  trace_ppb_opengles2_FramebufferRenderbuffer,
    .FramebufferTexture2D =     trace_ppb_opengles2_FramebufferTexture2D,
    .FrontFace =                trace_ppb_opengles2_FrontFace,
    .GenBuffers =               trace_ppb_opengles2_GenBuffers,
    .GenerateMipmap =           trace_ppb_opengles2_GenerateMipmap,
    .GenFramebuffers =          trace_ppb_opengles2_GenFramebuffers,
    .GenRenderbuffers =         trace_ppb_opengles2_GenRenderbuffers,
    .GenTextures =              trace_ppb_opengles2_GenTextures,
    .GetActiveAttrib =          trace_ppb_opengles2_GetActiveAttrib,
    .GetActiveUniform =         trace_ppb_opengles2_GetActiveUniform,
    .GetAttachedShaders =       trace_ppb_opengles2_GetAttachedShaders,
    .GetAttribLocation =        trace_ppb_opengles2_GetAttribLocation,
    .GetBooleanv =              trace_ppb_opengles2_GetBooleanv,
    .GetBufferParameteriv =     trace_ppb_opengles2_GetBufferParameteriv,
    .GetError =                 trace_ppb_opengles2_GetError,
    .GetFloatv =                trace_ppb_opengles2_GetFloatv,
    .GetFramebufferAttachmentParameteriv = trace_ppb_opengles2_GetFramebufferAttachmentParameteriv,
    .GetIntegerv =              trace_ppb_opengles2_GetIntegerv,
    .GetProgramiv =             trace_ppb_opengles2_GetProgramiv,
    .GetProgramInfoLog =        trace_ppb_opengles2_GetProgramInfoLog,
    .GetRenderbufferParameteriv = trace_ppb_opengles2_GetRenderbufferParameteriv,
    .GetShaderiv =              trace_ppb_opengles2_GetShaderiv,
    .GetShaderInfoLog =         trace_ppb_opengles2_GetShaderInfoLog,
    .GetShaderPrecisionFormat = trace_ppb_opengles2_GetShaderPrecisionFormat,
    .GetShaderSource =          trace_ppb_opengles2_GetShaderSource,
    .GetString =                trace_ppb_opengles2_GetString,
    .GetTexParameterfv =        trace_ppb_opengles2_GetTexParameterfv,
    .GetTexParameteriv =        trace_ppb_opengles2_GetTexParameteriv,
    .GetUniformfv =             trace_ppb_opengles2_GetUniformfv,
    .GetUniformiv =             trace_ppb_opengles2_GetUniformiv,
    .GetUniformLocation =       trace_ppb_opengles2_GetUniformLocation,
    .GetVertexAttribfv =        trace_ppb_opengles2_GetVertexAttribfv,
    .GetVertexAttribiv =        trace_ppb_opengles2_GetVertexAttribiv,
    .GetVertexAttribPointerv =  trace_ppb_opengles2_GetVertexAttribPointerv,
    .Hint =                     trace_ppb_opengles2_Hint,
    .IsBuffer =                 trace_ppb_opengles2_IsBuffer,
    .IsEnabled =                trace_ppb_opengles2_IsEnabled,
    .IsFramebuffer =            trace_ppb_opengles2_IsFramebuffer,
    .IsProgram =                trace_ppb_opengles2_IsProgram,
    .IsRenderbuffer =           trace_ppb_opengles2_IsRenderbuffer,
    .IsShader =                 trace_ppb_opengles2_IsShader,
    .IsTexture =                trace_ppb_opengles2_IsTexture,
    .LineWidth =                trace_ppb_opengles2_LineWidth,
    .LinkProgram =              trace_ppb_opengles2_LinkProgram,
    .PixelStorei =              trace_ppb_opengles2_PixelStorei,
    .PolygonOffset =            trace_ppb_opengles2_PolygonOffset,
    .ReadPixels =               trace_ppb_opengles2_ReadPixels,
    .ReleaseShaderCompiler =    trace_ppb_opengles2_ReleaseShaderCompiler,
    .RenderbufferStorage =      trace_ppb_opengles2_RenderbufferStorage,
    .SampleCoverage =           trace_ppb_opengles2_SampleCoverage,
    .Scissor =                  trace_ppb_opengles2_Scissor,
    .ShaderBinary =             trace_ppb_opengles2_ShaderBinary,
    .ShaderSource =             trace_ppb_opengles2_ShaderSource,
    .StencilFunc =              trace_ppb_opengles2_StencilFunc,
    .StencilFuncSeparate =      trace_ppb_opengles2_StencilFuncSeparate,
    .StencilMask =              trace_ppb_opengles2_StencilMask,
    .StencilMaskSeparate =      trace_ppb_opengles2_StencilMaskSeparate,
    .StencilOp =                trace_ppb_opengles2_StencilOp,
    .StencilOpSeparate =        trace_ppb_opengles2_StencilOpSeparate,
    .TexImage2D =               trace_ppb_opengles2_TexImage2D,
    .TexParameterf =            trace_ppb_opengles2_TexParameterf,
    .TexParameterfv =           trace_ppb_opengles2_TexParameterfv,
    .TexParameteri =            trace_ppb_opengles2_TexParameteri,
    .TexParameteriv =           trace_ppb_opengles2_TexParameteriv,
    .TexSubImage2D =            trace_ppb_opengles2_TexSubImage2D,
    .Uniform1f =                trace_ppb_opengles2_Uniform1f,
    .Uniform1fv =               trace_ppb_opengles2_Uniform1fv,
    .Uniform1i =                trace_ppb_opengles2_Uniform1i,
    .Uniform1iv =               trace_ppb_opengles2_Uniform1iv,
    .Uniform2f =                trace_ppb_opengles2_Uniform2f,
    .Uniform2fv =               trace_ppb_opengles2_Uniform2fv,
    .Uniform2i =                trace_ppb_opengles2_Uniform2i,
    .Uniform2iv =               trace_ppb_opengles2_Uniform2iv,
    .Uniform3f =                trace_ppb_opengles2_Uniform3f,
    .Uniform3fv =               trace_ppb_opengles2_Uniform3fv,
    .Uniform3i =                trace_ppb_opengles2_Uniform3i,
    .Uniform3iv =               trace_ppb_opengles2_Uniform3iv,
    .Uniform4f =                trace_ppb_opengles2_Uniform4f,
    .Uniform4fv =               trace_ppb_opengles2_Uniform4fv,
    .Uniform4i =                trace_ppb_opengles2_Uniform4i,
    .Uniform4iv =               trace_ppb_opengles2_Uniform4iv,
    .UniformMatrix2fv =         trace_ppb_opengles2_UniformMatrix2fv,
    .UniformMatrix3fv =         trace_ppb_opengles2_UniformMatrix3fv,
    .UniformMatrix4fv =         trace_ppb_opengles2_UniformMatrix4fv,
    .UseProgram =               trace_ppb_opengles2_UseProgram,
    .ValidateProgram =          trace_ppb_opengles2_ValidateProgram,
    .VertexAttrib1f =           trace_ppb_opengles2_VertexAttrib1f,
    .VertexAttrib1fv =          trace_ppb_opengles2_VertexAttrib1fv,
    .VertexAttrib2f =           trace_ppb_opengles2_VertexAttrib2f,
    .VertexAttrib2fv =          trace_ppb_opengles2_VertexAttrib2fv,
    .VertexAttrib3f =           trace_ppb_opengles2_VertexAttrib3f,
    .VertexAttrib3fv =          trace_ppb_opengles2_VertexAttrib3fv,
    .VertexAttrib4f =           trace_ppb_opengles2_VertexAttrib4f,
    .VertexAttrib4fv =          trace_ppb_opengles2_VertexAttrib4fv,
    .VertexAttribPointer =      trace_ppb_opengles2_VertexAttribPointer,
    .Viewport =                 trace_ppb_opengles2_Viewport,
};
