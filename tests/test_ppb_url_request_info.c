#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <src/ppb_url_request_info.c>

#define test_esc(in, out)                                           \
    do {                                                            \
        char *s = escape_unescaped_characters(in);                  \
        assert(strcmp(s, out) == 0);                                \
        free(s);                                                    \
    } while (0)

#define test_ltrim(in, out)                                         \
    do {                                                            \
        const char *s = ltrim(in);                                  \
        assert(strcmp(s, out) == 0);                                \
    } while (0)

int
main(void)
{
    test_esc("http://y.com/?q=hello world", "http://y.com/?q=hello+world");
    test_esc("y.c/?q=1|2|3", "y.c/?q=1%7c2%7c3");
    test_esc("y.c/?q=w&r=t", "y.c/?q=w&r=t");

    test_esc("example.org", "example.org");
    test_esc("example.org/?q=%25", "example.org/?q=%25");
    test_esc("http://example.org", "http://example.org");

    test_ltrim("", "");
    test_ltrim("hello", "hello");
    test_ltrim("   hello", "hello");
    test_ltrim(" \t  hello", "hello");
    test_ltrim("\n\nhello", "hello");

    printf("pass\n");
    return 0;
}
