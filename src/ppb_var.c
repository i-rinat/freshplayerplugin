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
    trace_info("{full} %s\n        var = {.type = %d, .value.as_int = %d, .value.as_double = %f, "
               ".value.as_id = 0x%" PRIx64 "}\n", __func__, var.type, var.value.as_int,
               var.value.as_double, var.value.as_id);
    switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
    case PP_VARTYPE_NULL:
    case PP_VARTYPE_BOOL:
    case PP_VARTYPE_INT32:
    case PP_VARTYPE_DOUBLE:
        // do nothing
        return;
    default:
        ref_var(var);
    }
}

static
void
ppb_var_release(struct PP_Var var)
{
    trace_info("{full} %s\n        var = {.type = %d, .value.as_int = %d, .value.as_double = %f, "
               ".value.as_id = 0x%" PRIx64 "}\n", __func__, var.type, var.value.as_int,
               var.value.as_double, var.value.as_id);

    switch (var.type) {
    case PP_VARTYPE_UNDEFINED:
    case PP_VARTYPE_NULL:
    case PP_VARTYPE_BOOL:
    case PP_VARTYPE_INT32:
    case PP_VARTYPE_DOUBLE:
        // do nothing
        return;
    default:
        if (unref_var(var) == 0 && var.type == PP_VARTYPE_STRING)
            free((void*)(size_t)var.value.as_id);
    }
}

static
struct PP_Var
ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    trace_info("{full} %s\n        data = %.*s, len = %d\n", __func__, len, data, len);
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
    trace_info("{full} %s\n        module = %d, data = %.*s, len = %d\n", __func__,
               module, len, data, len);
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
    trace_info("{full} %s\n        var = {.type = %d, .value.as_int = %d, .value.as_double = %f, "
               ".value.as_id = 0x%" PRIx64 "}\n", __func__, var.type, var.value.as_int,
               var.value.as_double, var.value.as_id);
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
