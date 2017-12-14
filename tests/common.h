#pragma once

#include <ppapi/c/pp_instance.h>

#ifndef TEST_DATA_DIR
#error TEST_DATA_DIR not defined
#endif

PP_Instance
create_instance(void);

void
destroy_instance(PP_Instance instance);
