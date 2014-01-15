#include "ppp_entry.h"
#include <unistd.h>
#include "np.h"
#include "trace.h"
#include "interface_list.h"

void *
pepper_plugin_thread_func(void *p)
{
    struct np_priv_s *priv = p;

    priv->ppp_instance_1_1->DidCreate(priv->pp_instance_id, priv->argc, priv->argn, priv->argv);

    trace_info("-----------------------------------------\n");
    PP_Resource urll = ppb_url_loader_interface_1_0.Create(priv->pp_instance_id);
    PP_Resource urlri = ppb_url_request_info_interface_1_0.Create(priv->pp_instance_id);
    ppb_url_loader_interface_1_0.Open(urll, urlri, PP_BlockUntilComplete());
    trace_info("=========================================\n");
    priv->ppp_instance_1_1->HandleDocumentLoad(priv->pp_instance_id, urll);

    while (1) {
        sleep(1);
    }

    return NULL;
}
