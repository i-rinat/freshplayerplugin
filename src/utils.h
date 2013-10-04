#ifndef FFP__UTILS_H
#define FFP__UTILS_H

#include <ppapi/c/pp_var.h>

void    ref_var(struct PP_Var var);
void    unref_var(struct PP_Var var);

#endif // FFP__UTILS_H
