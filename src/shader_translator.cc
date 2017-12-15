/*
 * Copyright © 2013-2017  Rinat Ibragimov
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

#include "shader_translator.h"
#include <GLSLANG/ShaderLang.h>
#include <glib.h>
#include <string>

static
void
__attribute__((constructor))
constructor_shader_translator(void)
{
    ShInitialize();
}

static
void
__attribute__((destructor))
destructor_shader_translator(void)
{
    ShFinalize();
}

char *
translate_shader(GLenum type, const char *str)
{
    ShBuiltInResources resources;

    ShInitBuiltInResources(&resources);
    resources.MaxVertexAttribs =             8;
    resources.MaxVertexUniformVectors =      128;
    resources.MaxVaryingVectors =            8;
    resources.MaxVertexTextureImageUnits =   0;
    resources.MaxCombinedTextureImageUnits = 8;
    resources.MaxTextureImageUnits =         8;
    resources.MaxFragmentUniformVectors =    16;
    resources.MaxDrawBuffers =               1;

    resources.OES_standard_derivatives =     0;
    resources.OES_EGL_image_external =       0;

    ShHandle compiler = ShConstructCompiler(type, SH_GLES2_SPEC, SH_GLSL_OUTPUT, &resources);

    ShCompile(compiler, &str, 1, SH_OBJECT_CODE);

    char *result = g_strdup(ShGetObjectCode(compiler).c_str());
    ShDestruct(compiler);
    return result;
}
