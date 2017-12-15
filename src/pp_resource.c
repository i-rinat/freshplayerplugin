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

#include "config.h"
#include "pp_resource.h"
#include "trace_core.h"
#include <glib.h>
#include <ppapi/c/pp_resource.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static GHashTable      *res_tbl;
static GHashTable      *destructors_ht = NULL;
static int              res_tbl_next = 0;
static pthread_mutex_t  res_tbl_lock = PTHREAD_MUTEX_INITIALIZER;

struct pp_resource_generic_s {
    COMMON_STRUCTURE_FIELDS
};

static
__attribute__((constructor))
void
constructor_pp_resource(void)
{
    pthread_mutex_lock(&res_tbl_lock);
    res_tbl = g_hash_table_new(g_direct_hash, g_direct_equal);
    res_tbl_next = 1;
    pthread_mutex_unlock(&res_tbl_lock);
}

PP_Resource
pp_resource_allocate(enum pp_resource_type_e type, struct pp_instance_s *instance)
{
    struct pp_resource_generic_s *res = g_slice_alloc0(LARGEST_RESOURCE_SIZE);
    res->resource_type = type;
    res->ref_cnt = 1;
    pthread_mutex_init(&res->lock, NULL);
    res->instance = instance;

    pthread_mutex_lock(&res_tbl_lock);
    res->self_id = res_tbl_next ++;
    g_hash_table_insert(res_tbl, GINT_TO_POINTER(res->self_id), res);
    pthread_mutex_unlock(&res_tbl_lock);

    return res->self_id;
}

void
pp_resource_expunge(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    void *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        g_slice_free1(LARGEST_RESOURCE_SIZE, ptr);
        g_hash_table_remove(res_tbl, GINT_TO_POINTER(resource));
    }
    pthread_mutex_unlock(&res_tbl_lock);
}

void *
pp_resource_acquire(PP_Resource resource, enum pp_resource_type_e type)
{
    struct pp_resource_generic_s *gr = NULL;
    while (1) {
        pthread_mutex_lock(&res_tbl_lock);
        gr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
        if (!gr || gr->resource_type != type) {
            gr = NULL;
            break;
        }
        if (pthread_mutex_trylock(&gr->lock) == 0)
            break;
        pthread_mutex_unlock(&res_tbl_lock);
        usleep(1);
    }

    // reference to avoid freeing acquired resource
    if (gr)
        gr->ref_cnt++;

    pthread_mutex_unlock(&res_tbl_lock);
    return gr;
}

void
pp_resource_release(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *gr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (gr) {
        pthread_mutex_unlock(&gr->lock);
    }
    pthread_mutex_unlock(&res_tbl_lock);

    // unref referenced in pp_resource_acquire()
    pp_resource_unref(resource);
}

enum pp_resource_type_e
pp_resource_get_type(PP_Resource resource)
{
    enum pp_resource_type_e type = PP_RESOURCE_UNKNOWN;
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        type = ptr->resource_type;
    }
    pthread_mutex_unlock(&res_tbl_lock);
    return type;
}

PP_Resource
pp_resource_ref(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        ptr->ref_cnt ++;
    } else {
        trace_warning("%s, no such resource %d\n", __func__, resource);
    }
    pthread_mutex_unlock(&res_tbl_lock);
    return resource;
}

static
void
count_resources_cb(gpointer key, gpointer value, gpointer user_data)
{
    int *counts = user_data;
    struct pp_resource_generic_s *r = value;

    if (r->resource_type < PP_RESOURCE_TYPES_COUNT)
        counts[r->resource_type] ++;
    else
        counts[PP_RESOURCE_TYPES_COUNT] ++;
}

void
pp_resource_unref(PP_Resource resource)
{
    void (*resource_destructor)(void *) = NULL;
    int ref_cnt = 0;

    pthread_mutex_lock(&res_tbl_lock);
    struct pp_resource_generic_s *ptr = g_hash_table_lookup(res_tbl, GINT_TO_POINTER(resource));
    if (ptr) {
        ref_cnt = --ptr->ref_cnt;

        // reference count should always be non-negative
        if (ref_cnt < 0)
            trace_error("%s, logic error, ref count gone negative\n", __func__);

        // prevent from being destroyed twice
        if (ref_cnt <= 0)
            g_hash_table_remove(res_tbl, GINT_TO_POINTER(resource));

        // find destructor
        resource_destructor = g_hash_table_lookup(destructors_ht,
                                                  GSIZE_TO_POINTER(ptr->resource_type));
    }
    pthread_mutex_unlock(&res_tbl_lock);

    if (!ptr)
        return;

    if (ref_cnt <= 0) {
        if (resource_destructor)
            resource_destructor(ptr);
        else
            trace_error("%s, no destructor for type %d\n", __func__, ptr->resource_type);

        // finally, free memory occupied by resource
        g_slice_free1(LARGEST_RESOURCE_SIZE, ptr);
    }

    if (config.quirks.dump_resource_histogram) {
        time_t current_time = time(NULL);
        static uintptr_t throttling = 0;

        if (current_time % 5 == 0) {
            if (!throttling) {
                int counts[PP_RESOURCE_TYPES_COUNT + 1] = {};

                pthread_mutex_lock(&res_tbl_lock);
                g_hash_table_foreach(res_tbl, count_resources_cb, counts);
                pthread_mutex_unlock(&res_tbl_lock);

                trace_error("-- %10lu ------------\n", (unsigned long)current_time);
                for (int k = 0; k < PP_RESOURCE_TYPES_COUNT; k ++)
                    if (counts[k] > 0)
                        trace_error("counts[%2d] = %d\n", k, counts[k]);
                if (counts[PP_RESOURCE_TYPES_COUNT] > 0)
                    trace_error("%d unknown resources (should never happen)\n",
                                counts[PP_RESOURCE_TYPES_COUNT]);
                trace_error("==========================\n");
                throttling = 1;
            }
        } else {
            throttling = 0;
        }
    }
}

void
register_resource(enum pp_resource_type_e type, void (*destructor)(void *ptr))
{
    pthread_mutex_lock(&res_tbl_lock);

    if (!destructors_ht)
        destructors_ht = g_hash_table_new(g_direct_hash, g_direct_equal);

    g_hash_table_insert(destructors_ht, GSIZE_TO_POINTER(type), (void *)destructor);

    pthread_mutex_unlock(&res_tbl_lock);
}

static
void
__attribute__((destructor))
destructor_pp_resource(void)
{
    g_hash_table_unref(destructors_ht);
    destructors_ht = NULL;
}
