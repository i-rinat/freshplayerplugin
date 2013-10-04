#include <stdarg.h>
#include <stdio.h>
#include "trace.h"


void
trace_info(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "[fresh] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void
trace_warning(const char *fmt, ...)
{
    va_list args;
    fprintf(stdout, "[fresh] [warning] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}
