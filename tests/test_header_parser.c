#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <src/header_parser.c>

int
main(void)
{
    // ===
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

    assert(ph);
    assert(ph->http_code == 200);
    assert(hp_header_exists(ph, "Date") == 1);
    assert(hp_header_exists(ph, "date") == 1);
    assert(hp_header_exists(ph, "Date2") == 0);
    assert(hp_header_exists(ph, "Content-Type") == 1);
    assert(hp_header_exists(ph, "bytes") == 0);

    assert(0 == strcmp(hp_get_header_value(ph, "Date"), "Sat, 19 Apr 2014 15:53:34 GMT"));
    assert(0 == strcmp(ph->status_line, "HTTP/1.1 200 OK"));
    hp_free_parsed_headers(ph);

    // ===
    ph = hp_parse_headers(
        "HTTP/1.0 301 Moved Permanently\r\n"
        "Location: http://example.org\r\n"
        "Connection: Keep-Alive\r\n"
    );

    assert(ph->http_code == 301);
    assert(0 == strcmp(hp_get_header_value(ph, "Location"), "http://example.org"));
    assert(0 == strcmp(ph->status_line, "HTTP/1.0 301 Moved Permanently"));
    hp_free_parsed_headers(ph);

    // ===
    ph = hp_parse_headers(NULL);
    assert(0 == ph->cnt);
    hp_free_parsed_headers(ph);

    // ===
    // no \r\n at the end of last header line
    ph = hp_parse_headers(
        "HTTP/1.0 301 Moved Permanently\r\n"
        "Location: http://example.org\r\n"
        "Connection: Keep-Alive"
    );

    assert(hp_header_exists(ph, "Location") == 1);
    assert(hp_header_exists(ph, "Connection") == 1);
    assert(0 == strcmp(hp_get_header_value(ph, "Connection"), "Keep-Alive"));
    hp_free_parsed_headers(ph);

    // ===
    printf("pass\n");
    return 0;
}
