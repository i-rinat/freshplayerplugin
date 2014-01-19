#include <inttypes.h>
#include <ppapi/c/ppb_var.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "tables.h"


void
ppb_var_add_ref(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__, var_str);
    free(var_str);
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        tables_ref_var(var);
    }
}

void
ppb_var_release(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__, var_str);
    free(var_str);
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        if (tables_unref_var(var) == 0 && var.type == PP_VARTYPE_STRING)
            free((void*)(size_t)var.value.as_id);
    }
}

struct PP_Var
ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s data=%.*s, len=%d\n", __func__, len, data, len);
    struct PP_Var var = {0};
    char *cpy = malloc(len + 1);

    var.type = PP_VARTYPE_STRING;
    memcpy(cpy, data, len);
    cpy[len] = 0;
    var.value.as_id = (size_t)(void *)cpy;
    tables_ref_var(var);
    return var;
}

struct PP_Var
ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s module=%d, data=%.*s, len=%d\n", __func__, module, len, data, len);
    struct PP_Var var = {0};
    char *cpy = malloc(len + 1);

    var.type = PP_VARTYPE_STRING;
    memcpy(cpy, data, len);
    cpy[len] = 0;
    var.value.as_id = (size_t)(void *)cpy;
    tables_ref_var(var);

    return var;
}

const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__, var_str);
    free(var_str);
    if (PP_VARTYPE_STRING == var.type) {
        const char *s = (void *)(size_t)var.value.as_id;
        *len = strlen(s);
        return s;
    } else {
        trace_warning("var is not a string in %s\n", __func__);
        *len = 0;
        return "";
    }
}


const struct PPB_Var_1_1 ppb_var_interface_1_1 = {
    .AddRef = ppb_var_add_ref,
    .Release = ppb_var_release,
    .VarFromUtf8 = ppb_var_var_from_utf8_1_1,
    .VarToUtf8 = ppb_var_var_to_utf8
};

const struct PPB_Var_1_0 ppb_var_interface_1_0 = {
    .AddRef = ppb_var_add_ref,
    .Release = ppb_var_release,
    .VarFromUtf8 = ppb_var_var_from_utf8_1_0,
    .VarToUtf8 = ppb_var_var_to_utf8,
};
