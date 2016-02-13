// testing that "__thread" specifier works

#undef NDEBUG
#include <assert.h>
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

int
main(void)
{
    int ret;

    ret = pthread_barrier_init(&b1, NULL, 2);
    assert(ret == 0);
    ret = pthread_barrier_init(&b2, NULL, 2);
    assert(ret == 0);

    vaddr1 = &v;
    v = 1;  // assign thread local variable a value

    ret = pthread_create(&t, NULL, thread_func, NULL);
    assert(ret == 0);

    // wait for thread to set "v"
    ret = pthread_barrier_wait(&b1);
    assert(ret == 0);

    // ensure this "v" is not affected by other thread
    assert(v == 1);

    // allow other thread to continue
    pthread_barrier_wait(&b2);
    pthread_join(t, NULL);

    ret = pthread_barrier_destroy(&b1);
    assert(ret == 0);
    ret = pthread_barrier_destroy(&b2);
    assert(ret == 0);

    printf("vaddr1 = %p\n", vaddr1);
    printf("vaddr2 = %p\n", vaddr2);

    printf("pass\n");
    return 0;
}


