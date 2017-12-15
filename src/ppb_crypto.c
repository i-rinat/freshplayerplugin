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

#include "pp_interface.h"
#include "ppb_crypto.h"
#include "tables.h"
#include "trace_core.h"
#include <stdlib.h>
#include <unistd.h>

void
ppb_crypto_get_random_bytes(char *buffer, uint32_t num_bytes)
{
    ssize_t bytes_read = read(tables_get_urandom_fd(), buffer, num_bytes);
    if (bytes_read < num_bytes) {
        // can't read from file, falling back to rand()
        for (uint32_t k = 0; k < num_bytes; k ++)
            buffer[k] = ((uint32_t)rand() >> 1) & 0xffu;
    }
}


// trace wrapper
TRACE_WRAPPER
void
trace_ppb_crypto_get_random_bytes(char *buffer, uint32_t num_bytes)
{
    trace_info("[PPB] {full} %s num_bytes=%d\n", __func__+6, num_bytes);
    ppb_crypto_get_random_bytes(buffer, num_bytes);
}


const struct PPB_Crypto_Dev_0_1 ppb_crypto_dev_interface_0_1 = {
    .GetRandomBytes = TWRAPF(ppb_crypto_get_random_bytes),
};

static
void
__attribute__((constructor))
constructor_ppb_crypto(void)
{
    register_interface(PPB_CRYPTO_DEV_INTERFACE_0_1, &ppb_crypto_dev_interface_0_1);
}
