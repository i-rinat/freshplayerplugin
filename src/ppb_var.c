#include <ppapi/c/ppb_var.h>
#include <stddef.h>
#include "trace.h"

static
void
ppb_var_add_ref(struct PP_Var var)
{
    trace_info("{zilch} %s\n", __func__);
}

static
void
ppb_var_release(struct PP_Var var)
{
    trace_info("{zilch} %s\n", __func__);
}

static
struct PP_Var
ppb_var_var_from_utf8(const char *data, uint32_t len)
{
    trace_info("{zilch} %s\n        data = %.*s, len = %d\n", __func__, len, data, len);
    struct PP_Var var = {0};
    return var;
}

static
const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    trace_info("{zilch} %s\n", __func__);
    *len = 5;
    return "zilch";
}


const struct PPB_Var_1_1 ppb_var_interface_1_1 = {
    .AddRef = ppb_var_add_ref,
    .Release = ppb_var_release,
    .VarFromUtf8 = ppb_var_var_from_utf8,
    .VarToUtf8 = ppb_var_var_to_utf8
};
