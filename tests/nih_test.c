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

#define _XOPEN_SOURCE  500
#include "nih_test.h"
#include <string.h>

struct testcase {
    char             *name;
    void            (*f)(void);
    struct testcase  *next;
};

static
struct testcase head = {};

static
struct testcase **next = &head.next;

static void (*nih_test_setup_func)(void) = NULL;
static void (*nih_test_teardown_func)(void) = NULL;
static void (*nih_testsuite_setup_func)(void) = NULL;
static void (*nih_testsuite_teardown_func)(void) = NULL;

static int constructors_available = 0;

void
nih_test_add_func(const char *name, void (*f)(void))
{
    struct testcase *t = malloc(sizeof(*t));

    if (t == NULL) {
        printf("memory allocation failure\n");
        abort();
    }

    t->name = strdup(name);
    t->f =    f;
    t->next = NULL;

    *next = t;
    next = &t->next;
}

void
nih_test_run_all(void)
{
    // call all tests, one by one
    for (struct testcase *t = head.next; t != NULL; t = t->next) {
        printf("running %s\n", t->name);

        // initialize, if needed
        if (nih_test_setup_func)
            nih_test_setup_func();

        // run actual test function
        t->f();

        // deinitialize, if needed
        if (nih_test_teardown_func)
            nih_test_teardown_func();
    }

    // cleanup memory
    while (head.next) {
        struct testcase *t = head.next;

        head.next = t->next;
        free(t->name);
        free(t);
    }
}

void
nih_test_set_test_setup(void (*f)(void))
{
    nih_test_setup_func = f;
}

void
nih_test_set_test_teardown(void (*f)(void))
{
    nih_test_teardown_func = f;
}

void
nih_test_set_testsuite_setup(void (*f)(void))
{
    nih_testsuite_setup_func = f;
}

void
nih_test_set_testsuite_teardown(void (*f)(void))
{
    nih_testsuite_teardown_func = f;
}

__attribute__((constructor))
static
void
nih_test_constructor_check_constructors(void)
{
    constructors_available = 1;
}

// provides a main() function for a test that doesn't have any.
__attribute__((weak))
int
main(void)
{
    if (!constructors_available) {
        printf("__attribute__((constructor)) doesn't work\n");
        return 1;
    }

    if (nih_testsuite_setup_func)
        nih_testsuite_setup_func();

    nih_test_run_all();

    if (nih_testsuite_teardown_func)
        nih_testsuite_teardown_func();

    printf("pass\n");
    return 0;
}
