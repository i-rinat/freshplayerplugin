#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ----------------------------------------------------------------------
#define TEST(suite_name, test_name)                                     \
static                                                                  \
void                                                                    \
test_ ## suite_name ## _ ## test_name(void);                            \
                                                                        \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
constructor_test_ ## suite_name ## _ ## test_name(void)                 \
{                                                                       \
    const char *testname = #suite_name ":" #test_name;                  \
    test_add_func(testname, test_ ## suite_name ## _ ## test_name);     \
}                                                                       \
                                                                        \
static                                                                  \
void                                                                    \
test_ ## suite_name ## _ ## test_name(void)
// ======================================================================

// ----------------------------------------------------------------------
#define TEST_CONSTRUCTOR(func)                                          \
__attribute__((constructor))                                            \
static                                                                  \
void                                                                    \
constructor_test_constructor(void)                                      \
{                                                                       \
    test_set_constructor(func);                                         \
}
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
test_add_func(const char *name, void (*f)(void));

void
test_set_constructor(void (*f)(void));

void
test_run_all(void);
