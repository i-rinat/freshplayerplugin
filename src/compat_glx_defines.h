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

#pragma once

// https://www.opengl.org/registry/specs/ARB/glx_create_context.txt
#define GLX_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB           0x2092
#define GLX_CONTEXT_FLAGS_ARB                   0x2094
#define GLX_CONTEXT_PROFILE_MASK_ARB            0x9126

// https://www.opengl.org/registry/specs/EXT/glx_create_context_es2_profile.txt
#define GLX_CONTEXT_ES_PROFILE_BIT_EXT          0x00000004
#define GLX_CONTEXT_ES2_PROFILE_BIT_EXT         0x00000004

// https://www.opengl.org/registry/specs/EXT/texture_from_pixmap.txt
#define GLX_BIND_TO_TEXTURE_RGB_EXT        0x20D0
#define GLX_BIND_TO_TEXTURE_RGBA_EXT       0x20D1
#define GLX_BIND_TO_MIPMAP_TEXTURE_EXT     0x20D2
#define GLX_BIND_TO_TEXTURE_TARGETS_EXT    0x20D3
#define GLX_Y_INVERTED_EXT                 0x20D4

#define GLX_TEXTURE_FORMAT_EXT             0x20D5
#define GLX_TEXTURE_TARGET_EXT             0x20D6
#define GLX_MIPMAP_TEXTURE_EXT             0x20D7

#define GLX_TEXTURE_FORMAT_NONE_EXT        0x20D8
#define GLX_TEXTURE_FORMAT_RGB_EXT         0x20D9
#define GLX_TEXTURE_FORMAT_RGBA_EXT        0x20DA

#define GLX_TEXTURE_1D_BIT_EXT             0x00000001
#define GLX_TEXTURE_2D_BIT_EXT             0x00000002
#define GLX_TEXTURE_RECTANGLE_BIT_EXT      0x00000004

#define GLX_TEXTURE_1D_EXT                 0x20DB
#define GLX_TEXTURE_2D_EXT                 0x20DC
#define GLX_TEXTURE_RECTANGLE_EXT          0x20DD

#define GLX_FRONT_LEFT_EXT                 0x20DE
#define GLX_FRONT_RIGHT_EXT                0x20DF
#define GLX_BACK_LEFT_EXT                  0x20E0
#define GLX_BACK_RIGHT_EXT                 0x20E1
#define GLX_FRONT_EXT                      GLX_FRONT_LEFT_EXT
#define GLX_BACK_EXT                       GLX_BACK_LEFT_EXT
#define GLX_AUX0_EXT                       0x20E2
#define GLX_AUX1_EXT                       0x20E3
#define GLX_AUX2_EXT                       0x20E4
#define GLX_AUX3_EXT                       0x20E5
#define GLX_AUX4_EXT                       0x20E6
#define GLX_AUX5_EXT                       0x20E7
#define GLX_AUX6_EXT                       0x20E8
#define GLX_AUX7_EXT                       0x20E9
#define GLX_AUX8_EXT                       0x20EA
#define GLX_AUX9_EXT                       0x20EB
