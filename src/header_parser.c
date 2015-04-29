/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include "header_parser.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>


struct parsed_headers_s *
hp_parse_headers(const char *headers)
{
    struct parsed_headers_s *ph = calloc(1, sizeof(*ph));
    const char *delimiter = "\r\n";
    char *headers_copy, *saveptr, *part;
    int k;

    if (headers == NULL) {
        // all fields zeroed
        return ph;
    }

    // count headers, determine http code
    headers_copy = strdup(headers);
    ph->cnt = 0;
    ph->http_code = 200;
    part = strtok_r(headers_copy, delimiter, &saveptr);
    if (part) {
        int major = 0, minor = 0, code = 0, ret;
        ret = sscanf(part, "HTTP/%6d.%6d %6d", &major, &minor, &code);
        if (ret >= 3) {
            ph->http_code = code;
        }
        part = strtok_r(NULL, delimiter, &saveptr);
    }
    while (part) {
        ph->cnt ++;
        part = strtok_r(NULL, delimiter, &saveptr);
    }
    free(headers_copy);

    // parse headers to name and value
    headers_copy = strdup(headers);
    if (ph->cnt > 0) {
        ph->name = malloc(sizeof(char *) * ph->cnt);
        ph->value = malloc(sizeof(char *) * ph->cnt);
    }
    part = strtok_r(headers_copy, delimiter, &saveptr);

    // save status line
    if (part) {
        ph->status_line = strdup(part);
        part = strtok_r(NULL, delimiter, &saveptr);
    }

    // save headers
    k = 0;
    while (part && k < ph->cnt) {
        char *colon = strchr(part, ':');
        if (colon) {
            *colon = 0;
            ph->name[k] = strdup(part);
            char *value_start = colon + 1;
            while (isspace(*value_start))
                value_start ++;
            ph->value[k] = strdup(value_start);
            *colon = ':';
        } else {
            ph->name[k] = strdup(part);
            ph->value[k] = strdup("");
        }

        part = strtok_r(NULL, delimiter, &saveptr);
        k ++;
    }
    free(headers_copy);

    return ph;
}

void
hp_free_parsed_headers(struct parsed_headers_s *ph)
{
    for (unsigned int k = 0; k < ph->cnt; k ++) {
        free(ph->name[k]);
        free(ph->value[k]);
    }
    free(ph->name);
    free(ph->value);
    free(ph->status_line);
    free(ph);
}

const char *
hp_get_header_value(struct parsed_headers_s *ph, const char *name)
{
    for (unsigned int k = 0; k < ph->cnt; k ++) {
        if (strcasecmp(ph->name[k], name) == 0)
            return ph->value[k];
    }
    return NULL;
}

int
hp_header_exists(struct parsed_headers_s *ph, const char *name)
{
    for (unsigned int k = 0; k < ph->cnt; k ++) {
        if (strcasecmp(ph->name[k], name) == 0)
            return 1;
    }
    return 0;
}
