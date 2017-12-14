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

#include "encoding_alias.h"
#include <glib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum {
    UIGNORE = 0,
    ZERO =    1,
    NONZ =    2,
};

static const uint8_t char_types[256] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
    ZERO, NONZ, NONZ, NONZ, NONZ, NONZ, NONZ, NONZ, NONZ, NONZ, 0,    0,    0,    0,    0,    0,
    0,    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0,    0,    0,    0,    0,
    0,    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0,    0,    0,    0,    0,

    /* remaining 128-255 are 0 */
};

// maps an alias to the canonical encoding name
static GHashTable *ce_ht; // char * -> char *


static
char *
strip_enc_name(const char *name)
{
    if (!name)
        return NULL;

    char *dst = strdup(name);
    if (!dst)
        return NULL;

    char *p = dst;
    int after_digit = 0;

    while (1) {
        char c = *name++;
        if (c == '\0')
            break;

        const uint8_t type = char_types[(uint8_t)c];
        switch (type) {
        case UIGNORE:
            after_digit = 0;
            // ignore all but letter and digits
            continue;

        case ZERO:
            if (!after_digit) {
                const uint8_t next_type = char_types[(uint8_t)(*name)];
                if (next_type == ZERO || next_type == NONZ) {
                    // ignore leading zero before another digit
                    continue;
                }
            }
            break;

        case NONZ:
            after_digit = 1;
            break;

        default:
            c = type; // lowercased letter
            after_digit = 0;
            break;
        }

        *p++ = c;
    }

    *p = 0;
    return dst;
}

__attribute__((constructor))
static
void
constructor_encoding_alias(void)
{
    ce_ht = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    struct {
        const char *dst;
        const char *src;
    } encs[] = {
#include "encoding_alias_list.inc.c"
    };

    for (uint32_t k = 0; k < sizeof(encs) / sizeof(encs[0]); k ++) {
        char *stripped_src_name = strip_enc_name(encs[k].src);
        g_hash_table_insert(ce_ht, stripped_src_name, strdup(encs[k].dst));
    }
}

__attribute__((destructor))
static
void
destructor_encoding_alias(void)
{
    g_hash_table_destroy(ce_ht);
}

const char *
encoding_alias_get_canonical_name(const char *name)
{
    const char *res = name;
    char *stripped_name = strip_enc_name(name);

    res = g_hash_table_lookup(ce_ht, stripped_name);
    if (!res)
        res = name;

    g_free(stripped_name);
    return res;
}
