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
#include "tables.h"
#include "pp_resource.h"
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/ppb_file_io.h>
#include <errno.h>


static
int
pp_mode_to_open_mode(int32_t mode)
{
    int ret = 0;
    if ((mode & PP_FILEOPENFLAG_READ) && !(mode & PP_FILEOPENFLAG_WRITE))
        ret = O_RDONLY;
    if (!(mode & PP_FILEOPENFLAG_READ) && (mode & PP_FILEOPENFLAG_WRITE))
        ret = O_WRONLY;
    if ((mode & PP_FILEOPENFLAG_READ) && (mode & PP_FILEOPENFLAG_WRITE))
        ret = O_RDWR;
    if (mode & PP_FILEOPENFLAG_CREATE)
        ret |= O_CREAT;
    if (mode & PP_FILEOPENFLAG_TRUNCATE)
        ret |= O_TRUNC;
    if (mode & PP_FILEOPENFLAG_EXCLUSIVE)
        ret |= O_EXCL;
    if (mode & PP_FILEOPENFLAG_APPEND)
        ret |= O_APPEND;
    return ret;
}

int32_t
ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    struct pp_file_ref_s *fr = pp_resource_acquire(file_ref_id, PP_RESOURCE_FILE_REF);
    if (!fr) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    if (!fr->path) {
        trace_error("%s, resource have no path\n", __func__);
        pp_resource_release(file_ref_id);
        return PP_ERROR_FAILED;
    }

    int xmode = pp_mode_to_open_mode(mode);

    *file = open(fr->path, xmode, 0666);
    pp_resource_release(file_ref_id);

    if (*file > 0) {
        return PP_OK;
    } else {
        switch (errno) {
        case ENOENT: return PP_ERROR_FILENOTFOUND;
        case EACCES: return PP_ERROR_NOACCESS;
        default:     return PP_ERROR_FAILED;
        }
    }
}

int32_t
ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    return -1;
}


// trace wrappers
TRACE_WRAPPER
int32_t
trace_ppb_flash_file_file_ref_open_file(PP_Resource file_ref_id, int32_t mode, PP_FileHandle *file)
{
    trace_info("[PPB] {full} %s file_ref_id=%d, mode=%d\n", __func__+6, file_ref_id, mode);
    return ppb_flash_file_file_ref_open_file(file_ref_id, mode, file);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_file_file_ref_query_file(PP_Resource file_ref_id, struct PP_FileInfo *info)
{
    trace_info("[PPB] {zilch} %s\n", __func__+6);
    return ppb_flash_file_file_ref_query_file(file_ref_id, info);
}


const struct PPB_Flash_File_FileRef ppb_flash_file_file_ref_interface_2_0 = {
    .OpenFile =     TWRAPF(ppb_flash_file_file_ref_open_file),
    .QueryFile =    TWRAPZ(ppb_flash_file_file_ref_query_file),
};
