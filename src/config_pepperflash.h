#ifndef FPP_CONFIG_PEPPERFLASH_H
#define FPP_CONFIG_PEPPERFLASH_H

typedef struct version {
    int v1;
    int v2;
    int v3;
    int v4;
} version;

int
is_newer_version(const char *, version *);
#endif // FPP_CONFIG_PEPPERFLASH_H
