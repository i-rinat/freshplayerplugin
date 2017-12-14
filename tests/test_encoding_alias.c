#include "nih_test.h"
#include <src/encoding_alias.c>

TEST(encoding_alias, utf8)
{
    // UTF-8
    ASSERT_STREQ(encoding_alias_get_canonical_name("utf8"), "UTF-8"); // plain
    ASSERT_STREQ(encoding_alias_get_canonical_name("uTF8"), "UTF-8"); // partial uppercase
    ASSERT_STREQ(encoding_alias_get_canonical_name("uTF-8"), "UTF-8");
}

TEST(encoding_alias, cp1251)
{
    // cp1251
    ASSERT_STREQ(encoding_alias_get_canonical_name("cp1251"), "windows-1251");
    ASSERT_STREQ(encoding_alias_get_canonical_name("windows-1251"), "windows-1251");
}

TEST(encoding_alias, GBK)
{
    // GBK
    ASSERT_STREQ(encoding_alias_get_canonical_name("GBK"), "GBK");
    ASSERT_STREQ(encoding_alias_get_canonical_name("gb2312"), "GBK");
    ASSERT_STREQ(encoding_alias_get_canonical_name("gb2312-80"), "GBK");
}

static
void
strip_enc_name_tst(const char *a, const char *b)
{
    char *s = strip_enc_name(a);
    ASSERT_STREQ(s, b);
    free(s);
}

TEST(encoding_alias, strip_enc_name)
{
    strip_enc_name_tst("UTF-8",      "utf8");
    strip_enc_name_tst("  U_t_F_8 ", "utf8");
    strip_enc_name_tst("utf08",      "utf8");
    strip_enc_name_tst("utf0008",    "utf8");
    strip_enc_name_tst("0",  "0");
    strip_enc_name_tst("",   "");
    strip_enc_name_tst("00",  "0");
    strip_enc_name_tst("000", "0");
    strip_enc_name_tst("0010", "10");
    strip_enc_name_tst("asd0", "asd0");
}
