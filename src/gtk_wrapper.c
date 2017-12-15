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

#define _GNU_SOURCE

#include "gtk_wrapper.h"
#include "trace_core.h"
#include <dlfcn.h>
#include <link.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static struct {
    int     major_version;
    int     minor_version;
    int     available_flag;
    void   *dl_handler;
} gw = {};

GdkAtom
(*gw_gdk_atom_intern)(const gchar *atom_name, gboolean only_if_exists);

GdkAtom
(*gw_gdk_atom_intern_static_string)(const gchar *atom_name);

gchar *
(*gw_gdk_atom_name)(GdkAtom atom);

GdkDisplay *
(*gw_gdk_display_get_default)(void);

void
(*gw_gdk_event_free)(GdkEvent *event);

GdkEvent *
(*gw_gdk_event_new)(GdkEventType type);

gboolean
(*gw_gdk_keymap_get_entries_for_keycode)(GdkKeymap *keymap, guint hardware_keycode,
                                         GdkKeymapKey **keys, guint **keyvals, gint *n_entries);

GdkKeymap *
(*gw_gdk_keymap_get_for_display)(GdkDisplay *display);

GdkDisplay *
(*gw_gdk_window_get_display)(GdkWindow *window);

void
(*gw_gdk_window_set_user_data)(GdkWindow *window, gpointer user_data);

Display *
(*gw_gdk_x11_display_get_xdisplay)(GdkDisplay *display);

Window
(*gw_gdk_x11_drawable_get_xid)(GdkWindow *window);

GdkDisplay *
(*gw_gdk_x11_lookup_xdisplay)(Display *xdisplay);

GdkWindow *
(*gw_gdk_x11_window_foreign_new_for_display)(GdkDisplay *display, Window window);

GdkWindow *
(*gw_gdk_x11_window_lookup_for_display)(GdkDisplay *display, Window window);

GType
(*gw_gtk_check_menu_item_get_type)(void);

GtkWidget *
(*gw_gtk_check_menu_item_new_with_label)(const gchar *label);

void
(*gw_gtk_check_menu_item_set_active)(GtkCheckMenuItem *check_menu_item, gboolean is_active);

void
(*gw_gtk_clipboard_clear)(GtkClipboard *clipboard);

GtkClipboard *
(*gw_gtk_clipboard_get)(GdkAtom selection);

void
(*gw_gtk_clipboard_set_can_store)(GtkClipboard *clipboard, const GtkTargetEntry *targets,
                                  gint n_targets);
gboolean
(*gw_gtk_clipboard_set_with_data)(GtkClipboard *clipboard, const GtkTargetEntry *targets,
                                  guint n_targets, GtkClipboardGetFunc get_func,
                                  GtkClipboardClearFunc clear_func, gpointer user_data);
GtkSelectionData *
(*gw_gtk_clipboard_wait_for_contents)(GtkClipboard *clipboard, GdkAtom target);

gboolean
(*gw_gtk_clipboard_wait_is_target_available)(GtkClipboard *clipboard, GdkAtom target);

GtkWidget *
(*gw_gtk_file_chooser_dialog_new)(const gchar *title, GtkWindow *parent,
                                  GtkFileChooserAction action, const gchar *first_button_text, ...);

GSList *
(*gw_gtk_file_chooser_get_filenames)(GtkFileChooser *chooser);

GType
(*gw_gtk_file_chooser_get_type)(void);

void
(*gw_gtk_file_chooser_set_select_multiple)(GtkFileChooser *chooser, gboolean select_multiple);

guint32
(*gw_gtk_get_current_event_time)(void);

gboolean
(*gw_gtk_im_context_filter_keypress)(GtkIMContext *context, GdkEventKey *event);

void
(*gw_gtk_im_context_focus_in)(GtkIMContext *context);

void
(*gw_gtk_im_context_focus_out)(GtkIMContext *context);

void
(*gw_gtk_im_context_get_preedit_string)(GtkIMContext *context, gchar **str, PangoAttrList **attrs,
                                        gint *cursor_pos);

void
(*gw_gtk_im_context_reset)(GtkIMContext *context);

void
(*gw_gtk_im_context_set_client_window)(GtkIMContext *context, GdkWindow *window);

void
(*gw_gtk_im_context_set_cursor_location)(GtkIMContext *context, const GdkRectangle *area);

void
(*gw_gtk_im_context_set_surrounding)(GtkIMContext *context, const gchar *text, gint len,
                                     gint cursor_index);

GtkIMContext *
(*gw_gtk_im_context_simple_new)(void);

GtkIMContext *
(*gw_gtk_im_multicontext_new)(void);

GtkWidget *
(*gw_gtk_label_new)(const gchar *str);

GType
(*gw_gtk_menu_get_type)(void);

GType
(*gw_gtk_menu_item_get_type)(void);

GtkWidget *
(*gw_gtk_menu_item_new_with_label)(const gchar *label);

void
(*gw_gtk_menu_item_set_submenu)(GtkMenuItem *menu_item, GtkWidget *submenu);

GtkWidget *
(*gw_gtk_menu_new)(void);

void
(*gw_gtk_menu_popup)(GtkMenu *menu, GtkWidget *parent_menu_shell, GtkWidget *parent_menu_item,
                     GtkMenuPositionFunc func, gpointer data, guint button, guint32 activate_time);

void
(*gw_gtk_menu_shell_append)(GtkMenuShell *menu_shell, GtkWidget *child);

GType
(*gw_gtk_menu_shell_get_type)(void);

void
(*gw_gtk_selection_data_free)(GtkSelectionData *data);

const guchar *
(*gw_gtk_selection_data_get_data)(const GtkSelectionData *selection_data);

gint
(*gw_gtk_selection_data_get_length)(const GtkSelectionData *selection_data);

void
(*gw_gtk_selection_data_set)(GtkSelectionData *selection_data, GdkAtom type, gint format,
                             const guchar *data, gint length);

GtkWidget *
(*gw_gtk_separator_menu_item_new)(void);

void
(*gw_gtk_widget_destroy)(GtkWidget *widget);

GType
(*gw_gtk_widget_get_type)(void);

GdkWindow *
(*gw_gtk_widget_get_window)(GtkWidget *widget);

void
(*gw_gtk_widget_realize)(GtkWidget *widget);

void
(*gw_gtk_widget_set_realized)(GtkWidget *widget, gboolean realized);

void
(*gw_gtk_widget_set_sensitive)(GtkWidget *widget, gboolean sensitive);

void
(*gw_gtk_widget_show)(GtkWidget *widget);

static
int
find_gtk_cb(struct dl_phdr_info *info, size_t size, void *data)
{
    if (!info->dlpi_name)
        return 0;

    // try GTK+ 2
    if (strstr(info->dlpi_name, "/libgtk-x11-2.0")) {
        gw.dl_handler = dlopen(info->dlpi_name, RTLD_NOW);
        gw.major_version = 2;
    }

    // try GTK+ 3
    if (strstr(info->dlpi_name, "/libgtk-3.so")) {
        gw.dl_handler = dlopen(info->dlpi_name, RTLD_NOW);
        gw.major_version = 3;
    }

    // don't care if both versions are loaded.
    // In that case we're in a buggy state anyway.

    return 0;
}

void
gtk_wrapper_initialize(void)
{
    static int initialize_was_called = 0;

    if (initialize_was_called)
        return;

    initialize_was_called = 1;

    dl_iterate_phdr(find_gtk_cb, NULL);

    if (gw.major_version == 0)
        return;

    gw.available_flag = 1;

    uint32_t *minor_version;
    uint32_t (*gw_gtk_get_minor_version)(void);

    switch (gw.major_version) {
    case 2:
        minor_version = dlsym(RTLD_DEFAULT, "gtk_minor_version");
        if (minor_version)
            memcpy(&gw.minor_version, minor_version, sizeof(uint32_t));
        break;

    case 3:
        gw_gtk_get_minor_version = dlsym(RTLD_DEFAULT, "gtk_get_minor_version");
        if (gw_gtk_get_minor_version)
            gw.minor_version = gw_gtk_get_minor_version();
        break;

    default:
        // do nothing
        break;
    }

#define GET_SYMBOL2(symname, libname)                   \
do {                                                    \
    gw_ ## symname = dlsym(gw.dl_handler, #libname);    \
    if (gw_ ## symname == NULL)                         \
        trace_error("can't resolve " #libname "\n");    \
} while (0)

#define GET_SYMBOL(symname)                             \
do {                                                    \
    GET_SYMBOL2(symname, symname);                      \
} while (0)

    GET_SYMBOL(gtk_check_menu_item_get_type);
    GET_SYMBOL(gtk_check_menu_item_new_with_label);
    GET_SYMBOL(gtk_check_menu_item_set_active);
    GET_SYMBOL(gtk_clipboard_clear);
    GET_SYMBOL(gtk_clipboard_get);
    GET_SYMBOL(gtk_clipboard_set_can_store);
    GET_SYMBOL(gtk_clipboard_set_with_data);
    GET_SYMBOL(gtk_clipboard_wait_for_contents);
    GET_SYMBOL(gtk_clipboard_wait_is_target_available);
    GET_SYMBOL(gtk_file_chooser_dialog_new);
    GET_SYMBOL(gtk_file_chooser_get_filenames);
    GET_SYMBOL(gtk_file_chooser_get_type);
    GET_SYMBOL(gtk_file_chooser_set_select_multiple);
    GET_SYMBOL(gtk_get_current_event_time);
    GET_SYMBOL(gtk_im_context_filter_keypress);
    GET_SYMBOL(gtk_im_context_focus_in);
    GET_SYMBOL(gtk_im_context_focus_out);
    GET_SYMBOL(gtk_im_context_get_preedit_string);
    GET_SYMBOL(gtk_im_context_reset);
    GET_SYMBOL(gtk_im_context_set_client_window);
    GET_SYMBOL(gtk_im_context_set_cursor_location);
    GET_SYMBOL(gtk_im_context_set_surrounding);
    GET_SYMBOL(gtk_im_context_simple_new);
    GET_SYMBOL(gtk_im_multicontext_new);
    GET_SYMBOL(gtk_label_new);
    GET_SYMBOL(gtk_menu_get_type);
    GET_SYMBOL(gtk_menu_item_get_type);
    GET_SYMBOL(gtk_menu_item_new_with_label);
    GET_SYMBOL(gtk_menu_item_set_submenu);
    GET_SYMBOL(gtk_menu_new);
    GET_SYMBOL(gtk_menu_popup);
    GET_SYMBOL(gtk_menu_shell_append);
    GET_SYMBOL(gtk_menu_shell_get_type);
    GET_SYMBOL(gtk_selection_data_free);
    GET_SYMBOL(gtk_selection_data_get_data);
    GET_SYMBOL(gtk_selection_data_get_length);
    GET_SYMBOL(gtk_selection_data_set);
    GET_SYMBOL(gtk_separator_menu_item_new);
    GET_SYMBOL(gtk_widget_destroy);
    GET_SYMBOL(gtk_widget_get_type);
    GET_SYMBOL(gtk_widget_get_window);
    GET_SYMBOL(gtk_widget_realize);
    GET_SYMBOL(gtk_widget_set_realized);
    GET_SYMBOL(gtk_widget_set_sensitive);
    GET_SYMBOL(gtk_widget_show);

    GET_SYMBOL(gdk_atom_intern);
    GET_SYMBOL(gdk_atom_intern_static_string);
    GET_SYMBOL(gdk_atom_name);
    GET_SYMBOL(gdk_display_get_default);
    GET_SYMBOL(gdk_event_free);
    GET_SYMBOL(gdk_event_new);
    GET_SYMBOL(gdk_keymap_get_entries_for_keycode);
    GET_SYMBOL(gdk_keymap_get_for_display);
    GET_SYMBOL(gdk_window_get_display);
    GET_SYMBOL(gdk_window_set_user_data);
    GET_SYMBOL(gdk_x11_display_get_xdisplay);
    GET_SYMBOL(gdk_x11_lookup_xdisplay);

    if (gw.major_version == 2) {
        GET_SYMBOL(gdk_x11_drawable_get_xid);

    } else {
        GET_SYMBOL2(gdk_x11_drawable_get_xid, gdk_x11_window_get_xid);
    }

    if (gw.major_version == 2 && gw.minor_version < 24) {
        // Those functions didn't exists before 2.24.
        // They appear in 2.24, while previous names become deprecated.
        GET_SYMBOL2(gdk_x11_window_foreign_new_for_display, gdk_window_foreign_new_for_display);
        GET_SYMBOL2(gdk_x11_window_lookup_for_display, gdk_window_lookup_for_display);

    } else {
        // in 2.24, or in 3.*
        GET_SYMBOL(gdk_x11_window_foreign_new_for_display);
        GET_SYMBOL(gdk_x11_window_lookup_for_display);
    }
}

int
gw_major_version(void)
{
    return gw.major_version;
}

int
gw_minor_version(void)
{
    return gw.minor_version;
}

int
gw_gtk_available(void)
{
    return gw.available_flag;
}
