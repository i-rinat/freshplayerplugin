#include <stdio.h>

char *
NP_GetPluginVersion(void);

int
main(void)
{
    printf("version: %s\n", NP_GetPluginVersion());
    printf("pass\n");
    return 0;
}
