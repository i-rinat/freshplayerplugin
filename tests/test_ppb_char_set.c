#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <langinfo.h>
#include <src/ppb_var.h>
#include <src/ppb_char_set.c>

static PP_Instance instance = 0;

// npn.memalloc
static
void *
t_memalloc(uint32_t sz)
{
    return malloc(sz);
}

// npn.memfree
static
void
t_memfree(void *ptr)
{
    free(ptr);
}

static
void
test_default_charset(void)
{
    // silly test actually. Both test and implementation contains almost the same code
    setlocale(LC_ALL, "");
    char *current_charset = nl_langinfo(CODESET);
    struct PP_Var cs = ppb_char_set_get_default_char_set(instance);
    assert(strcmp(ppb_var_var_to_utf8(cs, NULL), current_charset) == 0);
    printf("default charset = %s\n", current_charset);
    ppb_var_release(cs);
}

static
void
test_to_utf16(void)
{
    uint16_t *res;
    uint32_t res_len;

    printf("to utf16: all ASCII\n");
    {
        const char *in = "Hello, world!";
        const uint8_t out[] = {'H', 0, 'e', 0, 'l', 0, 'l', 0, 'o', 0, ',', 0, ' ', 0, 'w', 0,
                               'o', 0, 'r', 0, 'l', 0, 'd', 0, '!', 0};
        res_len = 7777;
        res = ppb_char_set_char_set_to_utf16(instance, in, strlen(in), "UTF-8",
                                             PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == sizeof(out) / sizeof(uint16_t));
        assert(memcmp(res, out, sizeof(out)) == 0);
        free(res);
    }

    printf("to utf16: basic UTF-8\n");
    {
        const char *in = "Привет, мир!";
        const uint8_t out[] = {0x1f, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04,
                               0x42, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x3c, 0x04, 0x38, 0x04,
                               0x40, 0x04, 0x21, 0x00};
        res_len = 7777;
        res = ppb_char_set_char_set_to_utf16(instance, in, strlen(in), "UTF-8",
                                                 PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == sizeof(out) / sizeof(uint16_t));
        assert(memcmp(res, out, sizeof(out)) == 0);
        free(res);
    }

    printf("to utf16: wrong UTF-8 with error\n");
    {
        const char *in = "Hel\x80lo";
        res_len = 7777;
        res = ppb_char_set_char_set_to_utf16(instance, in, strlen(in), "UTF-8",
                                             PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == 0);
        assert(res == NULL);
        free(res);
    }

    // testing PP_CHARSET_CONVERSIONERROR_SKIP or PP_CHARSET_CONVERSIONERROR_SUBSTITUTE
    // have no much sense since every unicode character can be represented in UTF-16
}

static
void
test_from_utf16(void)
{
    char *res;
    uint32_t res_len;

    printf("from utf16: all ASCII\n");
    {
        const uint8_t in[] = {'H', 0, 'e', 0, 'l', 0, 'l', 0, 'o', 0, ',', 0, ' ', 0, 'w', 0,
                              'o', 0, 'r', 0, 'l', 0, 'd', 0, '!', 0};
        const char *out = "Hello, world!";
        res_len = 7777;
        res = ppb_char_set_utf16_to_char_set(instance, (const uint16_t *)in,
                                             sizeof(in) / sizeof(uint16_t), "cp1251",
                                             PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == strlen(out));
        assert(memcmp(res, out, res_len) == 0);
        free(res);
    }

    printf("to utf16: non-ASCII all correct\n");
    {
        const uint8_t in[] = {0x1f, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04,
                              0x42, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x3c, 0x04, 0x38, 0x04,
                              0x40, 0x04, 0x21, 0x00}; // "Привет, мир!"
        const char *out = "\xcf\xf0\xe8\xe2\xe5\xf2\x2c\x20\xec\xe8\xf0\x21"; // "Привет, мир!"
        res_len = 7777;
        res = ppb_char_set_utf16_to_char_set(instance, (const uint16_t *)in,
                                             sizeof(in) / sizeof(uint16_t), "cp1251",
                                             PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == strlen(out));
        assert(memcmp(res, out, res_len) == 0);
        free(res);
    }

    printf("to utf16: non-ASCII PP_CHARSET_CONVERSIONERROR_FAIL\n");
    {
        const uint8_t in[] = {0x1f, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04,
                              0x42, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x6b, 0x26, 0x3c, 0x04,
                              0x38, 0x04, 0x40, 0x04, 0x21, 0x00}; // "Привет, ♫мир!"
        // const char *out = "\xcf\xf0\xe8\xe2\xe5\xf2\x2c\x20\xec\xe8\xf0\x21"; // "Привет, мир!"
        res_len = 7777;
        res = ppb_char_set_utf16_to_char_set(instance, (const uint16_t *)in,
                                             sizeof(in) / sizeof(uint16_t), "cp1251",
                                             PP_CHARSET_CONVERSIONERROR_FAIL, &res_len);
        assert(res_len == 0);
        assert(res == NULL);
        free(res);
    }

    printf("to utf16: non-ASCII PP_CHARSET_CONVERSIONERROR_SKIP\n");
    {
        const uint8_t in[] = {0x1f, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04,
                              0x42, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x6b, 0x26, 0x3c, 0x04,
                              0x38, 0x04, 0x40, 0x04, 0x21, 0x00}; // "Привет, ♫мир!"
        const char *out = "\xcf\xf0\xe8\xe2\xe5\xf2\x2c\x20\xec\xe8\xf0\x21"; // "Привет, мир!"
        res_len = 7777;
        res = ppb_char_set_utf16_to_char_set(instance, (const uint16_t *)in,
                                             sizeof(in) / sizeof(uint16_t), "cp1251",
                                             PP_CHARSET_CONVERSIONERROR_SKIP, &res_len);
        assert(res_len == strlen(out));
        assert(memcmp(res, out, res_len) == 0);
        free(res);
    }

    printf("to utf16: non-ASCII PP_CHARSET_CONVERSIONERROR_SUBSTITUTE\n");
    {
        const uint8_t in[] = {0x1f, 0x04, 0x40, 0x04, 0x38, 0x04, 0x32, 0x04, 0x35, 0x04,
                              0x42, 0x04, 0x2c, 0x00, 0x20, 0x00, 0x6b, 0x26, 0x3c, 0x04,
                              0x38, 0x04, 0x40, 0x04, 0x21, 0x00}; // "Привет, ♫мир!"
        const char *out = "\xcf\xf0\xe8\xe2\xe5\xf2\x2c\x20\x3f\xec\xe8\xf0\x21";// "Привет, ?мир!"
        res_len = 7777;
        res = ppb_char_set_utf16_to_char_set(instance, (const uint16_t *)in,
                                             sizeof(in) / sizeof(uint16_t), "cp1251",
                                             PP_CHARSET_CONVERSIONERROR_SUBSTITUTE, &res_len);
        assert(res_len == strlen(out));
        assert(memcmp(res, out, res_len) == 0);
        free(res);
    }
}

int
main(void)
{
    npn.memalloc = t_memalloc;
    npn.memfree = t_memfree;

    test_default_charset();
    test_to_utf16();
    test_from_utf16();

    printf("pass\n");
    return 0;
}
