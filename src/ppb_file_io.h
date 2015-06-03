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

#ifndef FPP_PPB_FILE_IO_H
#define FPP_PPB_FILE_IO_H

#include <ppapi/c/ppb_file_io.h>
#include <ppapi/c/private/ppb_file_io_private.h>


int32_t
ppb_file_io_request_os_file_handle(PP_Resource file_io, PP_FileHandle *handle,
                                   struct PP_CompletionCallback callback);

PP_Resource
ppb_file_io_create(PP_Instance instance);

PP_Bool
ppb_file_io_is_file_io(PP_Resource resource);

int32_t
ppb_file_io_open(PP_Resource file_io, PP_Resource file_ref, int32_t open_flags,
                 struct PP_CompletionCallback callback);

int32_t
ppb_file_io_query(PP_Resource file_io, struct PP_FileInfo *info,
                  struct PP_CompletionCallback callback);

int32_t
ppb_file_io_touch(PP_Resource file_io, PP_Time last_access_time, PP_Time last_modified_time,
                  struct PP_CompletionCallback callback);

int32_t
ppb_file_io_read(PP_Resource file_io, int64_t offset, char *buffer, int32_t bytes_to_read,
                 struct PP_CompletionCallback callback);

int32_t
ppb_file_io_write(PP_Resource file_io, int64_t offset, const char *buffer, int32_t bytes_to_write,
                  struct PP_CompletionCallback callback);

int32_t
ppb_file_io_set_length(PP_Resource file_io, int64_t length, struct PP_CompletionCallback callback);

int32_t
ppb_file_io_flush(PP_Resource file_io, struct PP_CompletionCallback callback);

void
ppb_file_io_close(PP_Resource file_io);

int32_t
ppb_file_io_read_to_array(PP_Resource file_io, int64_t offset, int32_t max_read_length,
                          struct PP_ArrayOutput *output, struct PP_CompletionCallback callback);


#endif // FPP_PPB_FILE_IO_H
