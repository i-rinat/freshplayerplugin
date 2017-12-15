/*
 * Copyright © 2013-2017  Rinat Ibragimov
 *
 * This file is part of FreshPlayerPlugin.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "compat.h"
#include "config.h"
#include "config_priv.h"
#include "gtk_wrapper.h"
#include "main_thread.h"
#include "np_asynccall.h"
#include "pp_interface.h"
#include "ppb_core.h"
#include "ppb_instance.h"
#include "ppb_message_loop.h"
#include "reverse_constant.h"
#include "tables.h"
#include "trace_core.h"
#include "utils.h"
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <glib.h>
#include <npapi/npapi.h>
#include <npapi/npfunctions.h>
#include <ppapi/c/pp_bool.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/ppb.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

static void *module_dl_handler;
static struct pp_instance_s *aux_instance = NULL;
static int np_initialize_was_called = 0;

struct call_plugin_init_module_param_s {
    PP_Resource     m_loop;
    int             depth;
    int32_t       (*ppp_initialize_module)(PP_Module module_id,
                                           PPB_GetInterface get_browser_interface);
    int             result;
};

static
void
call_plugin_init_module_comt(void *user_data, int32_t result)
{
    struct call_plugin_init_module_param_s *p = user_data;

    // TODO: make module ids distinct
    // p->ppp_initialize_module is always non-NULL
    p->result = p->ppp_initialize_module(42, ppb_get_interface);

    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
call_plugin_init_module_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(call_plugin_init_module_comt, user_data), PP_OK,
                                       __func__);
}

static
int
call_plugin_init_module(void)
{
    int32_t   (*ppp_initialize_module)(PP_Module module_id, PPB_GetInterface get_browser_interface);

    if (!module_dl_handler)
        return 0;

    ppp_initialize_module = dlsym(module_dl_handler, "PPP_InitializeModule");
    if (!ppp_initialize_module)
        return 0;

    struct call_plugin_init_module_param_s *p = g_slice_alloc(sizeof(*p));
    p->m_loop =                ppb_message_loop_get_for_browser_thread();
    p->depth =                 ppb_message_loop_get_depth(p->m_loop) + 1;
    p->ppp_initialize_module = ppp_initialize_module;

    ppb_message_loop_post_work_with_result(p->m_loop,
                                           PP_MakeCCB(call_plugin_init_module_prepare_comt, p), 0,
                                           PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);
    int res = p->result;
    g_slice_free1(sizeof(*p), p);

    return res;
}

static int
probe_ppp_module(void)
{
    fpp_config_initialize();

    if (!fpp_config_get_plugin_path()) {
        config.quirks.plugin_missing = 1;
        trace_error("%s, can't find %s\n", __func__, fpp_config_get_plugin_file_name());
        return 1;
    }

    return 0;
}

static
uintptr_t
load_ppp_module()
{
    if (module_dl_handler) {
        // already loaded
        return 0;
    }

    // ensure we have a module name
    probe_ppp_module();
    if (!fpp_config_get_plugin_path())
        goto err;

    module_dl_handler = dlopen(fpp_config_get_plugin_path(), RTLD_LAZY);
    if (!module_dl_handler) {
        trace_info_f("%s, can't open %s\n", __func__, fpp_config_get_plugin_path());
        goto err;
    }

    int32_t (*ppp_initialize_module)(PP_Module module_id, PPB_GetInterface get_browser_interface);
    ppp_initialize_module = dlsym(module_dl_handler, "PPP_InitializeModule");
    ppp_get_interface = dlsym(module_dl_handler, "PPP_GetInterface");

    if (!ppp_initialize_module || !ppp_get_interface) {
        trace_error("%s, one of required PPP_* is missing\n", __func__);
        if (module_dl_handler)
            dlclose(module_dl_handler);
        module_dl_handler = NULL;
        goto err;
    }

    // allocate auxiliary instance
    if (!aux_instance) {
        aux_instance = calloc(1, sizeof(*aux_instance));
        if (!aux_instance)
            goto err;

        aux_instance->id = tables_generate_new_pp_instance_id();
        tables_add_pp_instance(aux_instance->id, aux_instance);
    }

    // allocate message loop for browser thread
    if (ppb_message_loop_get_current() == 0) {
        PP_Resource message_loop = ppb_message_loop_create(aux_instance->id);
        ppb_message_loop_attach_to_current_thread(message_loop);
        ppb_message_loop_proclaim_this_thread_browser();
    }

    // allocate message loop for plugin thread (main thread)
    if (ppb_message_loop_get_for_main_thread() == 0) {
        pthread_barrier_init(&aux_instance->main_thread_barrier, NULL, 2);
        pthread_create(&aux_instance->main_thread, NULL, fresh_wrapper_main_thread, aux_instance);
        pthread_detach(aux_instance->main_thread);
        pthread_barrier_wait(&aux_instance->main_thread_barrier);
        pthread_barrier_destroy(&aux_instance->main_thread_barrier);
    }

    return 0;

err:
    config.quirks.plugin_missing = 1;
    return 1;
}

struct call_plugin_shutdown_module_param_s {
    PP_Resource m_loop;
    int         depth;
    void      (*ppp_shutdown_module)(void);
};

static
void
call_plugin_shutdown_module_comt(void *user_data, int32_t result)
{
    struct call_plugin_shutdown_module_param_s *p = user_data;
    p->ppp_shutdown_module();   // p->ppp_shutdown_module is always non-NULL
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
call_plugin_shutdown_module_prepare_comt(void *user_data, int32_t result)
{
    ppb_core_trampoline_to_main_thread(PP_MakeCCB(call_plugin_shutdown_module_comt, user_data),
                                       PP_OK, __func__);
}

static
void
call_plugin_shutdown_module(void)
{
    if (!module_dl_handler)
        return;

    void (*ppp_shutdown_module)(void);
    ppp_shutdown_module = dlsym(module_dl_handler, "PPP_ShutdownModule");
    if (!ppp_shutdown_module)
        return;

    struct call_plugin_shutdown_module_param_s *p = g_slice_alloc(sizeof(*p));
    p->m_loop =              ppb_message_loop_get_for_browser_thread();
    p->depth =               ppb_message_loop_get_depth(p->m_loop) + 1;
    p->ppp_shutdown_module = ppp_shutdown_module;

    ppb_message_loop_post_work_with_result(p->m_loop,
                                           PP_MakeCCB(call_plugin_shutdown_module_prepare_comt, p),
                                           0, PP_OK, p->depth, __func__);
    ppb_message_loop_run_nested(p->m_loop);
    g_slice_free1(sizeof(*p), p);
}

static
void
unload_ppp_module(void)
{
    // call module shutdown handler if exists
    call_plugin_shutdown_module();

    if (module_dl_handler)
        dlclose(module_dl_handler);
    module_dl_handler = NULL;

    fpp_config_destroy();
}


const char *
NP_GetMIMEDescription(void)
{
    trace_info_f("[NP] %s\n", __func__);
    return fpp_config_get_plugin_mime_type();
}

char *
NP_GetPluginVersion(void)
{
    trace_info_f("[NP] %s\n", __func__);
    probe_ppp_module();
    return (char *)fpp_config_get_plugin_version();
}

NPError
NP_GetValue(void *instance, NPPVariable variable, void *value)
{
    trace_info_f("[NP] %s instance=%p, variable=%s, value=%p\n", __func__, instance,
                 reverse_npp_variable(variable), value);

    probe_ppp_module();

    switch (variable) {
    case NPPVpluginNameString:
        *(const char **)value = fpp_config_get_plugin_name();
        break;
    case NPPVpluginDescriptionString:
        *(const char **)value = fpp_config_get_plugin_descr();
        break;
    default:
        trace_info_z("    not implemented variable %d\n", variable);
    }
    return NPERR_NO_ERROR;
}

static
int
x_error_handler(Display *dpy, XErrorEvent *ee)
{
    trace_error("[NP] caught Xlib error %d\n", ee->error_code);
    return 0;
}

static
int
x_io_error_hanlder(Display *dpy)
{
    // IO errors can't be ignored, they always terminate program.
    // Let's crash here to get core file!
    trace_error("[NP] got Xlib IO error\n");
    abort();
    return 0;
}

static
void
call_gdb_signal_handler(int sig, siginfo_t *si, void *p)
{
    static char cmd[4096];

    pid_t pid = getpid();
    time_t now = time(NULL);

    // ask gdb to debug this process
    snprintf(cmd, sizeof(cmd), "gdb --pid %d"
             " -ex 'set logging file /tmp/freshwrapper-backtrace-%d-%d.txt'"
             " -ex 'set logging on'"
             " -ex 'set pagination off'"
             " -ex 'echo === backtrace triggered by signal %d ===\\n'"
             " -ex 'echo === current thread ===\\n'"
             " -ex bt"
             " -ex 'echo === thread list ===\\n'"
             " -ex 'info threads'"
             " -ex 'echo === all threads ===\\n'"
             " -ex 'thread apply all bt full'"
             " -ex 'set confirm off'"
             " -ex 'quit'",
             (int)pid, (int)now, (int)pid, sig);

    // call gdb
    int ret = system(cmd);
    if (ret != 0)
        printf("gdb return status: %d\n", ret);

    exit(sig);
}

static
void
setup_sig_handlers(void)
{
    int s[] = { SIGSEGV, SIGILL, SIGABRT };
    struct sigaction sa = {};

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    for (uintptr_t k = 0; k < sizeof(s) / sizeof(s[0]); k ++)
        sigaddset(&sa.sa_mask, s[k]);
    sa.sa_sigaction = call_gdb_signal_handler;

    for (uintptr_t k = 0; k < sizeof(s) / sizeof(s[0]); k ++) {
        struct sigaction prev;

        // ensure there were no handlers before
        if (sigaction(s[k], NULL, &prev) != 0)
            continue;

        if ((prev.sa_flags & SA_SIGINFO) && prev.sa_sigaction != NULL)
            continue;

        if (!(prev.sa_flags & SA_SIGINFO) && prev.sa_handler != NULL)
            continue;

        // install own handler
        if (sigaction(s[k], &sa, NULL) != 0)
            trace_error("%s, can't set signal %d handler\n", __func__, s[k]);
    }
}

NPError
NP_Initialize(NPNetscapeFuncs *aNPNFuncs, NPPluginFuncs *aNPPFuncs)
{
    trace_info_f("[NP] %s aNPNFuncs=%p, aNPPFuncs=%p, browser API version = %u\n", __func__,
                 aNPNFuncs, aNPPFuncs, aNPNFuncs->version);

    if (np_initialize_was_called) {
        trace_warning("NP_Initialize was called more than once\n");
        return NPERR_NO_ERROR;
    }

    np_initialize_was_called = 1;

    setup_sig_handlers();
    gtk_wrapper_initialize();

    if (!gw_gtk_available()) {
        trace_error("no GTK+ loaded\n");
        return NPERR_NO_ERROR;
    }

    trace_info_f("found GTK+ %d.%d\n", gw_major_version(), gw_minor_version());

    // set logging-only error handler.
    // Ignore a previous one, we have no plans to restore it
    (void)XSetErrorHandler(x_error_handler);
    (void)XSetIOErrorHandler(x_io_error_hanlder);

    memset(&npn, 0, sizeof(npn));
    memcpy(&npn, aNPNFuncs, sizeof(npn) < aNPNFuncs->size ? sizeof(npn) : aNPNFuncs->size);

    if (npn.pluginthreadasynccall == NULL) {
        trace_info_f("browser have npn.pluginthreadasynccall == NULL\n");
        if (np_asynccall_initialize() != 0) {
            trace_error("can't initialize async call emulation\n");

            // It's required, can't continue.
            return NPERR_GENERIC_ERROR;
        }

        npn.pluginthreadasynccall = np_asynccall_call;
    }

    NPPluginFuncs pf;
    memset(&pf, 0, sizeof(NPPluginFuncs));
    pf.size = MIN(aNPPFuncs->size, sizeof(NPPluginFuncs));

    // browser is supposed to fill .size and .version
    pf.newp =               NPP_New;
    pf.destroy =            NPP_Destroy;
    pf.setwindow =          NPP_SetWindow;
    pf.newstream =          NPP_NewStream;
    pf.destroystream =      NPP_DestroyStream;
    pf.asfile =             NPP_StreamAsFile;
    pf.writeready =         NPP_WriteReady;
    pf.write =              NPP_Write;
    pf.print =              NPP_Print;
    pf.event =              NPP_HandleEvent;
    pf.urlnotify =          NPP_URLNotify;
    pf.getvalue =           NPP_GetValue;
    pf.setvalue =           NPP_SetValue;
    pf.gotfocus =           NPP_GotFocus;
    pf.lostfocus =          NPP_LostFocus;
    pf.urlredirectnotify =  NPP_URLRedirectNotify;
    pf.clearsitedata =      NPP_ClearSiteData;
    pf.getsiteswithdata =   NPP_GetSitesWithData;
    pf.didComposite =       NPP_DidComposite;

    memcpy(aNPPFuncs, &pf, pf.size);

    if (aNPNFuncs->version < NPVERS_HAS_PLUGIN_THREAD_ASYNC_CALL) {
        config.quirks.plugin_missing = 1;
        config.quirks.incompatible_npapi_version = 1;
    }

    load_ppp_module();

    if (tables_open_display() != 0)
        return NPERR_GENERIC_ERROR;

    int res = call_plugin_init_module();
    if (res != 0) {
        trace_error("%s, PPP_InitializeModule returned %d\n", __func__, res);
        return NPERR_GENERIC_ERROR;
    }

    return NPERR_NO_ERROR;
}

NPError
NP_Shutdown(void)
{
    trace_info_f("[NP] %s\n", __func__);

    unload_ppp_module();
    tables_close_display();

    return NPERR_NO_ERROR;
}
