#include <glib.h>
#include "tables.h"


static GHashTable *var_ht;
static GHashTable *pp_to_np_ht;

static
void
__attribute__((constructor))
constructor_tables(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
    pp_to_np_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
}

static
void
__attribute__((destructor))
destructor_tables(void)
{
    g_hash_table_unref(var_ht);
    g_hash_table_unref(pp_to_np_ht);
}

int
tables_ref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt + 1));
    return refcnt + 1;
}

int
tables_unref_var(struct PP_Var var)
{
    int refcnt = GPOINTER_TO_INT(g_hash_table_lookup(var_ht, (void*)(size_t)var.value.as_id));
    g_hash_table_replace(var_ht, (void*)(size_t)var.value.as_id, GINT_TO_POINTER(refcnt - 1));
    return refcnt - 1;
}

struct np_priv_s *
tables_pp_instance_to_np_priv(PP_Instance instance)
{
    return g_hash_table_lookup(pp_to_np_ht, GINT_TO_POINTER(instance));
}

void
tables_add_pp_np_mapping(PP_Instance instance, struct np_priv_s *priv)
{
    g_hash_table_replace(pp_to_np_ht, GINT_TO_POINTER(instance), priv);
}
