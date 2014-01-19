#include <ppapi/c/dev/ppb_crypto_dev.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "trace.h"


static int rand_fd = -1;


static
void
__attribute__((constructor))
constructor_ppb_crypto_dev(void)
{
    rand_fd = open("/dev/urandom", O_RDONLY);
}

static
void
__attribute__((destructor))
destructor_ppb_crypto_dev(void)
{
    close(rand_fd);
}

void
ppb_crypto_dev_get_random_bytes(char *buffer, uint32_t num_bytes)
{
    trace_info("[PPB] {full} %s num_bytes=%d\n", __func__, num_bytes);
    read(rand_fd, buffer, num_bytes);
}

const struct PPB_Crypto_Dev_0_1 ppb_crypto_dev_interface_0_1 = {
    .GetRandomBytes = ppb_crypto_dev_get_random_bytes
};
