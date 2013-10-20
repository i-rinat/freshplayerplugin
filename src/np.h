#ifndef FPP__NP_H
#define FPP__NP_H

#include <ppapi/c/ppp_instance.h>
#include <X11/Xlib.h>
#include <npapi.h>

struct np_priv_s {
    const struct PPP_Instance_1_1  *ppp_instance_1_1;
    Window          wnd;
    PP_Instance     pp_instance_id;
    uint32_t        x;
    uint32_t        y;
    uint32_t        width;
    uint32_t        height;
    NPRect          clip_rect;
    void           *ws_info;
    NPWindowType    window_type;
    int             argc;
    const char    **argn;
    const char    **argv;
    int             instance_loaded;
    const char     *swf_fname;
    const char     *instance_url;
};

#endif // FPP_NP_H
