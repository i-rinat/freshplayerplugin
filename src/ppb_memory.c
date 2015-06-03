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

#include "ppb_memory.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_interface.h"


void *
ppb_memory_mem_alloc(uint32_t num_bytes)
{
    return npn.memalloc(num_bytes); // TODO: is it safe to call npn.memalloc()?
}

void
ppb_memory_mem_free(void *ptr)
{
    npn.memfree(ptr); // TODO: is it safe to call npn.memfree()?
}


// trace wrappers
TRACE_WRAPPER
void *
trace_ppb_memory_mem_alloc(uint32_t num_bytes)
{
    trace_info("[PPB] {full} %s num_bytes=%u\n", __func__+6, num_bytes);
    return ppb_memory_mem_alloc(num_bytes);
}

TRACE_WRAPPER
void
trace_ppb_memory_mem_free(void *ptr)
{
    trace_info("[PPB] {full} %s ptr=%p\n", __func__+6, ptr);
    ppb_memory_mem_free(ptr);
}


const struct PPB_Memory_Dev_0_1 ppb_memory_dev_interface_0_1 = {
    .MemAlloc = TWRAPF(ppb_memory_mem_alloc),
    .MemFree =  TWRAPF(ppb_memory_mem_free),
};

static
void
__attribute__((constructor))
constructor_ppb_memory(void)
{
    register_interface(PPB_MEMORY_DEV_INTERFACE_0_1, &ppb_memory_dev_interface_0_1);
}
