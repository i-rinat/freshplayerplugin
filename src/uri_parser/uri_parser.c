/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../compat.h"
#include "uri_parser.h"
#include <glib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int   len;
    char *data;
} str_t;


static
void
set_url_component(struct PP_URLComponent_Dev *c, const char *s, const char *b, const char *e)
{
    c->begin = b - s;
    c->len = e - b;
}

static
void
reset_url_component(struct PP_URLComponent_Dev *c)
{
    c->begin = 0;
    c->len = -1;
}

static
void
reset_url_components(struct PP_URLComponents_Dev *c)
{
    reset_url_component(&c->scheme);
    reset_url_component(&c->username);
    reset_url_component(&c->password);
    reset_url_component(&c->host);
    reset_url_component(&c->port);
    reset_url_component(&c->path);
    reset_url_component(&c->query);
    reset_url_component(&c->ref);
}

void
uri_parser_parse_uri(const char *s, struct PP_URLComponents_Dev *components)
{
    const char *p = s;
    const char *pe = s + strlen(s);
    const char *eof = pe;
    struct {
        const char *scheme;
        const char *username;
        const char *password;
        const char *host;
        const char *port;
        const char *path;
        const char *query;
        const char *fragment;
    } mark;
    const char *end_username = NULL, *end_password = NULL;
    int cs;
    struct PP_URLComponents_Dev c2;

    reset_url_components(components);
    reset_url_components(&c2);

    memset(&mark, 0, sizeof(mark));

#include <uri_parser_fsm.c>

    // silence unused-variable warnings
    (void)uri_parser_en_main;
    (void)uri_parser_error;
    (void)uri_parser_first_final;

    (void)end_password;
    (void)end_username;
}

static inline int
urlcomponent_is_empty(const struct PP_URLComponent_Dev *c)
{
    return c->begin == 0 && c->len == -1;
}

static inline int
urlcomponent_is_not_empty(const struct PP_URLComponent_Dev *c)
{
    return !urlcomponent_is_empty(c);
}

static inline int
authority_is_not_empty(const struct PP_URLComponents_Dev *c)
{
    return  urlcomponent_is_not_empty(&c->username) ||
            urlcomponent_is_not_empty(&c->password) ||
            urlcomponent_is_not_empty(&c->host) ||
            urlcomponent_is_not_empty(&c->port);
}

static str_t
extract_component(const char *s, const struct PP_URLComponent_Dev *c)
{
    str_t res;
    res.data = (char *)s + c->begin;
    res.len = (c->len >= 0) ? c->len : 0;
    return res;
}

static str_t
extract_authority(const char *s, struct PP_URLComponents_Dev *c)
{
    int32_t begin = 0;
    if (c->port.len >= 0)       begin = c->port.begin;
    if (c->host.len >= 0)       begin = c->host.begin;
    if (c->password.len >= 0)   begin = c->password.begin;
    if (c->username.len >= 0)   begin = c->username.begin;

    int32_t end = -1;
    if (c->username.len >= 0)   end = c->username.begin + c->username.len;
    if (c->password.len >= 0)   end = c->password.begin + c->password.len;
    if (c->host.len >= 0)       end = c->host.begin + c->host.len;
    if (c->port.len >= 0)       end = c->port.begin + c->port.len;

    struct PP_URLComponent_Dev cc = { .begin = begin, .len = end - begin };
    return extract_component(s, &cc);
}

static str_t
merge_path(const char *base_uri, const struct PP_URLComponents_Dev *base_c, const char *rel_uri,
           const struct PP_URLComponents_Dev *rel_c, GList **m)
{
    gchar *s;
    str_t rel_path = extract_component(rel_uri, &rel_c->path);

    if (base_c->host.len > 0 && base_c->path.len <= 0) {
        s = g_strdup_printf("/%.*s", rel_path.len, rel_path.data);
    } else {
        str_t base_path = extract_component(base_uri, &base_c->path);
        int len = base_path.len;

        while (len > 0 && base_path.data[len - 1] != '/')
            len --;

        s = g_strdup_printf("%.*s%.*s", len, base_path.data, rel_path.len, rel_path.data);
    }

    *m = g_list_prepend(*m, s);
    return (str_t){ .data = s, .len = strlen(s) };
}

static str_t
remove_dot_segments(str_t s, GList **m)
{
    // make a working copy
    str_t res;
    res.data = g_strndup(s.data, s.len);
    res.len = strlen(res.data);
    *m = g_list_prepend(*m, res.data);

    int pdst = 0;
    int dot_cnt = 0;
    int after_slash = 1;
    for (int psrc = 0; psrc < s.len; psrc ++) {
        char c = s.data[psrc];
        switch (c) {
        case '/':
            if (dot_cnt == 1 || dot_cnt == 2) {
                for (int k = 0; k < dot_cnt; k ++) {
                    while (pdst > 0 && res.data[pdst - 1] != '/')
                        pdst --;
                    if (pdst > 0)
                        pdst --;
                }
            }
            res.data[pdst++] = '/';
            dot_cnt = 0;
            after_slash = 1;
            break;
        case '.':
            res.data[pdst++] = '.';
            if (after_slash)
                dot_cnt ++;
            break;
        default:
            dot_cnt = 0;
            after_slash = 0;
            res.data[pdst++] = c;
            break;
        }
    }

    // treat trailing "/.." and "/."
    if (after_slash && (dot_cnt == 1 || dot_cnt == 2)) {
        for (int k = 0; k < dot_cnt; k ++) {
            while (pdst > 0 && res.data[pdst - 1] != '/')
                pdst --;
            if (pdst > 0)
                pdst --;
        }
        res.data[pdst++] = '/';
    }

    res.len = pdst;
    return res;
}

gchar *
uri_parser_merge_uris(const char *base_uri, const char *rel_uri)
{
    struct PP_URLComponents_Dev base_c, rel_c;
    str_t                       scheme, authority, path, query, fragment;
    GList                      *m = NULL;    // list of allocated memory blocks
    int                         scheme_is_file = 0;

    uri_parser_parse_uri(base_uri, &base_c);

    // if base_uri is local file, remove heading slashed from rel_uri to make it relative.
    // That will emulate webserver root
    if (base_c.scheme.len > 0 && strncmp(base_uri + base_c.scheme.begin, "file", 4) == 0
        && base_c.scheme.len == 4)
    {
        scheme_is_file = 1;
        while (rel_uri && *rel_uri == '/')
            rel_uri ++;
    }

    uri_parser_parse_uri(rel_uri, &rel_c);

    // See RFC 3986, 5.2. Relative Resolution
    if (urlcomponent_is_not_empty(&rel_c.scheme)) {
        scheme =    extract_component(rel_uri, &rel_c.scheme);
        authority = extract_authority(rel_uri, &rel_c);
        path =      extract_component(rel_uri, &rel_c.path);
        path =      remove_dot_segments(path, &m);
        query =     extract_component(rel_uri, &rel_c.query);
    } else {
        if (authority_is_not_empty(&rel_c)) {
            authority = extract_authority(rel_uri, &rel_c);
            path =      extract_component(rel_uri, &rel_c.path);
            path =      remove_dot_segments(path, &m);
            query =     extract_component(rel_uri, &rel_c.query);
        } else {
            if (rel_c.path.len <= 0) {
                path = extract_component(base_uri, &base_c.path);
                if (urlcomponent_is_not_empty(&rel_c.query)) {
                    query = extract_component(rel_uri, &rel_c.query);
                } else {
                    query = extract_component(base_uri, &base_c.query);
                }
            } else {
                if (rel_c.path.len > 0 && rel_uri[rel_c.path.begin] == '/') {
                    path = extract_component(rel_uri, &rel_c.path);
                    path = remove_dot_segments(path, &m);
                } else {
                    path = merge_path(base_uri, &base_c, rel_uri, &rel_c, &m);
                    path = remove_dot_segments(path, &m);
                }
                query = extract_component(rel_uri, &rel_c.query);
            }
            authority = extract_authority(base_uri, &base_c);
        }
        scheme = extract_component(base_uri, &base_c.scheme);
    }

    fragment = extract_component(rel_uri, &rel_c.ref);

    gchar *res = g_strdup_printf(
        "%.*s"  // scheme
        "%s"    // ":" if scheme is defined
        "%s"    // "//" if authority is defined
        "%.*s"  // authority
        "%.*s"  // path
        "%s"    // "?" if query is defined
        "%.*s"  // query
        "%s"    // "#" if fragment is defined
        "%.*s", // fragment
        scheme.len, scheme.data,
        scheme.len > 0 ? ":" : "",
        (authority.len > 0 || scheme_is_file) ? "//" : "",
        authority.len, authority.data,
        path.len, path.data,
        query.len > 0 ? "?" : "",
        query.len, query.data,
        fragment.len > 0 ? "#" : "",
        fragment.len, fragment.data);

    // free temporary strings
    g_list_free_full(m, g_free);

    return res;
}
