#include "confuse.h"
#include <string.h>
#include <signal.h>
#include <unistd.h>

cfg_t *cfg = 0;
const char *config_filename = "./reread.conf";

void read_config(void)
{
    cfg_opt_t arg_opts[] = {
        CFG_STR("value", "default", CFGF_NONE),
        CFG_END()
    };
    cfg_opt_t opts[] = {
        CFG_INT("delay", 3, CFGF_NONE),
        CFG_STR("message", "This is a message", CFGF_NONE),
        CFG_SEC("argument", arg_opts, CFGF_MULTI | CFGF_TITLE),
        CFG_END()
    };
    int ret;

    char *buf = "" \
        " delay = 3\n" \
        "# message = \"asdfasfasfd tersf\"\n" \
        " argument one { value = 1 }\n" \
        " argument two { value=foo}\n";

    cfg_free(cfg);

    cfg = cfg_init(opts, 0);
    ret = cfg_parse_buf(cfg, buf);
    ret = cfg_parse(cfg, config_filename);
}

void sighandler(int sig)
{
    read_config();
    signal(SIGHUP, sighandler);
}

static int loop = 1;

void usr1handler(int sig)
{
    loop = 0;
}

int main(void)
{
    unsigned int i;

    read_config();
    signal(SIGHUP, sighandler);
    signal(SIGUSR1, usr1handler);

    while(loop)
    {
        printf("Message: %s", cfg_getstr(cfg, "message"));
        for(i = 0; i < cfg_size(cfg, "argument"); i++)
        {
            cfg_t *arg = cfg_getnsec(cfg, "argument", i);
            printf(", %s", cfg_getstr(arg, "value"));
        }
        printf("\n");

        sleep(cfg_getint(cfg, "delay"));
    }

    cfg_free(cfg);
    cfg = 0;

    return 0;
}
