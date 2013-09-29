#include <ppapi/c/dev/ppb_memory_dev.h>
#include <stddef.h>


const PPB_Memory_Dev ppb_memory_dev = {
    .MemAlloc = NULL,
    .MemFree = NULL
};
