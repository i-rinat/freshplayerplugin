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

#include "ppb_flash_file.h"
#include <stdlib.h>
#include "trace.h"


int32_t
ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    return -1;
}

int32_t
ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    return -1;
}


// trace wrappers
static
int32_t
trace_ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_file_file_ref_open_file(file_ref_id, mode, file);
}

static
int32_t
trace_ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_file_file_ref_query_file(file_ref_id, info);
}


const struct PPB_Flash_File_FileRef ppb_flash_file_file_ref_interface_2_0 = {
    .OpenFile =     trace_ppb_flash_file_file_ref_open_file,
    .QueryFile =    trace_ppb_flash_file_file_ref_query_file,
};
