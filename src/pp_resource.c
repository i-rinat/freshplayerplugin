#include "pp_resource.h"
#include <glib.h>
#include <stdlib.h>


static GArray          *res_tbl;
static pthread_mutex_t  res_tbl_lock = PTHREAD_MUTEX_INITIALIZER;

static
__attribute__((constructor))
void
pp_resource_constructor(void)
{
    void *null_val = NULL;
    res_tbl = g_array_new(FALSE, FALSE, sizeof(void*));
    g_array_append_val(res_tbl, null_val);
}

PP_Resource
pp_resource_allocate(enum pp_resource_type_e type)
{
    void *ptr;

#define ALLOC_HELPER(typename)              \
    ptr = calloc(sizeof(typename), 1);      \
    ((typename *)ptr)->_parent.type = type; \
    ((typename *)ptr)->_parent.ref_cnt = 1;

    pthread_mutex_lock(&res_tbl_lock);
    switch (type) {
    case PP_RESOURCE_URL_LOADER:
        ALLOC_HELPER(struct pp_url_loader_s);
        break;
    case PP_RESOURCE_URL_REQUEST_INFO:
        ALLOC_HELPER(struct pp_url_request_info_s);
        break;
    case PP_RESOURCE_URL_RESPONSE_INFO:
        ALLOC_HELPER(struct pp_url_response_info_s);
        break;
    default:
        // fall through
    case PP_RESOURCE_UNKNOWN:
        ptr = calloc(sizeof(struct pp_resource_generic_s), 1);
        ((struct pp_resource_generic_s *)ptr)->type = type;
        break;
    }

    int handle = res_tbl->len;
    g_array_append_val(res_tbl, ptr);

    pthread_mutex_unlock(&res_tbl_lock);
    return handle;
}

void
pp_resource_expunge(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    if (resource < 1 || resource >= res_tbl->len) {
        pthread_mutex_unlock(&res_tbl_lock);
        return;
    }

    void **ptr = &g_array_index(res_tbl, void *, resource);
    free(*ptr);
    *ptr = NULL;
    pthread_mutex_unlock(&res_tbl_lock);
}

void *
pp_resource_acquire(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    if (resource < 1 || resource >= res_tbl->len) {
        pthread_mutex_unlock(&res_tbl_lock);
        return NULL;
    }

    // TODO: add mutexes for every resource
    // TODO: lock mutex of particular resource handle
    struct pp_resource_generic_s *ptr = g_array_index(res_tbl, void*, resource);

    pthread_mutex_unlock(&res_tbl_lock);
    return ptr;
}

void
pp_resource_release(PP_Resource resource)
{
    // TODO: unlock mutex of particular resource handle
}

enum pp_resource_type_e
pp_resource_get_type(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    if (resource < 1 || resource >= res_tbl->len) {
        pthread_mutex_unlock(&res_tbl_lock);
        return PP_RESOURCE_UNKNOWN;
    }

    struct pp_resource_generic_s *ptr = g_array_index(res_tbl, void*, resource);
    enum pp_resource_type_e type = ptr->type;

    pthread_mutex_unlock(&res_tbl_lock);
    return type;
}

void
pp_resource_ref(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    if (resource < 1 || resource >= res_tbl->len) {
        pthread_mutex_unlock(&res_tbl_lock);
        return;
    }
    struct pp_resource_generic_s *ptr = g_array_index(res_tbl, void*, resource);
    ptr->ref_cnt ++;
    pthread_mutex_unlock(&res_tbl_lock);
}

void
pp_resource_unref(PP_Resource resource)
{
    pthread_mutex_lock(&res_tbl_lock);
    if (resource < 1 || resource >= res_tbl->len) {
        pthread_mutex_unlock(&res_tbl_lock);
        return;
    }
    struct pp_resource_generic_s *ptr = g_array_index(res_tbl, void*, resource);
    ptr->ref_cnt --;
    pthread_mutex_unlock(&res_tbl_lock);

    if (0 == ptr->ref_cnt)
        pp_resource_expunge(resource);
}
