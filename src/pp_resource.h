#ifndef FPP__PP_RESOURCE_H
#define FPP__PP_RESOURCE_H

#include <ppapi/c/pp_resource.h>
#include <ppapi/c/pp_completion_callback.h>
#include <stddef.h>

enum pp_resource_type_e {
    PP_RESOURCE_UNKNOWN = 0,
    PP_RESOURCE_URL_LOADER,
    PP_RESOURCE_URL_REQUEST_INFO,
    PP_RESOURCE_URL_RESPONSE_INFO,
};


struct pp_resource_generic_s {
    int         type;
    int         ref_cnt;
};

struct pp_url_loader_s {
    struct pp_resource_generic_s    _parent;
    const char     *headers;
    char           *body;
    size_t          body_size;
    size_t          body_allocated;
    int             loaded;         ///< if all stream loaded
    struct PP_CompletionCallback    ccb;
};

struct pp_url_request_info_s {
    struct pp_resource_generic_s _parent;
};

struct pp_url_response_info_s {
    struct pp_resource_generic_s _parent;
    PP_Resource     url_loader;
};


PP_Resource             pp_resource_allocate(enum pp_resource_type_e type);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);
void                    pp_resource_ref(PP_Resource resource);
void                    pp_resource_unref(PP_Resource resource);


#endif // FPP__PP_RESOURCE_H
