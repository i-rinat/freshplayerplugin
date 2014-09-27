#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <src/ppb_url_request_info.c>

#define test_ltrim(in, out)                                         \
    do {                                                            \
        const char *s = ltrim(in);                                  \
        assert(strcmp(s, out) == 0);                                \
    } while (0)

int
main(void)
{
    test_ltrim("", "");
    test_ltrim("hello", "hello");
    test_ltrim("   hello", "hello");
    test_ltrim(" \t  hello", "hello");
    test_ltrim("\n\nhello", "hello");

    printf("pass\n");
    return 0;
}
