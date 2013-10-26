#ifndef FFP__TRACE_H
#define FFP__TRACE_H

#include <ppapi/c/pp_var.h>
#include <ppapi/c/pp_rect.h>


void    trace_info(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
void    trace_warning(const char *fmt, ...) __attribute__((format (printf, 1, 2)));
char   *trace_var_as_string(struct PP_Var var);
char   *trace_rect_as_string(const struct PP_Rect *rect);
char   *trace_point_as_string(const struct PP_Point *point);

#endif // FFP__TRACE_H
