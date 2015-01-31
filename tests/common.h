#ifndef FPP_TESTS_COMMON_H
#define FPP_TESTS_COMMON_H

#include <src/pp_resource.h>

#ifndef TEST_DATA_DIR
#error TEST_DATA_DIR not defined
#endif


PP_Instance
create_instance(void);

void
destroy_instance(PP_Instance instance);


#endif // FPP_TESTS_COMMON_H
