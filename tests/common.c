#include "common.h"
#include <src/tables.h>

PP_Instance
create_instance(void)
{
    static int id = 10;
    struct pp_instance_s *pp_i = calloc(sizeof(*pp_i), 1);
    pp_i->id = id;
    tables_add_pp_instance(id, pp_i);
    return id++;
}

void
destroy_instance(PP_Instance instance)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    tables_remove_pp_instance(instance);
    free(pp_i);
}
