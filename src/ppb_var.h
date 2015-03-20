/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#ifndef FPP_PPB_VAR_H
#define FPP_PPB_VAR_H

#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_var_array_buffer.h>
#include <ppapi/c/ppb_var_array.h>
#include <ppapi/c/ppb_var_dictionary.h>
#include <ppapi/c/dev/ppb_var_deprecated.h>
#include <string.h>
#include <npapi/npapi.h>
#include <npapi/npruntime.h>


void
ppb_var_add_ref(struct PP_Var var);

struct PP_Var
ppb_var_add_ref2(struct PP_Var var);

void
ppb_var_release(struct PP_Var var);

int
ppb_var_get_ref_count(struct PP_Var var);

struct PP_Var
ppb_var_var_from_utf8(const char *data, uint32_t len);

struct PP_Var
ppb_var_var_from_utf8_z(const char *data);

const char *
ppb_var_var_to_utf8(struct PP_Var var, uint32_t *len);

bool
ppb_var_has_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception);

bool
ppb_var_has_method(struct PP_Var object, struct PP_Var name, struct PP_Var *exception);

struct PP_Var
ppb_var_get_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception);

void
ppb_var_get_all_property_names(struct PP_Var object, uint32_t *property_count,
                               struct PP_Var **properties, struct PP_Var *exception);

void
ppb_var_set_property(struct PP_Var object, struct PP_Var name, struct PP_Var value,
                     struct PP_Var *exception);

void
ppb_var_remove_property(struct PP_Var object, struct PP_Var name, struct PP_Var *exception);

struct PP_Var
ppb_var_call(struct PP_Var object, struct PP_Var method_name, uint32_t argc, struct PP_Var *argv,
             struct PP_Var *exception);

struct PP_Var
ppb_var_construct(struct PP_Var object, uint32_t argc, struct PP_Var *argv,
                  struct PP_Var *exception);

bool
ppb_var_is_instance_of(struct PP_Var var, const struct PPP_Class_Deprecated *object_class,
                       void **object_data);

struct PP_Var
ppb_var_create_object(PP_Instance instance, const struct PPP_Class_Deprecated *object_class,
                      void *object_data);

struct PP_Var
ppb_var_create_object_with_module_deprecated(PP_Module module,
                                             const struct PPP_Class_Deprecated *object_class,
                                             void *object_data);

NPVariant
pp_var_to_np_variant(struct PP_Var var);

struct PP_Var
np_variant_to_pp_var(NPVariant v);

char *
ppb_var_trace_object_var(struct PP_Var var);

struct PP_Var
ppb_var_array_buffer_create(uint32_t size_in_bytes);

PP_Bool
ppb_var_array_buffer_byte_length(struct PP_Var var, uint32_t *byte_length);

void *
ppb_var_array_buffer_map(struct PP_Var var);

void
ppb_var_array_buffer_unmap(struct PP_Var var);

struct PP_Var
ppb_var_dictionary_create(void);

struct PP_Var
ppb_var_dictionary_get(struct PP_Var dict, struct PP_Var key);

PP_Bool
ppb_var_dictionary_set(struct PP_Var dict, struct PP_Var key, struct PP_Var value);

void
ppb_var_dictionary_delete(struct PP_Var dict, struct PP_Var key);

PP_Bool
ppb_var_dictionary_has_key(struct PP_Var dict, struct PP_Var key);

struct PP_Var
ppb_var_dictionary_get_keys(struct PP_Var dict);

struct PP_Var
ppb_var_array_create(void);

struct PP_Var
ppb_var_array_get(struct PP_Var array, uint32_t index);

PP_Bool
ppb_var_array_set(struct PP_Var array, uint32_t index, struct PP_Var value);

uint32_t
ppb_var_array_get_length(struct PP_Var array);

PP_Bool
ppb_var_array_set_length(struct PP_Var array, uint32_t length);


#endif // FPP_PPB_VAR_H
