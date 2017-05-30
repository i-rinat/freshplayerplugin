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

#if defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
#define STATIC_ASSERT_ATTRIBUTE_UNUSED  __attribute__((unused))
#else
#define STATIC_ASSERT_ATTRIBUTE_UNUSED
#endif

#define STATIC_ASSERT_HELPER1(expr, msg)    typedef char static_assertion_##msg[(!!(expr))*2 - 1] \
                                            STATIC_ASSERT_ATTRIBUTE_UNUSED
#define STATIC_ASSERT_HELPER2(expr, msg)    STATIC_ASSERT_HELPER1(expr, msg)
#define STATIC_ASSERT(expr)                 STATIC_ASSERT_HELPER2(expr, __LINE__)


#define STATIC_ASSERT_LESS_OR_EQ(expr1, expr2)  STATIC_ASSERT(expr1 <= expr2)
