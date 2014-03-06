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
void
trace_ppb_opengles2_query_gen_queries_ext(PP_Resource context, GLsizei n, GLuint *queries)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_gen_queries_ext(context, n, queries);
}

void
trace_ppb_opengles2_query_delete_queries_ext(PP_Resource context, GLsizei n, const GLuint *queries)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_delete_queries_ext(context, n, queries);
}

GLboolean
trace_ppb_opengles2_query_is_query_ext(PP_Resource context, GLuint id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_opengles2_query_is_query_ext(context, id);
}

void
trace_ppb_opengles2_query_begin_query_ext(PP_Resource context, GLenum target, GLuint id)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_begin_query_ext(context, target, id);
}

void
trace_ppb_opengles2_query_end_query_ext(PP_Resource context, GLenum target)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_end_query_ext(context, target);
}

void
trace_ppb_opengles2_query_get_queryiv_ext(PP_Resource context, GLenum target, GLenum pname,
                                          GLint *params)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_get_queryiv_ext(context, target, pname, params);
}

void
trace_ppb_opengles2_query_get_query_objectuiv_ext(PP_Resource context, GLuint id, GLenum pname,
                                                  GLuint *params)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    ppb_opengles2_query_get_query_objectuiv_ext(context, id, pname, params);
}


const struct PPB_OpenGLES2Query ppb_opengles2_query_interface_1_0 = {
    .GenQueriesEXT =        trace_ppb_opengles2_query_gen_queries_ext,
    .DeleteQueriesEXT =     trace_ppb_opengles2_query_delete_queries_ext,
    .IsQueryEXT =           trace_ppb_opengles2_query_is_query_ext,
    .BeginQueryEXT =        trace_ppb_opengles2_query_begin_query_ext,
    .EndQueryEXT =          trace_ppb_opengles2_query_end_query_ext,
    .GetQueryivEXT =        trace_ppb_opengles2_query_get_queryiv_ext,
    .GetQueryObjectuivEXT = trace_ppb_opengles2_query_get_query_objectuiv_ext,
};
