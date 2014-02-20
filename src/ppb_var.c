/*
 * Copyright © 2013-2014  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

// trace wrappers
static
void
trace_ppb_var_add_ref(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    free(var_str);
    ppb_var_add_ref(var);
}

static
void
trace_ppb_var_release(struct PP_Var var)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    free(var_str);
    ppb_var_release(var);
}

static
struct PP_Var
trace_ppb_var_var_from_utf8_1_1(const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s data=%.*s, len=%d\n", __func__+6, len, data, len);
    return ppb_var_var_from_utf8_1_1(data, len);
}

static
struct PP_Var
trace_ppb_var_var_from_utf8_1_0(PP_Module module, const char *data, uint32_t len)
{
    trace_info("[PPB] {full} %s module=%d, data=%.*s, len=%d\n", __func__+6, module, len, data,len);
    return ppb_var_var_from_utf8_1_0(module, data, len);
}

static
const char *
trace_ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len)
{
    char *var_str = trace_var_as_string(var);
    trace_info("[PPB] {full} %s var=%s\n", __func__+6, var_str);
    free(var_str);
    return ppb_var_var_to_utf8(var, len);
}


const struct PPB_Var_1_1 ppb_var_interface_1_1 = {
    .AddRef =       trace_ppb_var_add_ref,
    .Release =      trace_ppb_var_release,
    .VarFromUtf8 =  trace_ppb_var_var_from_utf8_1_1,
    .VarToUtf8 =    trace_ppb_var_var_to_utf8
};

const struct PPB_Var_1_0 ppb_var_interface_1_0 = {
    .AddRef =       trace_ppb_var_add_ref,
    .Release =      trace_ppb_var_release,
    .VarFromUtf8 =  trace_ppb_var_var_from_utf8_1_0,
    .VarToUtf8 =    trace_ppb_var_var_to_utf8,
};
