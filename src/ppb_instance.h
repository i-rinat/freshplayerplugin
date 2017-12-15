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

#pragma once

#include "gtk_wrapper.h"
#include <glib.h>
#include <npapi/npapi.h>
#include <npapi/npruntime.h>
#include <ppapi/c/dev/ppb_text_input_dev.h>
#include <ppapi/c/pp_completion_callback.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/private/ppb_instance_private.h>
#include <pthread.h>
#include <stdint.h>

struct pp_instance_s {
    const struct PPP_Instance_1_1  *ppp_instance_1_1;
    const struct PPP_InputEvent_0_1 *ppp_input_event;
    const struct PPP_Instance_Private_0_1 *ppp_instance_private;
    const struct PPP_TextInput_Dev_0_1    *ppp_text_input_dev;
    struct PP_Var                   scriptable_pp_obj;
    NPObject                       *np_window_obj;
    NPObject                       *np_plugin_element_obj;
    uint32_t                        event_mask;
    uint32_t                        filtered_event_mask;
    Window                          wnd;
    Window                          browser_wnd;
    GtkWidget                      *catcher_widget; ///< catches keypresses returned from IME
    PP_Instance                     id;
    NPP                             npp;
    uint32_t                        is_fullframe;
    uint32_t                        is_fullscreen;
    uint32_t                        is_fullscreen_apparent;
    uint32_t                        is_transparent;
    uint32_t                        windowed_mode;
    uint32_t                        use_xembed;
    uint32_t                        incognito_mode;
    volatile gint                   instance_loaded;
    uint32_t                        ignore_focus_events_cnt; ///< number of focus events to ignore
    unsigned long                   last_button_release_timestamp;
    PP_Resource                     content_url_loader;
    uint32_t                        content_url_loader_used;
    volatile gint                   audio_source_count; ///< number of currently active audiosources
    volatile gint                   is_muted;

    Cursor                          prev_cursor;
    int                             have_prev_cursor;
    volatile gint                   cursor_inside_instance;

    // full screen
    pthread_t           fs_thread;
    Window              fs_wnd;     ///< fullscreen window handle
    uint32_t            fs_width;   ///< established width of fullscreen window
    uint32_t            fs_height;  ///< established height of fullscreen window

    // current geometry of fullscreen window. Can change multiple times during transition
    uint32_t            fs_width_current;
    uint32_t            fs_height_current;

    // geometry
    int32_t                         x;
    int32_t                         y;
    uint32_t                        width;
    uint32_t                        height;
    int32_t                         offset_x;   ///< relative to a browser window top left corner
    int32_t                         offset_y;   ///< relative to a browser window top left corner

    struct {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    } clip_rect;

    int                             argc;
    char                          **argn;
    char                          **argv;

    struct PP_Var                   instance_url;           ///< absolute instance url
    struct PP_Var                   document_url;
    struct PP_Var                   document_base_url;
    pthread_t                       main_thread;
    pthread_barrier_t               main_thread_barrier;

    // graphics2d and graphics3d
    PP_Resource                     graphics;
    struct PP_CompletionCallback    graphics_ccb;
    uint32_t                        graphics_in_progress;
    PP_Resource                     graphics_ccb_ml;

    // input method context
    PP_TextInput_Type_Dev           textinput_type;
    GtkIMContext                   *im_context;
    GtkIMContext                   *im_context_multi;
    GtkIMContext                   *im_context_simple;
};

PP_Bool
ppb_instance_bind_graphics(PP_Instance instance, PP_Resource device);

PP_Bool
ppb_instance_is_full_frame(PP_Instance instance);

struct PP_Var
ppb_instance_get_window_object(PP_Instance instance);

struct PP_Var
ppb_instance_get_owner_element_object(PP_Instance instance);

struct PP_Var
ppb_instance_execute_script(PP_Instance instance, struct PP_Var script, struct PP_Var *exception);
