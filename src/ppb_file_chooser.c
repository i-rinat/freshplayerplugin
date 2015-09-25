/*
 * Copyright © 2013-2015  Rinat Ibragimov
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

#include "ppb_file_chooser.h"
#include "ppb_message_loop.h"
#include <stdlib.h>
#include "pp_resource.h"
#include "tables.h"
#include "trace.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include "ppb_file_ref.h"
#include "reverse_constant.h"
#include <ppapi/c/pp_errors.h>
#include <gdk/gdkx.h>
#include "pp_interface.h"


PP_Resource
ppb_file_chooser_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                        struct PP_Var accept_types)
{
    struct pp_instance_s *pp_i = tables_get_pp_instance(instance);
    if (!pp_i) {
        trace_error("%s, bad instance\n", __func__);
        return 0;
    }
    PP_Resource file_chooser = pp_resource_allocate(PP_RESOURCE_FILE_CHOOSER, pp_i);
    struct pp_file_chooser_s *fc = pp_resource_acquire(file_chooser, PP_RESOURCE_FILE_CHOOSER);
    if (!fc) {
        trace_error("%s, failed to create file chooser resource\n", __func__);
        return 0;
    }

    fc->mode = mode;
    fc->accept_types = accept_types;
    ppb_var_add_ref(accept_types);

    pp_resource_release(file_chooser);
    return file_chooser;
}

static
void
ppb_file_chooser_destroy(void *p)
{
    struct pp_file_chooser_s *fc = p;

    ppb_var_release(fc->accept_types);
}

PP_Bool
ppb_file_chooser_is_file_chooser(PP_Resource resource)
{
    return pp_resource_get_type(resource) == PP_RESOURCE_FILE_CHOOSER;
}

int32_t
ppb_file_chooser_show(PP_Resource chooser, struct PP_ArrayOutput output,
                      struct PP_CompletionCallback callback)
{
    return 0;
}

struct show_param_s {
    struct pp_instance_s           *pp_i;
    PP_Bool                         save_as;
    struct PP_Var                   suggested_file_name;
    struct PP_ArrayOutput           output;
    struct PP_CompletionCallback    ccb;
    PP_FileChooserMode_Dev          mode;
    struct PP_Var                   accept_types;
    PP_Resource                     chooser_id;
    PP_Resource                     message_loop;

    int                             dialog_closed;
};

static
void
fcd_response_handler(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    struct show_param_s *p = user_data;
    int32_t callback_result;

    if (response_id == GTK_RESPONSE_OK) {
        PP_Resource *file_refs, *file_ref;
        GSList *fname_lst = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(dialog));
        guint cnt = g_slist_length(fname_lst);

        callback_result = PP_OK;
        file_refs = p->output.GetDataBuffer(p->output.user_data, cnt, sizeof(PP_Resource));
        // TODO: what to do if file_refs == NULL?
        GSList *ll = fname_lst;
        file_ref = file_refs;
        while (ll) {
            *file_ref = ppb_file_ref_create_unrestricted((char *)ll->data, !p->save_as);
            ll = g_slist_next(ll);
            file_ref ++;
        }
        g_slist_free(fname_lst);
    } else {
        callback_result = PP_ERROR_USERCANCEL;
    }

    if (!p->dialog_closed)
        gtk_widget_destroy(GTK_WIDGET(dialog));

    ppb_message_loop_post_work_with_result(p->message_loop, p->ccb, 0, callback_result, 0,
                                           __func__);

    ppb_core_release_resource(p->chooser_id);
    g_slice_free(struct show_param_s, p);
}

static
void
fcd_close_handler(GtkDialog *arg0, gpointer user_data)
{
    struct show_param_s *p = user_data;
    p->dialog_closed = 1;
}

static
void
show_without_user_guesture_ptac(void *param)
{
    struct show_param_s *p = param;
    GtkWidget *fcd;
    const char *dialog_title;

    if (p->save_as) {
        dialog_title = "Save file";
    } else if (p->mode == PP_FILECHOOSERMODE_OPENMULTIPLE) {
        dialog_title = "Open files";
    } else {
        dialog_title = "Open file";
    }

#if GTK_MAJOR_VERSION == 3
    void *open_button_title = "_Open";
    void *close_button_title = "_Close";
#elif GTK_MAJOR_VERSION == 2
    void *open_button_title = GTK_STOCK_OPEN;
    void *close_button_title = GTK_STOCK_CANCEL;
#else
#error Unknown GTK version
#endif

    fcd = gtk_file_chooser_dialog_new(dialog_title, NULL,
                                      p->save_as ? GTK_FILE_CHOOSER_ACTION_SAVE
                                                 : GTK_FILE_CHOOSER_ACTION_OPEN,
                                      close_button_title, GTK_RESPONSE_CANCEL,
                                      open_button_title, GTK_RESPONSE_OK, NULL);

    if (p->mode == PP_FILECHOOSERMODE_OPENMULTIPLE)
        gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(fcd), 1);

    gtk_widget_realize(fcd);

    Window parent_wnd;
    if (npn.getvalue(p->pp_i->npp, NPNVnetscapeWindow, &parent_wnd) == NPERR_NO_ERROR) {
        GdkWindow *fcd_wnd = gtk_widget_get_window(fcd);

        XSetTransientForHint(GDK_WINDOW_XDISPLAY(fcd_wnd),
                             GDK_WINDOW_XID(fcd_wnd),
                             parent_wnd);
    } else {
        trace_warning("%s, can't get NPNVnetscapeWindow", __func__);
    }

    g_signal_connect(G_OBJECT(fcd), "response", G_CALLBACK(fcd_response_handler), p);
    g_signal_connect(G_OBJECT(fcd), "close", G_CALLBACK(fcd_close_handler), p);

    gtk_widget_show(fcd);
}

int32_t
ppb_file_chooser_show_without_user_gesture(PP_Resource chooser, PP_Bool save_as,
                                           struct PP_Var suggested_file_name,
                                           struct PP_ArrayOutput output,
                                           struct PP_CompletionCallback callback)
{
    struct pp_file_chooser_s *fc = pp_resource_acquire(chooser, PP_RESOURCE_FILE_CHOOSER);
    if (!fc) {
        trace_error("%s, bad resource\n", __func__);
        return PP_ERROR_BADRESOURCE;
    }

    struct show_param_s *p = g_slice_alloc0(sizeof(*p));
    p->pp_i =                   fc->instance;
    p->save_as =                save_as;
    p->suggested_file_name =    ppb_var_add_ref2(suggested_file_name);
    p->output =                 output;
    p->ccb =                    callback;
    p->mode =                   fc->mode;
    p->accept_types =           ppb_var_add_ref2(fc->accept_types);
    p->chooser_id =             chooser;
    p->message_loop =           ppb_message_loop_get_current();

    ppb_core_add_ref_resource(chooser);
    ppb_core_call_on_browser_thread(p->pp_i->id, show_without_user_guesture_ptac, p);

    pp_resource_release(chooser);
    return PP_OK_COMPLETIONPENDING;
}


// trace wrappers
TRACE_WRAPPER
PP_Resource
trace_ppb_file_chooser_create(PP_Instance instance, PP_FileChooserMode_Dev mode,
                              struct PP_Var accept_types)
{
    gchar *s_accept_types = trace_var_as_string(accept_types);
    trace_info("[PPB] {full} %s instance=%d, mode=%s(%u), accept_types=%s\n", __func__+6,
               instance, reverse_file_chooser_mode(mode), mode, s_accept_types);
    g_free(s_accept_types);
    return ppb_file_chooser_create(instance, mode, accept_types);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_file_chooser_is_file_chooser(PP_Resource resource)
{
    trace_info("[PPB] {full} %s resource=%d\n", __func__+6, resource);
    return ppb_file_chooser_is_file_chooser(resource);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_chooser_show(PP_Resource chooser, struct PP_ArrayOutput output,
                            struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {zilch} %s chooser=%d, output={.GetDataBuffer=%p, .user_data=%p}, "
               "callback={.func=%p, .user_data=%p, .flags=%u}\n", __func__+6, chooser,
               output.GetDataBuffer, output.user_data, callback.func, callback.user_data,
               callback.flags);
    return ppb_file_chooser_show(chooser, output, callback);
}

TRACE_WRAPPER
int32_t
trace_ppb_file_chooser_show_without_user_gesture(PP_Resource chooser, PP_Bool save_as,
                                                 struct PP_Var suggested_file_name,
                                                 struct PP_ArrayOutput output,
                                                 struct PP_CompletionCallback callback)
{
    gchar *s_suggested_file_name = trace_var_as_string(suggested_file_name);
    trace_info("[PPB] {full} %s chooser=%d, save_as=%u, suggested_file_name=%s, output="
               "{.GetDataBuffer=%p, .user_data=%p}, callback={.func=%p, .user_data=%p, "
               ".flags=%u}\n", __func__+6, chooser, save_as, s_suggested_file_name,
               output.GetDataBuffer, output.user_data, callback.func, callback.user_data,
               callback.flags);
    g_free(s_suggested_file_name);
    return ppb_file_chooser_show_without_user_gesture(chooser, save_as, suggested_file_name, output,
                                                      callback);
}


const struct PPB_FileChooser_Dev_0_6 ppb_file_chooser_dev_interface_0_6 = {
    .Create =           TWRAPF(ppb_file_chooser_create),
    .IsFileChooser =    TWRAPF(ppb_file_chooser_is_file_chooser),
    .Show =             TWRAPZ(ppb_file_chooser_show),
};

const struct PPB_FileChooserTrusted_0_6 ppb_file_chooser_trusted_interface_0_6 = {
    .ShowWithoutUserGesture = TWRAPF(ppb_file_chooser_show_without_user_gesture),
};

static
void
__attribute__((constructor))
constructor_ppb_file_chooser(void)
{
    register_interface(PPB_FILECHOOSER_DEV_INTERFACE_0_6, &ppb_file_chooser_dev_interface_0_6);
    register_interface(PPB_FILECHOOSER_TRUSTED_INTERFACE_0_6,
                       &ppb_file_chooser_trusted_interface_0_6);
    register_resource(PP_RESOURCE_FILE_CHOOSER, ppb_file_chooser_destroy);
}
