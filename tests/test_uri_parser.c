#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <src/uri_parser/uri_parser.h>

static
void
fill_parse_map(char *pm, char type, struct PP_URLComponent_Dev c)
{
    for (int k = c.begin; k < c.begin + c.len; k ++) {
        assert(pm[k] == '.');
        pm[k] = type;
    }
}

static
void
check_parse_uri(const char *uri, const char *pm)
{
    size_t uri_len = strlen(uri);
    size_t pr_len = strlen(uri);

    printf("checking   %s\n", uri);

    // length should be the same
    assert(uri_len == pr_len);

    // make room for another parse map
    char *pm2 = strdup(uri);
    assert(pm2);
    for (int k = 0; k < pr_len; k ++)
        pm2[k] = '.';

    // parse URI
    struct PP_URLComponents_Dev c;
    uri_parser_parse_uri(uri, &c);

    // fill parse map
    fill_parse_map(pm2, '1', c.scheme);
    fill_parse_map(pm2, '2', c.username);
    fill_parse_map(pm2, '3', c.password);
    fill_parse_map(pm2, '4', c.host);
    fill_parse_map(pm2, '5', c.port);
    fill_parse_map(pm2, '6', c.path);
    fill_parse_map(pm2, '7', c.query);
    fill_parse_map(pm2, '8', c.ref);

    printf("result     %s\nexpecting  %s\n\n", pm, pm2);
    assert(strcmp(pm, pm2) == 0);
    free(pm2);
}

static
void
test_parser(void)
{
    check_parse_uri("", "");

    check_parse_uri("http://user:pass@example.org:3128/path1/path2/?q=query#fragment",
                    "1111...2222.3333.44444444444.55556666666666666.7777777.88888888");

    check_parse_uri("http://user@example.org:3128/path1/path2/?q=query#fragment",
                    "1111...2222.44444444444.55556666666666666.7777777.88888888");

    check_parse_uri("http://user:pass@example.org/path1/path2/?q=query#fragment",
                    "1111...2222.3333.444444444446666666666666.7777777.88888888");

    check_parse_uri("https://user@example.org/path1/path2/?q=query#fragment",
                    "11111...2222.444444444446666666666666.7777777.88888888");

    check_parse_uri("http://example.org/path1/path2/?q=query#fragment",
                    "1111...444444444446666666666666.7777777.88888888");

    check_parse_uri("/path1/path2/?q=query#fragment",
                    "6666666666666.7777777.88888888");

    check_parse_uri("?q=query#fragment",
                    ".7777777.88888888");

    check_parse_uri("#fragment",
                    ".88888888");

    check_parse_uri("example.org",
                    "66666666666");

    check_parse_uri("http://example.org/path1/path2/?q=query#fragment",
                    "1111...444444444446666666666666.7777777.88888888");

    check_parse_uri("file:///usr/share/doc/coreutils/README",
                    "1111...6666666666666666666666666666666");

    check_parse_uri("//example.com:80/#h1",
                    "..44444444444.556.88");

    check_parse_uri("/just/path",
                    "6666666666");

    check_parse_uri("http://example.org/?q=spaces in query,errors in %encodings, unicode%u8888#1",
                    "1111...444444444446.77777777777777777777777777777777777777777777777777777.8");

    check_parse_uri("relative/path",
                    "6666666666666");

    check_parse_uri("../another/relative%path",
                    "666666666666666666666666");
}

static
void
check_path_merge(const char *base, const char *rel, const char *sample)
{
    gchar *merged = uri_parser_merge_uris(base, rel);
    printf("merging    %s\nwith       %s\nresult     %s\n", base, rel, merged);
    printf("expecting  %s\n\n", sample);
    assert(strcmp(merged, sample) == 0);
    g_free(merged);
}

static
void
test_path_merging(void)
{
    check_path_merge("http://example.com/path/qw", "asdf",
                     "http://example.com/path/asdf");

    // test cases from from RFC 3986
    const char *base = "http://a/b/c/d;p?q";
    // 5.4.1. Normal Examples
    check_path_merge(base, "g:h", "g:h");
    check_path_merge(base, "g", "http://a/b/c/g");
    check_path_merge(base, "./g", "http://a/b/c/g");
    check_path_merge(base, "g/", "http://a/b/c/g/");
    check_path_merge(base, "/g", "http://a/g");
    check_path_merge(base, "//g", "http://g");
    check_path_merge(base, "?y", "http://a/b/c/d;p?y");
    check_path_merge(base, "g?y", "http://a/b/c/g?y");
    check_path_merge(base, "#s", "http://a/b/c/d;p?q#s");
    check_path_merge(base, "g#s", "http://a/b/c/g#s");
    check_path_merge(base, "g?y#s", "http://a/b/c/g?y#s");
    check_path_merge(base, ";x", "http://a/b/c/;x");
    check_path_merge(base, "g;x", "http://a/b/c/g;x");
    check_path_merge(base, "g;x?y#s", "http://a/b/c/g;x?y#s");
    check_path_merge(base, "", "http://a/b/c/d;p?q");
    check_path_merge(base, ".", "http://a/b/c/");
    check_path_merge(base, "./", "http://a/b/c/");
    check_path_merge(base, "..", "http://a/b/");
    check_path_merge(base, "../", "http://a/b/");
    check_path_merge(base, "../g", "http://a/b/g");
    check_path_merge(base, "../..", "http://a/");
    check_path_merge(base, "../../", "http://a/");
    check_path_merge(base, "../../g", "http://a/g");

    // 5.4.2. Abnormal Examples
    check_path_merge(base, "../../../g", "http://a/g");
    check_path_merge(base, "../../../../g", "http://a/g");
    check_path_merge(base, "/./g", "http://a/g");
    check_path_merge(base, "/../g", "http://a/g");
    check_path_merge(base, "g.", "http://a/b/c/g.");
    check_path_merge(base, ".g", "http://a/b/c/.g");
    check_path_merge(base, "g..", "http://a/b/c/g..");
    check_path_merge(base, "..g", "http://a/b/c/..g");
    check_path_merge(base, "./../g", "http://a/b/g");
    check_path_merge(base, "./g/.", "http://a/b/c/g/");
    check_path_merge(base, "g/./h", "http://a/b/c/g/h");
    check_path_merge(base, "g/../h", "http://a/b/c/h");
    check_path_merge(base, "g;x=1/./y", "http://a/b/c/g;x=1/y");
    check_path_merge(base, "g;x=1/../y", "http://a/b/c/y");
    check_path_merge(base, "g?y/./x", "http://a/b/c/g?y/./x");
    check_path_merge(base, "g?y/../x", "http://a/b/c/g?y/../x");
    check_path_merge(base, "g#s/./x", "http://a/b/c/g#s/./x");
    check_path_merge(base, "g#s/../x", "http://a/b/c/g#s/../x");

    check_path_merge(base, "g../e", "http://a/b/c/g../e");
}

int
main(void)
{
    test_parser();
    test_path_merging();

    printf("pass\n");
    return 0;
}
