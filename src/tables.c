#include <glib.h>
#include "tables.h"


static GHashTable *var_ht;

static
void
__attribute__((constructor))
constructor_tables(void)
{
    var_ht = g_hash_table_new(g_direct_hash, g_direct_equal);
}

static
void
__attribute__((destructor))
destructor_tables(void)
{
    g_hash_table_unref(var_ht);
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
