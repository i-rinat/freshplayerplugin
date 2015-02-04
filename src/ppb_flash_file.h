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

#ifndef FPP_PPB_FLASH_FILE_H
#define FPP_PPB_FLASH_FILE_H

#include <ppapi/c/dev/deprecated_bool.h>
#include <ppapi/c/private/ppb_flash_file.h>


int32_t
ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file);

int32_t
ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info);

bool
ppb_flash_file_modulelocal_create_thread_adapter_for_instance(PP_Instance instance);

void
ppb_flash_file_modulelocal_clear_thread_adapter_for_instance(PP_Instance instance);

int32_t
ppb_flash_file_modulelocal_open_file(PP_Instance instance, const char *path, int32_t mode,
                                     PP_FileHandle *file);

int32_t
ppb_flash_file_modulelocal_rename_file(PP_Instance instance, const char *path_from,
                                       const char *path_to);

int32_t
ppb_flash_file_modulelocal_delete_file_or_dir(PP_Instance instance, const char *path,
                                              PP_Bool recursive);

int32_t
ppb_flash_file_modulelocal_create_dir(PP_Instance instance, const char *path);

int32_t
ppb_flash_file_modulelocal_query_file(PP_Instance instance, const char *path,
                                      struct PP_FileInfo *info);

int32_t
ppb_flash_file_modulelocal_get_dir_contents(PP_Instance instance, const char *path,
                                            struct PP_DirContents_Dev **contents);

void
ppb_flash_file_modulelocal_free_dir_contents(PP_Instance instance,
                                             struct PP_DirContents_Dev *contents);

int32_t
ppb_flash_file_modulelocal_create_temporary_file(PP_Instance instance, PP_FileHandle *file);


#endif // FPP_PPB_FLASH_FILE_H
