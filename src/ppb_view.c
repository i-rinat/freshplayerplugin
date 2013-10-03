#include <ppapi/c/ppb_view.h>
#include <stddef.h>


const struct PPB_View_1_0 ppb_view_interface_1_0 = {
    .IsView = (void *)1,
    .GetRect = (void *)2,
    .IsFullscreen = (void *)3,
    .IsVisible = (void *)4,
    .IsPageVisible = (void *)5,
    .GetClipRect = (void *)6,
};
