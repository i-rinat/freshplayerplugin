// testing that "__thread" specifier works

#include "nih_test.h"
#include <pthread.h>
#include <stdio.h>

static pthread_barrier_t b1, b2;
static pthread_t         t;
static __thread int      v;

static int *vaddr1;
static int *vaddr2;

static
void *
thread_func(void *param)
{
    vaddr2 = &v;
    v = 2;    // setting this shouldn't change "v" value in main thread

    pthread_barrier_wait(&b1);
    pthread_barrier_wait(&b2);
    return NULL;
}

TEST(thread_specifier, test_1)
{
    ASSERT_EQ(0, pthread_barrier_init(&b1, NULL, 2));
    ASSERT_EQ(0, pthread_barrier_init(&b2, NULL, 2));

    vaddr1 = &v;
    v = 1;  // assign thread local variable a value

    ASSERT_EQ(0, pthread_create(&t, NULL, thread_func, NULL));

    // wait for thread to set "v"
    ASSERT_EQ(0, pthread_barrier_wait(&b1));

    // ensure this "v" is not affected by other thread
    ASSERT_EQ(v, 1);

    // allow other thread to continue
    pthread_barrier_wait(&b2);
    pthread_join(t, NULL);

    ASSERT_EQ(0, pthread_barrier_destroy(&b1));
    ASSERT_EQ(0, pthread_barrier_destroy(&b2));

    printf("vaddr1 = %p\n", vaddr1);
    printf("vaddr2 = %p\n", vaddr2);
}
