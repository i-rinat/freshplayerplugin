#include <inttypes.h>
#include <glib.h>
#include <ppapi/c/ppb_var.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"


static GHashTable *var_ht;

static
void
__attribute__((constructor))
constructor_ppb_var(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
}

static
void
__attribute__((destructor))
destructor_ppb_var(void)
{
    g_hash_table_unref(var_ht);
}

static
void
trace_var(const char *impl_status, const char *func, struct PP_Var var)
{
    switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
        trace_info("[PPB] %s %s var = {.type=UNDEFINED}\n", impl_status, func);
        break;
    case PP_VARTYPE_NULL:
        trace_info("[PPB] %s %s var = {.type=NULL}\n", impl_status, func);
        break;
    case PP_VARTYPE_BOOL:
        trace_info("[PPB] %s %s var = {.type=BOOL, .value.as_int=%d}\n",
                   impl_status, func, var.value.as_int);
        break;
    case PP_VARTYPE_INT32:
        trace_info("[PPB] %s %s var = {.type=INT32, .value.as_int=%d}\n",
                   impl_status, func, var.value.as_int);
    case PP_VARTYPE_DOUBLE:
        trace_info("[PPB] %s %s var = {.type=DOUBLE, .value.as_double=%f}\n",
                   impl_status, func, var.value.as_double);
        break;
    case PP_VARTYPE_STRING:
        trace_info("[PPB] %s %s var = {.type=STRING, .value.as_id(str)=%s}\n",
                   impl_status, func, (char *)(size_t)var.value.as_id);
        break;
    default:
        trace_info("[PPB] %s %s var = {.type=NOTIMPLEMENTED (%d)}\n",
                   impl_status, func, var.type);
        break;
    }
}

int
ref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt + 1));
    return refcnt + 1;
}

int
unref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt - 1));
    return refcnt - 1;
}

static
void
ppb_var_add_ref(struct PP_Var var)
{
    trace_var("{full}", __func__, var);
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        ref_var(var);
    }
}

static
void
ppb_var_release(struct PP_Var var)
{
    trace_var("{full}", __func__, var);
    if (var.type == PP_VARTYPE_STRING || var.type == PP_VARTYPE_OBJECT ||
        var.type == PP_VARTYPE_ARRAY || var.type == PP_VARTYPE_DICTIONARY ||
        var.type == PP_VARTYPE_ARRAY_BUFFER)
    {
        if (unref_var(var) == 0 && var.type == PP_VARTYPE_STRING)
            free((void*)(size_t)var.value.as_id);
    }
}

static
struct PP_Var
ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    trace_info("{full} %s data=%.*s, len=%d\n", __func__, len, data, len);
    struct PP_Var var = {0};
    char *cpy = malloc(len + 1);

    var.type = PP_VARTYPE_STRING;
    memcpy(cpy, data, len);
    cpy[len] = 0;
    var.value.as_id = (size_t)(void *)cpy;
    g_hash_table_insert(var_ht, cpy, GINT_TO_POINTER(1));
    return var;
}

static
struct PP_Var
ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    trace_info("{full} %s module=%d, data=%.*s, len=%d\n", __func__, module, len, data, len);
    struct PP_Var var = {0};
    char *cpy = malloc(len + 1);

    var.type = PP_VARTYPE_STRING;
    memcpy(cpy, data, len);
    cpy[len] = 0;
    var.value.as_id = (size_t)(void *)cpy;
    g_hash_table_insert(var_ht, cpy, GINT_TO_POINTER(1));

    return var;
}

static
const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    trace_var("{full}", __func__, var);
    if (PP_VARTYPE_STRING == var.type) {
        const char *s = (void *)(size_t)var.value.as_id;
        *len = strlen(s);
        return s;
    } else {
        trace_warning("var is not string in %s\n", __func__);
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
