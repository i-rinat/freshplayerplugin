#ifndef FPP__PP_INTERFACE_H
#define FPP__PP_INTERFACE_H

extern const void *(*ppp_get_interface)(const char *interface_name);

const void *
ppb_get_interface(const char *interface_name);

#endif // FPP__PP_INTERFACE_H
