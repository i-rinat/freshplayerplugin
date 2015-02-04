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

#ifndef FPP_PPB_OPENGLES2_H
#define FPP_PPB_OPENGLES2_H

#include <GLES2/gl2.h>
#include <ppapi/c/ppb_opengles2.h>


void
ppb_opengles2_ActiveTexture(PP_Resource context, GLenum texture);

void
ppb_opengles2_AttachShader(PP_Resource context, GLuint program, GLuint shader);

void
ppb_opengles2_BindAttribLocation(PP_Resource context, GLuint program, GLuint index,
                                 const char *name);

void
ppb_opengles2_BindBuffer(PP_Resource context, GLenum target, GLuint buffer);

void
ppb_opengles2_BindFramebuffer(PP_Resource context, GLenum target, GLuint framebuffer);

void
ppb_opengles2_BindRenderbuffer(PP_Resource context, GLenum target, GLuint renderbuffer);

void
ppb_opengles2_BindTexture(PP_Resource context, GLenum target, GLuint texture);

void
ppb_opengles2_BlendColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha);

void
ppb_opengles2_BlendEquation(PP_Resource context, GLenum mode);

void
ppb_opengles2_BlendEquationSeparate(PP_Resource context, GLenum modeRGB, GLenum modeAlpha);

void
ppb_opengles2_BlendFunc(PP_Resource context, GLenum sfactor, GLenum dfactor);

void
ppb_opengles2_BlendFuncSeparate(PP_Resource context, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha,
                                GLenum dstAlpha);

void
ppb_opengles2_BufferData(PP_Resource context, GLenum target, GLsizeiptr size, const void *data,
                         GLenum usage);

void
ppb_opengles2_BufferSubData(PP_Resource context, GLenum target, GLintptr offset, GLsizeiptr size,
                            const void *data);

GLenum
ppb_opengles2_CheckFramebufferStatus(PP_Resource context, GLenum target);

void
ppb_opengles2_Clear(PP_Resource context, GLbitfield mask);

void
ppb_opengles2_ClearColor(PP_Resource context, GLclampf red, GLclampf green, GLclampf blue,
                         GLclampf alpha);

void
ppb_opengles2_ClearDepthf(PP_Resource context, GLclampf depth);

void
ppb_opengles2_ClearStencil(PP_Resource context, GLint s);

void
ppb_opengles2_ColorMask(PP_Resource context, GLboolean red, GLboolean green, GLboolean blue,
                        GLboolean alpha);

void
ppb_opengles2_CompileShader(PP_Resource context, GLuint shader);

void
ppb_opengles2_CompressedTexImage2D(PP_Resource context, GLenum target, GLint level,
                                   GLenum internalformat, GLsizei width, GLsizei height,
                                   GLint border, GLsizei imageSize, const void *data);

void
ppb_opengles2_CompressedTexSubImage2D(PP_Resource context, GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
                                      GLenum format, GLsizei imageSize, const void *data);

void
ppb_opengles2_CopyTexImage2D(PP_Resource context, GLenum target, GLint level, GLenum internalformat,
                             GLint x, GLint y, GLsizei width, GLsizei height, GLint border);

void
ppb_opengles2_CopyTexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                                GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);

GLuint
ppb_opengles2_CreateProgram(PP_Resource context);

GLuint
ppb_opengles2_CreateShader(PP_Resource context, GLenum type);

void
ppb_opengles2_CullFace(PP_Resource context, GLenum mode);

void
ppb_opengles2_DeleteBuffers(PP_Resource context, GLsizei n, const GLuint *buffers);

void
ppb_opengles2_DeleteFramebuffers(PP_Resource context, GLsizei n, const GLuint *framebuffers);

void
ppb_opengles2_DeleteProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_DeleteRenderbuffers(PP_Resource context, GLsizei n, const GLuint *renderbuffers);

void
ppb_opengles2_DeleteShader(PP_Resource context, GLuint shader);

void
ppb_opengles2_DeleteTextures(PP_Resource context, GLsizei n, const GLuint *textures);

void
ppb_opengles2_DepthFunc(PP_Resource context, GLenum func);

void
ppb_opengles2_DepthMask(PP_Resource context, GLboolean flag);

void
ppb_opengles2_DepthRangef(PP_Resource context, GLclampf zNear, GLclampf zFar);

void
ppb_opengles2_DetachShader(PP_Resource context, GLuint program, GLuint shader);

void
ppb_opengles2_Disable(PP_Resource context, GLenum cap);

void
ppb_opengles2_DisableVertexAttribArray(PP_Resource context, GLuint index);

void
ppb_opengles2_DrawArrays(PP_Resource context, GLenum mode, GLint first, GLsizei count);

void
ppb_opengles2_DrawElements(PP_Resource context, GLenum mode, GLsizei count, GLenum type,
                           const void *indices);

void
ppb_opengles2_Enable(PP_Resource context, GLenum cap);

void
ppb_opengles2_EnableVertexAttribArray(PP_Resource context, GLuint index);

void
ppb_opengles2_Finish(PP_Resource context);

void
ppb_opengles2_Flush(PP_Resource context);

void
ppb_opengles2_FramebufferRenderbuffer(PP_Resource context, GLenum target, GLenum attachment,
                                      GLenum renderbuffertarget, GLuint renderbuffer);

void
ppb_opengles2_FramebufferTexture2D(PP_Resource context, GLenum target, GLenum attachment,
                                   GLenum textarget, GLuint texture, GLint level);

void
ppb_opengles2_FrontFace(PP_Resource context, GLenum mode);

void
ppb_opengles2_GenBuffers(PP_Resource context, GLsizei n, GLuint *buffers);

void
ppb_opengles2_GenerateMipmap(PP_Resource context, GLenum target);

void
ppb_opengles2_GenFramebuffers(PP_Resource context, GLsizei n, GLuint *framebuffers);

void
ppb_opengles2_GenRenderbuffers(PP_Resource context, GLsizei n, GLuint *renderbuffers);

void
ppb_opengles2_GenTextures(PP_Resource context, GLsizei n, GLuint *textures);

void
ppb_opengles2_GetActiveAttrib(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                              GLsizei *length, GLint *size, GLenum *type, char *name);

void
ppb_opengles2_GetActiveUniform(PP_Resource context, GLuint program, GLuint index, GLsizei bufsize,
                               GLsizei *length, GLint *size, GLenum *type, char *name);

void
ppb_opengles2_GetAttachedShaders(PP_Resource context, GLuint program, GLsizei maxcount,
                                 GLsizei *count, GLuint *shaders);

GLint
ppb_opengles2_GetAttribLocation(PP_Resource context, GLuint program, const char *name);

void
ppb_opengles2_GetBooleanv(PP_Resource context, GLenum pname, GLboolean *params);

void
ppb_opengles2_GetBufferParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params);

GLenum
ppb_opengles2_GetError(PP_Resource context);

void
ppb_opengles2_GetFloatv(PP_Resource context, GLenum pname, GLfloat *params);

void
ppb_opengles2_GetFramebufferAttachmentParameteriv(PP_Resource context, GLenum target,
                                                  GLenum attachment, GLenum pname, GLint *params);

void
ppb_opengles2_GetIntegerv(PP_Resource context, GLenum pname, GLint *params);

void
ppb_opengles2_GetProgramiv(PP_Resource context, GLuint program, GLenum pname, GLint *params);

void
ppb_opengles2_GetProgramInfoLog(PP_Resource context, GLuint program, GLsizei bufsize,
                                GLsizei *length, char *infolog);

void
ppb_opengles2_GetRenderbufferParameteriv(PP_Resource context, GLenum target, GLenum pname,
                                         GLint *params);

void
ppb_opengles2_GetShaderiv(PP_Resource context, GLuint shader, GLenum pname, GLint *params);

void
ppb_opengles2_GetShaderInfoLog(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                               char *infolog);

void
ppb_opengles2_GetShaderPrecisionFormat(PP_Resource context, GLenum shadertype, GLenum precisiontype,
                                       GLint *range, GLint *precision);

void
ppb_opengles2_GetShaderSource(PP_Resource context, GLuint shader, GLsizei bufsize, GLsizei *length,
                              char *source);

const GLubyte *
ppb_opengles2_GetString(PP_Resource context, GLenum name);

void
ppb_opengles2_GetTexParameterfv(PP_Resource context, GLenum target, GLenum pname, GLfloat *params);

void
ppb_opengles2_GetTexParameteriv(PP_Resource context, GLenum target, GLenum pname, GLint *params);

void
ppb_opengles2_GetUniformfv(PP_Resource context, GLuint program, GLint location, GLfloat *params);

void
ppb_opengles2_GetUniformiv(PP_Resource context, GLuint program, GLint location, GLint *params);

GLint
ppb_opengles2_GetUniformLocation(PP_Resource context, GLuint program, const char *name);

void
ppb_opengles2_GetVertexAttribfv(PP_Resource context, GLuint index, GLenum pname, GLfloat *params);

void
ppb_opengles2_GetVertexAttribiv(PP_Resource context, GLuint index, GLenum pname, GLint *params);

void
ppb_opengles2_GetVertexAttribPointerv(PP_Resource context, GLuint index, GLenum pname,
                                      void **pointer);

void
ppb_opengles2_Hint(PP_Resource context, GLenum target, GLenum mode);

GLboolean
ppb_opengles2_IsBuffer(PP_Resource context, GLuint buffer);

GLboolean
ppb_opengles2_IsEnabled(PP_Resource context, GLenum cap);

GLboolean
ppb_opengles2_IsFramebuffer(PP_Resource context, GLuint framebuffer);

GLboolean
ppb_opengles2_IsProgram(PP_Resource context, GLuint program);

GLboolean
ppb_opengles2_IsRenderbuffer(PP_Resource context, GLuint renderbuffer);

GLboolean
ppb_opengles2_IsShader(PP_Resource context, GLuint shader);

GLboolean
ppb_opengles2_IsTexture(PP_Resource context, GLuint texture);

void
ppb_opengles2_LineWidth(PP_Resource context, GLfloat width);

void
ppb_opengles2_LinkProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_PixelStorei(PP_Resource context, GLenum pname, GLint param);

void
ppb_opengles2_PolygonOffset(PP_Resource context, GLfloat factor, GLfloat units);

void
ppb_opengles2_ReadPixels(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height,
                         GLenum format, GLenum type, void *pixels);

void
ppb_opengles2_ReleaseShaderCompiler(PP_Resource context);

void
ppb_opengles2_RenderbufferStorage(PP_Resource context, GLenum target, GLenum internalformat,
                                  GLsizei width, GLsizei height);

void
ppb_opengles2_SampleCoverage(PP_Resource context, GLclampf value, GLboolean invert);

void
ppb_opengles2_Scissor(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height);

void
ppb_opengles2_ShaderBinary(PP_Resource context, GLsizei n, const GLuint *shaders,
                           GLenum binaryformat, const void *binary, GLsizei length);

void
ppb_opengles2_ShaderSource(PP_Resource context, GLuint shader, GLsizei count, const char **str,
                           const GLint *length);

void
ppb_opengles2_StencilFunc(PP_Resource context, GLenum func, GLint ref, GLuint mask);

void
ppb_opengles2_StencilFuncSeparate(PP_Resource context, GLenum face, GLenum func, GLint ref,
                                  GLuint mask);

void
ppb_opengles2_StencilMask(PP_Resource context, GLuint mask);

void
ppb_opengles2_StencilMaskSeparate(PP_Resource context, GLenum face, GLuint mask);

void
ppb_opengles2_StencilOp(PP_Resource context, GLenum fail, GLenum zfail, GLenum zpass);

void
ppb_opengles2_StencilOpSeparate(PP_Resource context, GLenum face, GLenum fail, GLenum zfail,
                                GLenum zpass);

void
ppb_opengles2_TexImage2D(PP_Resource context, GLenum target, GLint level, GLint internalformat,
                         GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                         const void *pixels);

void
ppb_opengles2_TexParameterf(PP_Resource context, GLenum target, GLenum pname, GLfloat param);

void
ppb_opengles2_TexParameterfv(PP_Resource context, GLenum target, GLenum pname,
                             const GLfloat *params);

void
ppb_opengles2_TexParameteri(PP_Resource context, GLenum target, GLenum pname, GLint param);

void
ppb_opengles2_TexParameteriv(PP_Resource context, GLenum target, GLenum pname, const GLint *params);

void
ppb_opengles2_TexSubImage2D(PP_Resource context, GLenum target, GLint level, GLint xoffset,
                            GLint yoffset, GLsizei width, GLsizei height, GLenum format,
                            GLenum type, const void *pixels);

void
ppb_opengles2_Uniform1f(PP_Resource context, GLint location, GLfloat x);

void
ppb_opengles2_Uniform1fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v);

void
ppb_opengles2_Uniform1i(PP_Resource context, GLint location, GLint x);

void
ppb_opengles2_Uniform1iv(PP_Resource context, GLint location, GLsizei count, const GLint *v);

void
ppb_opengles2_Uniform2f(PP_Resource context, GLint location, GLfloat x, GLfloat y);

void
ppb_opengles2_Uniform2fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v);

void
ppb_opengles2_Uniform2i(PP_Resource context, GLint location, GLint x, GLint y);

void
ppb_opengles2_Uniform2iv(PP_Resource context, GLint location, GLsizei count, const GLint *v);

void
ppb_opengles2_Uniform3f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z);

void
ppb_opengles2_Uniform3fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v);

void
ppb_opengles2_Uniform3i(PP_Resource context, GLint location, GLint x, GLint y, GLint z);

void
ppb_opengles2_Uniform3iv(PP_Resource context, GLint location, GLsizei count, const GLint *v);

void
ppb_opengles2_Uniform4f(PP_Resource context, GLint location, GLfloat x, GLfloat y, GLfloat z,
                        GLfloat w);

void
ppb_opengles2_Uniform4fv(PP_Resource context, GLint location, GLsizei count, const GLfloat *v);

void
ppb_opengles2_Uniform4i(PP_Resource context, GLint location, GLint x, GLint y, GLint z, GLint w);

void
ppb_opengles2_Uniform4iv(PP_Resource context, GLint location, GLsizei count, const GLint *v);

void
ppb_opengles2_UniformMatrix2fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value);

void
ppb_opengles2_UniformMatrix3fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value);

void
ppb_opengles2_UniformMatrix4fv(PP_Resource context, GLint location, GLsizei count,
                               GLboolean transpose, const GLfloat *value);

void
ppb_opengles2_UseProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_ValidateProgram(PP_Resource context, GLuint program);

void
ppb_opengles2_VertexAttrib1f(PP_Resource context, GLuint indx, GLfloat x);

void
ppb_opengles2_VertexAttrib1fv(PP_Resource context, GLuint indx, const GLfloat *values);

void
ppb_opengles2_VertexAttrib2f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y);

void
ppb_opengles2_VertexAttrib2fv(PP_Resource context, GLuint indx, const GLfloat *values);

void
ppb_opengles2_VertexAttrib3f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z);

void
ppb_opengles2_VertexAttrib3fv(PP_Resource context, GLuint indx, const GLfloat *values);

void
ppb_opengles2_VertexAttrib4f(PP_Resource context, GLuint indx, GLfloat x, GLfloat y, GLfloat z,
                             GLfloat w);

void
ppb_opengles2_VertexAttrib4fv(PP_Resource context, GLuint indx, const GLfloat *values);

void
ppb_opengles2_VertexAttribPointer(PP_Resource context, GLuint indx, GLint size, GLenum type,
                                  GLboolean normalized, GLsizei stride, const void *ptr);

void
ppb_opengles2_Viewport(PP_Resource context, GLint x, GLint y, GLsizei width, GLsizei height);


GLboolean
ppb_opengles2_chromium_enable_feature_enable_feature_chromium(PP_Resource context,
                                                              const char *feature);

void *
ppb_opengles2_chromium_map_sub_map_buffer_sub_data_chromium(PP_Resource context, GLuint target,
                                                            GLintptr offset, GLsizeiptr size,
                                                            GLenum access);

void
ppb_opengles2_chromium_map_sub_unmap_buffer_sub_data_chromium(PP_Resource context, const void *mem);

void *
ppb_opengles2_chromium_map_sub_map_tex_sub_image_2d_chromium(PP_Resource context, GLenum target,
                                                             GLint level, GLint xoffset,
                                                             GLint yoffset, GLsizei width,
                                                             GLsizei height, GLenum format,
                                                             GLenum type, GLenum access);

void
ppb_opengles2_chromium_map_sub_unmap_tex_sub_image_2d_chromium(PP_Resource context,
                                                               const void *mem);

void
ppb_opengles2_framebuffer_blit_blit_framebuffer_ext(PP_Resource context, GLint srcX0, GLint srcY0,
                                                    GLint srcX1, GLint srcY1, GLint dstX0,
                                                    GLint dstY0, GLint dstX1, GLint dstY1,
                                                    GLbitfield mask, GLenum filter);

void
ppb_opengles2_framebuffer_multisample_renderbuffer_storage_multisample_ext
                    (PP_Resource context, GLenum target, GLsizei samples, GLenum internalformat,
                     GLsizei width, GLsizei height);

void
ppb_opengles2_instanced_arrays_draw_arrays_instanced_angle(PP_Resource context, GLenum mode,
                                                           GLint first, GLsizei count,
                                                           GLsizei primcount);

void
ppb_opengles2_instanced_arrays_draw_elements_instanced_angle(PP_Resource context, GLenum mode,
                                                             GLsizei count, GLenum type,
                                                             const void *indices,GLsizei primcount);

void
ppb_opengles2_instanced_arrays_vertex_attrib_divisor_angle(PP_Resource context, GLuint index,
                                                           GLuint divisor);

void
ppb_opengles2_query_gen_queries_ext(PP_Resource context, GLsizei n, GLuint *queries);

void
ppb_opengles2_query_delete_queries_ext(PP_Resource context, GLsizei n, const GLuint *queries);

GLboolean
ppb_opengles2_query_is_query_ext(PP_Resource context, GLuint id);

void
ppb_opengles2_query_begin_query_ext(PP_Resource context, GLenum target, GLuint id);

void
ppb_opengles2_query_end_query_ext(PP_Resource context, GLenum target);

void
ppb_opengles2_query_get_queryiv_ext(PP_Resource context, GLenum target, GLenum pname,GLint *params);

void
ppb_opengles2_query_get_query_objectuiv_ext(PP_Resource context, GLuint id, GLenum pname,
                                            GLuint *params);

#endif // FPP_PPB_OPENGLES2_H
