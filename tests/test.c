#include <string.h>
#include "test.h"


struct testcase {
    char             *name;
    void            (*f)(void);
    struct testcase  *next;
};

static
struct testcase head = {};

static
struct testcase **next = &head.next;

static
void (*test_constructor)(void) = NULL;

void
test_add_func(const char *name, void (*f)(void))
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
test_run_all(void)
{
    // call all tests, one by one
    for (struct testcase *t = head.next; t != NULL; t = t->next) {
        printf("running %s\n", t->name);
        t->f();
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
test_set_constructor(void (*f)(void))
{
    test_constructor = f;
}

// provides a main() function for test that doesn't have any.
__attribute__((weak))
int
main(void)
{
    if (test_constructor)
        test_constructor();

    test_run_all();
    printf("pass\n");
    return 0;
}
