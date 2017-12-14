#include "nih_test.h"
#include <src/header_parser.c>
#include <stdio.h>
#include <stdlib.h>

TEST(header_parser, test_1)
{
    struct parsed_headers_s *ph = hp_parse_headers(
        "HTTP/1.1 200 OK\r\n"
        "Date: Sat, 19 Apr 2014 15:53:34 GMT\r\n"
        "Server: Apache/2.4.9 (Debian)\r\n"
        "Last-Modified: Tue, 12 Feb 2013 12:57:02 GMT\r\n"
        "Etag: \"1efbf-4d5868fd0c380\"\r\n"
        "Accept-Ranges: bytes\r\n"
        "Content-Length: 126911\r\n"
        "Keep-Alive: timeout=5, max=95\r\n"
        "Connection: Keep-Alive\r\n"
        "Content-Type: application/x-shockwave-flash\r\n"
    );

    ASSERT_TRUE(ph);
    ASSERT_EQ(ph->http_code, 200);
    ASSERT_TRUE(hp_header_exists(ph, "Date"));
    ASSERT_TRUE(hp_header_exists(ph, "date"));
    ASSERT_FALSE(hp_header_exists(ph, "Date2"));
    ASSERT_TRUE(hp_header_exists(ph, "Content-Type"));
    ASSERT_FALSE(hp_header_exists(ph, "bytes"));

    ASSERT_STREQ(hp_get_header_value(ph, "Date"), "Sat, 19 Apr 2014 15:53:34 GMT");
    ASSERT_STREQ(ph->status_line, "HTTP/1.1 200 OK");
    hp_free_parsed_headers(ph);
}

TEST(header_parser, test_2)
{
    struct parsed_headers_s *ph = hp_parse_headers(
        "HTTP/1.0 301 Moved Permanently\r\n"
        "Location: http://example.org\r\n"
        "Connection: Keep-Alive\r\n"
    );

    ASSERT_EQ(ph->http_code, 301);
    ASSERT_STREQ(hp_get_header_value(ph, "Location"), "http://example.org");
    ASSERT_STREQ(ph->status_line, "HTTP/1.0 301 Moved Permanently");
    hp_free_parsed_headers(ph);
}

TEST(header_parser, test_3)
{
    struct parsed_headers_s *ph = hp_parse_headers(NULL);

    ASSERT_EQ(ph->cnt, 0);
    hp_free_parsed_headers(ph);
}

TEST(header_parser, test_4)
{
    // no \r\n at the end of last header line
    struct parsed_headers_s *ph = hp_parse_headers(
        "HTTP/1.0 301 Moved Permanently\r\n"
        "Location: http://example.org\r\n"
        "Connection: Keep-Alive"
    );

    ASSERT_TRUE(hp_header_exists(ph, "Location"));
    ASSERT_TRUE(hp_header_exists(ph, "Connection"));
    ASSERT_STREQ(hp_get_header_value(ph, "Connection"), "Keep-Alive");
    hp_free_parsed_headers(ph);
}
