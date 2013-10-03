#include <ppapi/c/ppb_file_ref.h>
#include <stddef.h>


const struct PPB_FileRef_1_0 ppb_file_ref_interface_1_0 = {
    .Create = (void *)256,
    .IsFileRef = (void *)257,
    .GetFileSystemType = (void *)258,
    .GetName = (void *)259,
    .GetPath = (void *)260,
    .GetParent = (void *)261,
    .MakeDirectory = (void *)262,
    .Touch = (void *)263,
    .Delete = (void *)264,
    .Rename = (void *)265,
};
