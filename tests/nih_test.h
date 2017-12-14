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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------
#define TEST(suite_name, test_name)                                     \
static                                                                  \
void                                                                    \
nih_test_ ## suite_name ## _ ## test_name(void);                        \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
nih_test_constructor_ ## suite_name ## _ ## test_name(void)             \
{                                                                       \
    const char *testname = #suite_name ":" #test_name;                  \
    nih_test_add_func(testname,                                         \
                      nih_test_ ## suite_name ## _ ## test_name);       \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
nih_test_ ## suite_name ## _ ## test_name(void)
// ======================================================================

// ----------------------------------------------------------------------
#define TEST_SETUP(...)                                                 \
static                                                                  \
void                                                                    \
nih_test_setup(void);                                                   \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
nih_test_constructor_test_setup(void)                                   \
{                                                                       \
    nih_test_set_test_setup(nih_test_setup);                            \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
nih_test_setup(void)
// ======================================================================

// ----------------------------------------------------------------------
#define TEST_TEARDOWN(...)                                              \
static                                                                  \
void                                                                    \
nih_test_teardown(void);                                                \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
nih_test_constructor_test_teardown(void)                                \
{                                                                       \
    nih_test_set_test_teardown(nih_test_teardown);                      \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
nih_test_teardown(void)
// ======================================================================

// ----------------------------------------------------------------------
#define TESTSUITE_SETUP(...)                                            \
static                                                                  \
void                                                                    \
nih_testsuite_setup(void);                                              \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
nih_test_constructor_testsuite_setup(void)                              \
{                                                                       \
    nih_test_set_testsuite_setup(nih_testsuite_setup);                  \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
nih_testsuite_setup(void)
// ======================================================================

// ----------------------------------------------------------------------
#define TESTSUITE_TEARDOWN(...)                                         \
static                                                                  \
void                                                                    \
nih_testsuite_teardown(void);                                           \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
nih_test_constructor_testsuite_teardown(void)                           \
{                                                                       \
    nih_test_set_testsuite_teardown(nih_testsuite_teardown);            \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
nih_testsuite_teardown(void)
// ======================================================================

// ----------------------------------------------------------------------
#define ASSERT_OP(expr1, operation, expr2, name)                        \
do {                                                                    \
    __typeof(expr1) ____value1 = (expr1);                               \
    __typeof(expr2) ____value2 = (expr2);                               \
    int             ____result = (____value1 operation ____value2);     \
                                                                        \
    if (!____result) {                                                  \
        printf("%s(%s, %s) failed\n", #name, #expr1, #expr2);           \
        abort();                                                        \
    }                                                                   \
} while (0)
// ======================================================================

// ----------------------------------------------------------------------
#define ASSERT_BOOL_OP(expr1, expr2, name)                              \
do {                                                                    \
    int ____value1 = !!(expr1);                                         \
    int ____value2 = !!(expr2);                                         \
    int ____result = (____value1 == ____value2);                        \
                                                                        \
    if (!____result) {                                                  \
        printf("%s(%s) failed\n", #name, #expr1);                       \
        abort();                                                        \
    }                                                                   \
} while (0)
// ======================================================================

#define ASSERT_TRUE(e)      ASSERT_BOOL_OP(e, 1, ASSERT_TRUE)
#define ASSERT_FALSE(e)     ASSERT_BOOL_OP(e, 0, ASSERT_FALSE)

#define ASSERT_EQ(e1, e2)   ASSERT_OP(e1, ==, e2, ASSERT_EQ)
#define ASSERT_NE(e1, e2)   ASSERT_OP(e1, !=, e2, ASSERT_NE)
#define ASSERT_LT(e1, e2)   ASSERT_OP(e1, <,  e2, ASSERT_LT)
#define ASSERT_LE(e1, e2)   ASSERT_OP(e1, <=, e2, ASSERT_LE)
#define ASSERT_GT(e1, e2)   ASSERT_OP(e1, >,  e2, ASSERT_GT)
#define ASSERT_GE(e1, e2)   ASSERT_OP(e1, >=, e2, ASSERT_GE)

// ----------------------------------------------------------------------
#define ASSERT_STREQ(expr1, expr2)                                      \
do {                                                                    \
    int ____result = (strcmp((expr1), (expr2)) == 0);                   \
                                                                        \
    if (!____result) {                                                  \
        printf("ASSERT_STREQ(%s, %s) failed\n", #expr1, #expr2);        \
        abort();                                                        \
    }                                                                   \
} while (0)
// ======================================================================


void
nih_test_add_func(const char *name, void (*f)(void));

void
nih_test_set_test_setup(void (*f)(void));

void
nih_test_set_test_teardown(void (*f)(void));

void
nih_test_set_testsuite_setup(void (*f)(void));

void
nih_test_set_testsuite_teardown(void (*f)(void));

void
nih_test_run_all(void);
