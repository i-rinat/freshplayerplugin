//
// Copyright (c) 2002-2013 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#ifndef GLSLANG_SHADERLANG_H_
#define GLSLANG_SHADERLANG_H_

#if defined(COMPONENT_BUILD) && !defined(ANGLE_TRANSLATOR_STATIC)
#if defined(_WIN32) || defined(_WIN64)

#if defined(ANGLE_TRANSLATOR_IMPLEMENTATION)
#define COMPILER_EXPORT __declspec(dllexport)
#else
#define COMPILER_EXPORT __declspec(dllimport)
#endif  // defined(ANGLE_TRANSLATOR_IMPLEMENTATION)

#else  // defined(_WIN32) || defined(_WIN64)
#define COMPILER_EXPORT __attribute__((visibility("default")))
#endif

#else  // defined(COMPONENT_BUILD) && !defined(ANGLE_TRANSLATOR_STATIC)
#define COMPILER_EXPORT
#endif

#include <stddef.h>

#include "KHR/khrplatform.h"

#include <map>
#include <string>
#include <vector>

//
// This is the platform independent interface between an OGL driver
// and the shading language compiler.
//

namespace sh
{
// GLenum alias
typedef unsigned int GLenum;
}

// Version number for shader translation API.
// It is incremented every time the API changes.
#define ANGLE_SH_VERSION 139

typedef enum {
  SH_GLES2_SPEC = 0x8B40,
} ShShaderSpec;

typedef enum {
  // SH_GLSL_OUTPUT is deprecated. This is to not break the build.
  SH_GLSL_OUTPUT               = 0x8B46,
  SH_GLSL_COMPATIBILITY_OUTPUT = 0x8B46,
} ShShaderOutput;

// Compile options.
typedef enum {
  SH_OBJECT_CODE             = 0x0004,
} ShCompileOptions;

//
// Driver must call this first, once, before doing any other
// compiler operations.
// If the function succeeds, the return value is true, else false.
//
COMPILER_EXPORT bool ShInitialize();
//
// Driver should call this at shutdown.
// If the function succeeds, the return value is true, else false.
//
COMPILER_EXPORT bool ShFinalize();

// The 64 bits hash function. The first parameter is the input string; the
// second parameter is the string length.
typedef khronos_uint64_t (*ShHashFunction64)(const char*, size_t);

//
// Implementation dependent built-in resources (constants and extensions).
// The names for these resources has been obtained by stripping gl_/GL_.
//
typedef struct
{
    // Constants.
    int MaxVertexAttribs;
    int MaxVertexUniformVectors;
    int MaxVaryingVectors;
    int MaxVertexTextureImageUnits;
    int MaxCombinedTextureImageUnits;
    int MaxTextureImageUnits;
    int MaxFragmentUniformVectors;
    int MaxDrawBuffers;

    // Extensions.
    // Set to 1 to enable the extension, else 0.
    int OES_standard_derivatives;
    int OES_EGL_image_external;
    int ARB_texture_rectangle;
    int EXT_blend_func_extended;
    int EXT_draw_buffers;
    int EXT_frag_depth;
    int EXT_shader_texture_lod;
    int WEBGL_debug_shader_precision;
    int EXT_shader_framebuffer_fetch;
    int NV_shader_framebuffer_fetch;
    int ARM_shader_framebuffer_fetch;

    // Set to 1 to enable replacing GL_EXT_draw_buffers #extension directives
    // with GL_NV_draw_buffers in ESSL output. This flag can be used to emulate
    // EXT_draw_buffers by using it in combination with GLES3.0 glDrawBuffers
    // function. This applies to Tegra K1 devices.
    int NV_draw_buffers;

    // Set to 1 if highp precision is supported in the fragment language.
    // Default is 0.
    int FragmentPrecisionHigh;

    // GLSL ES 3.0 constants.
    int MaxVertexOutputVectors;
    int MaxFragmentInputVectors;
    int MinProgramTexelOffset;
    int MaxProgramTexelOffset;

    // Extension constants.

    // Value of GL_MAX_DUAL_SOURCE_DRAW_BUFFERS_EXT for OpenGL ES output context.
    // Value of GL_MAX_DUAL_SOURCE_DRAW_BUFFERS for OpenGL output context.
    // GLES SL version 100 gl_MaxDualSourceDrawBuffersEXT value for EXT_blend_func_extended.
    int MaxDualSourceDrawBuffers;

    // Name Hashing.
    // Set a 64 bit hash function to enable user-defined name hashing.
    // Default is NULL.
    ShHashFunction64 HashFunction;

    // The maximum complexity an expression can be.
    int MaxExpressionComplexity;

    // The maximum depth a call stack can be.
    int MaxCallStackDepth;
} ShBuiltInResources;

//
// Initialize built-in resources with minimum expected values.
// Parameters:
// resources: The object to initialize. Will be comparable with memcmp.
//
COMPILER_EXPORT void ShInitBuiltInResources(ShBuiltInResources *resources);

//
// ShHandle held by but opaque to the driver.  It is allocated,
// managed, and de-allocated by the compiler. Its contents
// are defined by and used by the compiler.
//
// If handle creation fails, 0 will be returned.
//
typedef void *ShHandle;

//
// Returns the a concatenated list of the items in ShBuiltInResources as a
// null-terminated string.
// This function must be updated whenever ShBuiltInResources is changed.
// Parameters:
// handle: Specifies the handle of the compiler to be used.
COMPILER_EXPORT const std::string &ShGetBuiltInResourcesString(const ShHandle handle);

//
// Driver calls these to create and destroy compiler objects.
//
// Returns the handle of constructed compiler, null if the requested compiler is
// not supported.
// Parameters:
// type: Specifies the type of shader - GL_FRAGMENT_SHADER or GL_VERTEX_SHADER.
// spec: Specifies the language spec the compiler must conform to SH_GLES2_SPEC
// output: Specifies the output code type - SH_GLSL_OUTPUT
// resources: Specifies the built-in resources.
COMPILER_EXPORT ShHandle ShConstructCompiler(
    sh::GLenum type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources *resources);
COMPILER_EXPORT void ShDestruct(ShHandle handle);

//
// Compiles the given shader source.
// If the function succeeds, the return value is true, else false.
// Parameters:
// handle: Specifies the handle of compiler to be used.
// shaderStrings: Specifies an array of pointers to null-terminated strings
//                containing the shader source code.
// numStrings: Specifies the number of elements in shaderStrings array.
// compileOptions: A mask containing the following parameters:
// SH_VALIDATE: Validates shader to ensure that it conforms to the spec
//              specified during compiler construction.
// SH_OBJECT_CODE: Translates intermediate tree to glsl or hlsl shader.
//                 Can be queried by calling ShGetObjectCode().
//
COMPILER_EXPORT bool ShCompile(
    const ShHandle handle,
    const char * const shaderStrings[],
    size_t numStrings,
    int compileOptions);

// Clears the results from the previous compilation.
COMPILER_EXPORT void ShClearResults(const ShHandle handle);

// Return the currently set language output type.
COMPILER_EXPORT ShShaderOutput ShGetShaderOutputType(
    const ShHandle handle);

// Returns null-terminated object code for a compiled shader.
// Parameters:
// handle: Specifies the compiler
COMPILER_EXPORT const std::string &ShGetObjectCode(const ShHandle handle);


#endif // GLSLANG_SHADERLANG_H_
