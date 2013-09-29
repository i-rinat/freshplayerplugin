#include <ppapi/c/dev/ppb_cursor_control_dev.h>
#include <stddef.h>


const PPB_CursorControl_Dev ppb_cursor_control_dev = {
    .SetCursor = NULL,
    .LockCursor = NULL,
    .UnlockCursor = NULL,
    .HasCursorLock = NULL,
    .CanLockCursor = NULL
};
