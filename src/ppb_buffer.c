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
#include "pp_resource.h"
#include "ppb_buffer.h"
#include "ppb_core.h"
#include "static_assert.h"
#include "tables.h"
#include "trace_core.h"
#include <stdlib.h>

struct pp_buffer_s {
    COMMON_STRUCTURE_FIELDS
    void                   *data;
    uint32_t                len;
};

STATIC_ASSERT(sizeof(struct pp_buffer_s) <= LARGEST_RESOURCE_SIZE);

PP_Resource
ppb_buffer_create(PP_Instance instance, uint32_t size_in_bytes)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource buffer = pp_resource_allocate(PP_RESOURCE_BUFFER, pp_i);
    struct pp_buffer_s *b = pp_resource_acquire(buffer, PP_RESOURCE_BUFFER);
    if (!b) {
        trace_error("%s, failed to create resource\n", __func__);
        return 0;
    }

    b->len = size_in_bytes;
    b->data = calloc(size_in_bytes, 1);

    pp_resource_release(buffer);
    return buffer;
}

static
void
ppb_buffer_destroy(void *p)
{
    struct pp_buffer_s *b = p;
    free(b->data);
    b->data = NULL;
}

PP_Bool
ppb_buffer_is_buffer(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_BUFFER;
}

PP_Bool
ppb_buffer_describe(PP_Resource resource, uint32_t *size_in_bytes)
{
    struct pp_buffer_s *b = pp_resource_acquire(resource, PP_RESOURCE_BUFFER);
    if (!b)
        return PP_FALSE;

    if (size_in_bytes)
        *size_in_bytes = b->len;

    pp_resource_release(resource);
    return PP_TRUE;
}

void *
ppb_buffer_map(PP_Resource resource)
{
    struct pp_buffer_s *b = pp_resource_acquire(resource, PP_RESOURCE_BUFFER);
    if (!b)
        return NULL;

    ppb_core_add_ref_resource(resource);
    void *ptr = b->data;
    pp_resource_release(resource);
    return ptr;
}

void
ppb_buffer_unmap(PP_Resource resource)
{
    if (ppb_buffer_is_buffer(resource))
        ppb_core_release_resource(resource);
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_buffer_create(PP_Instance instance, uint32_t size_in_bytes)
{
    trace_info("[PPB] {full} %s instance=%d, size_in_bytes=%u\n", __func__+6,
               instance, size_in_bytes);
    return ppb_buffer_create(instance, size_in_bytes);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_buffer_is_buffer(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_buffer_is_buffer(resource);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_buffer_describe(PP_Resource resource, uint32_t *size_in_bytes)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_buffer_describe(resource, size_in_bytes);
}

TRACE_WRAPPER
void *
trace_ppb_buffer_map(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_buffer_map(resource);
}

TRACE_WRAPPER
void
trace_ppb_buffer_unmap(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    ppb_buffer_unmap(resource);
}


const struct PPB_Buffer_Dev_0_4 ppb_buffer_dev_interface_0_4 = {
    .Create =   TWRAPF(ppb_buffer_create),
    .IsBuffer = TWRAPF(ppb_buffer_is_buffer),
    .Describe = TWRAPF(ppb_buffer_describe),
    .Map =      TWRAPF(ppb_buffer_map),
    .Unmap =    TWRAPF(ppb_buffer_unmap),
};

static
void
__attribute__((constructor))
constructor_ppb_buffer(void)
{
    register_interface(PPB_BUFFER_DEV_INTERFACE_0_4, &ppb_buffer_dev_interface_0_4);
    register_resource(PP_RESOURCE_BUFFER, ppb_buffer_destroy);
}
