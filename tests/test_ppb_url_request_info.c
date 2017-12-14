#include "nih_test.h"
#include <src/ppb_url_request_info.c>
#include <stdio.h>

#define test_ltrim(in, out)                                         \
    do {                                                            \
        const char *s = ltrim(in);                                  \
        ASSERT_TRUE(strcmp(s, out) == 0);                           \
    } while (0)

TEST(ppb_url_request_info, ltrim)
{
    test_ltrim("", "");
    test_ltrim("hello", "hello");
    test_ltrim("   hello", "hello");
    test_ltrim(" \t  hello", "hello");
    test_ltrim("\n\nhello", "hello");
}
