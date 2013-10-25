#ifndef FFP__PP_RESOURCE_H
#define FFP__PP_RESOURCE_H

#include <ppapi/c/pp_resource.h>

enum pp_resource_type_e {
    PP_RESOURCE_UNKNOWN = 0,
    PP_RESOURCE_URL_LOADER,
    PP_RESOURCE_URL_REQUEST_INFO,
};


struct pp_resource_generic_s {
    int         type;
};

struct pp_url_loader_resource_s {
    struct pp_resource_generic_s _parent;
};

struct pp_url_request_info_resource_s {
    struct pp_resource_generic_s _parent;
};


PP_Resource             pp_resource_allocate(enum pp_resource_type_e type);
void                    pp_resource_expunge(PP_Resource resource);
void                   *pp_resource_acquire(PP_Resource resource);
void                    pp_resource_release(PP_Resource resource);
enum pp_resource_type_e pp_resource_get_type(PP_Resource resource);


#endif // FFP__PP_RESOURCE_H
