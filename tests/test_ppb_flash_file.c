#include "nih_test.h"
#include <src/ppb_flash_file.c>
#include <stdio.h>
#include <stdlib.h>

static
void
test_case(const char *s, const char *res)
{
    printf("  /%s -> %s\n", s, res);
    char *got = to_abs_path("/start", s);
    ASSERT_STREQ(got, res);
    free(got);
}

TEST(ppb_flash_file, to_abs_path)
{
    test_case("WritableRoot/#SharedObjects/4TVF7XJR/macromedia.com/support/flashplayer/sys/#127.0.0.1",
              "/start/WritableRoot/#SharedObjects/4TVF7XJR/macromedia.com/support/flashplayer/sys/#127.0.0.1");

    test_case("qw/../../../../", "/start/");
    test_case("qw/../../../..", "/start/");
    test_case("p1/p2/p3/../p4", "/start/p1/p2/p4");
    test_case("p1/p2/p3/./p4",  "/start/p1/p2/p3/p4");
    test_case("././p3",         "/start/p3");
    test_case("p1/.../p2",      "/start/p1/.../p2");
    test_case("p1/p2/./../p3",  "/start/p1/p3");
    test_case("..", "/start/");
    test_case(".", "/start/");
    test_case("../", "/start/");
    test_case("./", "/start/");
    test_case("g..", "/start/g..");
    test_case("g.", "/start/g.");
    test_case("g../a", "/start/g../a");
    test_case("g./a", "/start/g./a");
}
