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

#include <X11/Xlib.h>
#include <cairo.h>
#include <glib.h>
#include <pango/pango.h>

// GTK+ type mockups

#define GDK_WINDOW_XDISPLAY(win) \
    (gw_gdk_x11_display_get_xdisplay(gw_gdk_window_get_display(win)))

#define GDK_WINDOW_XID(win)         (gw_gdk_x11_drawable_get_xid(win))

#define GTK_TYPE_CHECK_MENU_ITEM    (gw_gtk_check_menu_item_get_type())
#define GTK_TYPE_FILE_CHOOSER       (gw_gtk_file_chooser_get_type())
#define GTK_TYPE_MENU               (gw_gtk_menu_get_type())
#define GTK_TYPE_MENU_ITEM          (gw_gtk_menu_item_get_type())
#define GTK_TYPE_MENU_SHELL         (gw_gtk_menu_shell_get_type())
#define GTK_TYPE_WIDGET             (gw_gtk_widget_get_type())

#define GTK_FILE_CHOOSER(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_FILE_CHOOSER, GtkFileChooser))

#define GTK_WIDGET(widget) \
    (G_TYPE_CHECK_INSTANCE_CAST((widget), GTK_TYPE_WIDGET, GtkWidget))

#define GTK_CHECK_MENU_ITEM(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_CHECK_MENU_ITEM, GtkCheckMenuItem))

#define GTK_MENU_SHELL(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_MENU_SHELL, GtkMenuShell))

#define GTK_MENU_ITEM(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_MENU_ITEM, GtkMenuItem))

#define GTK_MENU(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_MENU, GtkMenu))

#define _GDK_MAKE_ATOM(val)         ((GdkAtom)GUINT_TO_POINTER(val))

#define GDK_NONE                    _GDK_MAKE_ATOM(0)
#define GDK_SELECTION_PRIMARY       _GDK_MAKE_ATOM(1)
#define GDK_SELECTION_CLIPBOARD     _GDK_MAKE_ATOM(69)


typedef cairo_rectangle_int_t           GdkRectangle;
typedef struct _GdkAtom                *GdkAtom;
typedef struct _GdkDevice               GdkDevice;
typedef struct _GdkDisplay              GdkDisplay;
typedef struct _GdkDragContext          GdkDragContext;
typedef struct _GdkDrawable             GdkWindow;
typedef struct _GdkEventSequence        GdkEventSequence;
typedef struct _GdkKeymap               GdkKeymap;
typedef struct _GtkCheckMenuItem        GtkCheckMenuItem;
typedef struct _GtkClipboard            GtkClipboard;
typedef struct _GtkDialog               GtkDialog;
typedef struct _GtkFileChooser          GtkFileChooser;
typedef struct _GtkIMContext            GtkIMContext;
typedef struct _GtkMenu                 GtkMenu;
typedef struct _GtkMenuItem             GtkMenuItem;
typedef struct _GtkMenuShell            GtkMenuShell;
typedef struct _GtkSelectionData        GtkSelectionData;
typedef struct _GtkWidget               GtkWidget;
typedef struct _GtkWindow               GtkWindow;


typedef enum {
    GTK_FILE_CHOOSER_ACTION_OPEN,
    GTK_FILE_CHOOSER_ACTION_SAVE,
    GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
    GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
} GtkFileChooserAction;

typedef enum {
    GDK_VISIBILITY_UNOBSCURED,
    GDK_VISIBILITY_PARTIAL,
    GDK_VISIBILITY_FULLY_OBSCURED,
} GdkVisibilityState;

typedef enum {
    GDK_SCROLL_UP,
    GDK_SCROLL_DOWN,
    GDK_SCROLL_LEFT,
    GDK_SCROLL_RIGHT,
    GDK_SCROLL_SMOOTH,
} GdkScrollDirection;

typedef enum {
    GDK_CROSSING_NORMAL,
    GDK_CROSSING_GRAB,
    GDK_CROSSING_UNGRAB,
    GDK_CROSSING_GTK_GRAB,
    GDK_CROSSING_GTK_UNGRAB,
    GDK_CROSSING_STATE_CHANGED,
    GDK_CROSSING_TOUCH_BEGIN,
    GDK_CROSSING_TOUCH_END,
    GDK_CROSSING_DEVICE_SWITCH,
} GdkCrossingMode;

typedef enum {
    GDK_NOTIFY_ANCESTOR          = 0,
    GDK_NOTIFY_VIRTUAL           = 1,
    GDK_NOTIFY_INFERIOR          = 2,
    GDK_NOTIFY_NONLINEAR         = 3,
    GDK_NOTIFY_NONLINEAR_VIRTUAL = 4,
    GDK_NOTIFY_UNKNOWN           = 5,
} GdkNotifyType;

typedef enum {
    GDK_OWNER_CHANGE_NEW_OWNER,
    GDK_OWNER_CHANGE_DESTROY,
    GDK_OWNER_CHANGE_CLOSE,
} GdkOwnerChange;

typedef enum {
    GDK_SETTING_ACTION_NEW,
    GDK_SETTING_ACTION_CHANGED,
    GDK_SETTING_ACTION_DELETED,
} GdkSettingAction;

typedef enum {
    GDK_WINDOW_STATE_WITHDRAWN  = 1 << 0,
    GDK_WINDOW_STATE_ICONIFIED  = 1 << 1,
    GDK_WINDOW_STATE_MAXIMIZED  = 1 << 2,
    GDK_WINDOW_STATE_STICKY     = 1 << 3,
    GDK_WINDOW_STATE_FULLSCREEN = 1 << 4,
    GDK_WINDOW_STATE_ABOVE      = 1 << 5,
    GDK_WINDOW_STATE_BELOW      = 1 << 6,
    GDK_WINDOW_STATE_FOCUSED    = 1 << 7,
    GDK_WINDOW_STATE_TILED      = 1 << 8,
} GdkWindowState;

typedef enum {
    GDK_NOTHING           = -1,
    GDK_DELETE            = 0,
    GDK_DESTROY           = 1,
    GDK_EXPOSE            = 2,
    GDK_MOTION_NOTIFY     = 3,
    GDK_BUTTON_PRESS      = 4,
    GDK_2BUTTON_PRESS     = 5,
    GDK_3BUTTON_PRESS     = 6,
    GDK_BUTTON_RELEASE    = 7,
    GDK_KEY_PRESS         = 8,
    GDK_KEY_RELEASE       = 9,
    GDK_ENTER_NOTIFY      = 10,
    GDK_LEAVE_NOTIFY      = 11,
    GDK_FOCUS_CHANGE      = 12,
    GDK_CONFIGURE         = 13,
    GDK_MAP               = 14,
    GDK_UNMAP             = 15,
    GDK_PROPERTY_NOTIFY   = 16,
    GDK_SELECTION_CLEAR   = 17,
    GDK_SELECTION_REQUEST = 18,
    GDK_SELECTION_NOTIFY  = 19,
    GDK_PROXIMITY_IN      = 20,
    GDK_PROXIMITY_OUT     = 21,
    GDK_DRAG_ENTER        = 22,
    GDK_DRAG_LEAVE        = 23,
    GDK_DRAG_MOTION       = 24,
    GDK_DRAG_STATUS       = 25,
    GDK_DROP_START        = 26,
    GDK_DROP_FINISHED     = 27,
    GDK_CLIENT_EVENT      = 28,
    GDK_VISIBILITY_NOTIFY = 29,
    GDK_NO_EXPOSE         = 30,
    GDK_SCROLL            = 31,
    GDK_WINDOW_STATE      = 32,
    GDK_SETTING           = 33,
    GDK_OWNER_CHANGE      = 34,
    GDK_GRAB_BROKEN       = 35,
    GDK_DAMAGE            = 36,
    GDK_EVENT_LAST,
} GdkEventType;

typedef enum {
    GTK_RESPONSE_NONE         = -1,
    GTK_RESPONSE_REJECT       = -2,
    GTK_RESPONSE_ACCEPT       = -3,
    GTK_RESPONSE_DELETE_EVENT = -4,
    GTK_RESPONSE_OK           = -5,
    GTK_RESPONSE_CANCEL       = -6,
    GTK_RESPONSE_CLOSE        = -7,
    GTK_RESPONSE_YES          = -8,
    GTK_RESPONSE_NO           = -9,
    GTK_RESPONSE_APPLY        = -10,
    GTK_RESPONSE_HELP         = -11,
} GtkResponseType;

typedef struct _GdkEventAny {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
} GdkEventAny;

typedef struct _GdkEventExpose {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkRectangle    area;
    cairo_region_t *region;
    gint            count;
} GdkEventExpose;

typedef struct _GdkEventVisibility {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkVisibilityState state;
} GdkEventVisibility;

typedef struct _GdkEventMotion {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    guint32         time;
    gdouble         x, y;
    gdouble        *axes;
    guint           state;
    gint16          is_hint;
    GdkDevice      *device;
    gdouble         x_root, y_root;
} GdkEventMotion;

typedef struct _GdkEventButton {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    guint32         time;
    gdouble         x, y;
    gdouble        *axes;
    guint           state;
    guint           button;
    GdkDevice      *device;
    gdouble         x_root, y_root;
} GdkEventButton;

typedef struct _GdkEventTouch {
    GdkEventType        type;
    GdkWindow          *window;
    gint8               send_event;
    guint32             time;
    gdouble             x, y;
    gdouble            *axes;
    guint               state;
    GdkEventSequence   *sequence;
    gboolean            emulating_pointer;
    GdkDevice          *device;
    gdouble             x_root, y_root;
} GdkEventTouch;

typedef struct _GdkEventScroll {
    GdkEventType        type;
    GdkWindow          *window;
    gint8               send_event;
    guint32             time;
    gdouble             x, y;
    guint               state;
    GdkScrollDirection  direction;
    GdkDevice          *device;
    gdouble             x_root, y_root;
    gdouble             delta_x, delta_y;
    guint               is_stop:1;
} GdkEventScroll;

typedef struct _GdkEventKey {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    guint32         time;
    guint           state;
    guint           keyval;
    gint            length;
    gchar          *string;
    guint16         hardware_keycode;
    guint8          group;
    guint           is_modifier:1;
} GdkEventKey;

typedef struct _GdkEventCrossing {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkWindow      *subwindow;
    guint32         time;
    gdouble         x, y;
    gdouble         x_root, y_root;
    GdkCrossingMode mode;
    GdkNotifyType   detail;
    gboolean        focus;
    guint           state;
} GdkEventCrossing;

typedef struct _GdkEventFocus {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    gint16          in;
} GdkEventFocus;

typedef struct _GdkEventConfigure {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    gint            x, y;
    gint            width;
    gint            height;
} GdkEventConfigure;

typedef struct _GdkEventProperty {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkAtom         atom;
    guint32         time;
    guint           state;
} GdkEventProperty;

typedef struct _GdkEventSelection {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkAtom         selection;
    GdkAtom         target;
    GdkAtom         property;
    guint32         time;
    GdkWindow      *requestor;
} GdkEventSelection;

typedef struct _GdkEventOwnerChange {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkWindow      *owner;
    GdkOwnerChange  reason;
    GdkAtom         selection;
    guint32         time;
    guint32         selection_time;
} GdkEventOwnerChange;

typedef struct _GdkEventProximity {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    guint32         time;
    GdkDevice      *device;
} GdkEventProximity;

typedef struct _GdkEventSetting {
    GdkEventType        type;
    GdkWindow          *window;
    gint8               send_event;
    GdkSettingAction    action;
    char               *name;
} GdkEventSetting;

typedef struct _GdkEventWindowState {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkWindowState  changed_mask;
    GdkWindowState  new_window_state;
} GdkEventWindowState;

typedef struct _GdkEventGrabBroken {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    gboolean        keyboard;
    gboolean        implicit;
    GdkWindow      *grab_window;
} GdkEventGrabBroken;

typedef struct _GdkEventDND {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    GdkDragContext *context;
    guint32         time;
    gshort          x_root, y_root;
} GdkEventDND;

typedef struct _GdkEventTouchpadSwipe {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    gint8           phase;
    gint8           n_fingers;
    guint32         time;
    gdouble         x, y;
    gdouble         dx, dy;
    gdouble         x_root, y_root;
    guint           state;
} GdkEventTouchpadSwipe;

typedef struct _GdkEventTouchpadPinch {
    GdkEventType    type;
    GdkWindow      *window;
    gint8           send_event;
    gint8           phase;
    gint8           n_fingers;
    guint32         time;
    gdouble         x, y;
    gdouble         dx, dy;
    gdouble         angle_delta;
    gdouble         scale;
    gdouble         x_root, y_root;
    guint           state;
} GdkEventTouchpadPinch;

typedef union _GdkEvent {
    GdkEventType            type;
    GdkEventAny             any;
    GdkEventExpose          expose;
    GdkEventVisibility      visibility;
    GdkEventMotion          motion;
    GdkEventButton          button;
    GdkEventTouch           touch;
    GdkEventScroll          scroll;
    GdkEventKey             key;
    GdkEventCrossing        crossing;
    GdkEventFocus           focus_change;
    GdkEventConfigure       configure;
    GdkEventProperty        property;
    GdkEventSelection       selection;
    GdkEventOwnerChange     owner_change;
    GdkEventProximity       proximity;
    GdkEventDND             dnd;
    GdkEventWindowState     window_state;
    GdkEventSetting         setting;
    GdkEventGrabBroken      grab_broken;
    GdkEventTouchpadSwipe   touchpad_swipe;
    GdkEventTouchpadPinch   touchpad_pinch;
} GdkEvent;

typedef struct _GdkKeymapKey {
    guint   keycode;
    gint    group;
    gint    level;
} GdkKeymapKey;

typedef struct _GtkTargetEntry {
    gchar  *target;
    guint   flags;
    guint   info;
} GtkTargetEntry;


typedef void (*GtkClipboardGetFunc)(GtkClipboard *clipboard, GtkSelectionData *selection_data,
                                    guint info, gpointer user_data_or_owner);

typedef void (*GtkClipboardClearFunc)(GtkClipboard *clipboard, gpointer user_data_or_owner);

typedef void (*GtkMenuPositionFunc)(GtkMenu *menu, gint *x, gint *y, gboolean *push_in,
                                    gpointer user_data);

// list of pointers to GTK+ functions

extern
GdkAtom
(*gw_gdk_atom_intern)(const gchar *atom_name, gboolean only_if_exists);

extern
GdkAtom
(*gw_gdk_atom_intern_static_string)(const gchar *atom_name);

extern
gchar *
(*gw_gdk_atom_name)(GdkAtom atom);

extern
GdkDisplay *
(*gw_gdk_display_get_default)(void);

extern
void
(*gw_gdk_event_free)(GdkEvent *event);

extern
GdkEvent *
(*gw_gdk_event_new)(GdkEventType type);

extern
gboolean
(*gw_gdk_keymap_get_entries_for_keycode)(GdkKeymap *keymap, guint hardware_keycode,
                                         GdkKeymapKey **keys, guint **keyvals, gint *n_entries);

extern
GdkKeymap *
(*gw_gdk_keymap_get_for_display)(GdkDisplay *display);

extern
GdkDisplay *
(*gw_gdk_window_get_display)(GdkWindow *window);

extern
void
(*gw_gdk_window_set_user_data)(GdkWindow *window, gpointer user_data);

extern
Display *
(*gw_gdk_x11_display_get_xdisplay)(GdkDisplay *display);

extern
Window
(*gw_gdk_x11_drawable_get_xid)(GdkWindow *window);

extern
GdkDisplay *
(*gw_gdk_x11_lookup_xdisplay)(Display *xdisplay);

extern
GdkWindow *
(*gw_gdk_x11_window_foreign_new_for_display)(GdkDisplay *display, Window window);

extern
GdkWindow *
(*gw_gdk_x11_window_lookup_for_display)(GdkDisplay *display, Window window);

extern
GType
(*gw_gtk_check_menu_item_get_type)(void);

extern
GtkWidget *
(*gw_gtk_check_menu_item_new_with_label)(const gchar *label);

extern
void
(*gw_gtk_check_menu_item_set_active)(GtkCheckMenuItem *check_menu_item, gboolean is_active);

extern
void
(*gw_gtk_clipboard_clear)(GtkClipboard *clipboard);

extern
GtkClipboard *
(*gw_gtk_clipboard_get)(GdkAtom selection);

extern
void
(*gw_gtk_clipboard_set_can_store)(GtkClipboard *clipboard, const GtkTargetEntry *targets,
                                  gint n_targets);
extern
gboolean
(*gw_gtk_clipboard_set_with_data)(GtkClipboard *clipboard, const GtkTargetEntry *targets,
                                  guint n_targets, GtkClipboardGetFunc get_func,
                                  GtkClipboardClearFunc clear_func, gpointer user_data);
extern
GtkSelectionData *
(*gw_gtk_clipboard_wait_for_contents)(GtkClipboard *clipboard, GdkAtom target);

extern
gboolean
(*gw_gtk_clipboard_wait_is_target_available)(GtkClipboard *clipboard, GdkAtom target);

extern
GtkWidget *
(*gw_gtk_file_chooser_dialog_new)(const gchar *title, GtkWindow *parent,
                                  GtkFileChooserAction action, const gchar *first_button_text, ...);

extern
GSList *
(*gw_gtk_file_chooser_get_filenames)(GtkFileChooser *chooser);

extern
GType
(*gw_gtk_file_chooser_get_type)(void);

extern
void
(*gw_gtk_file_chooser_set_select_multiple)(GtkFileChooser *chooser, gboolean select_multiple);

extern
guint32
(*gw_gtk_get_current_event_time)(void);

extern
gboolean
(*gw_gtk_im_context_filter_keypress)(GtkIMContext *context, GdkEventKey *event);

extern
void
(*gw_gtk_im_context_focus_in)(GtkIMContext *context);

extern
void
(*gw_gtk_im_context_focus_out)(GtkIMContext *context);

extern
void
(*gw_gtk_im_context_get_preedit_string)(GtkIMContext *context, gchar **str, PangoAttrList **attrs,
                                        gint *cursor_pos);

extern
void
(*gw_gtk_im_context_reset)(GtkIMContext *context);

extern
void
(*gw_gtk_im_context_set_client_window)(GtkIMContext *context, GdkWindow *window);

extern
void
(*gw_gtk_im_context_set_cursor_location)(GtkIMContext *context, const GdkRectangle *area);

extern
void
(*gw_gtk_im_context_set_surrounding)(GtkIMContext *context, const gchar *text, gint len,
                                     gint cursor_index);

extern
GtkIMContext *
(*gw_gtk_im_context_simple_new)(void);

extern
GtkIMContext *
(*gw_gtk_im_multicontext_new)(void);

extern
GtkWidget *
(*gw_gtk_label_new)(const gchar *str);

extern
GType
(*gw_gtk_menu_get_type)(void);

extern
GType
(*gw_gtk_menu_item_get_type)(void);

extern
GtkWidget *
(*gw_gtk_menu_item_new_with_label)(const gchar *label);

extern
void
(*gw_gtk_menu_item_set_submenu)(GtkMenuItem *menu_item, GtkWidget *submenu);

extern
GtkWidget *
(*gw_gtk_menu_new)(void);

extern
void
(*gw_gtk_menu_popup)(GtkMenu *menu, GtkWidget *parent_menu_shell, GtkWidget *parent_menu_item,
                     GtkMenuPositionFunc func, gpointer data, guint button, guint32 activate_time);

extern
void
(*gw_gtk_menu_shell_append)(GtkMenuShell *menu_shell, GtkWidget *child);

extern
GType
(*gw_gtk_menu_shell_get_type)(void);

extern
void
(*gw_gtk_selection_data_free)(GtkSelectionData *data);

extern
const guchar *
(*gw_gtk_selection_data_get_data)(const GtkSelectionData *selection_data);

extern
gint
(*gw_gtk_selection_data_get_length)(const GtkSelectionData *selection_data);

extern
void
(*gw_gtk_selection_data_set)(GtkSelectionData *selection_data, GdkAtom type, gint format,
                             const guchar *data, gint length);

extern
GtkWidget *
(*gw_gtk_separator_menu_item_new)(void);

extern
void
(*gw_gtk_widget_destroy)(GtkWidget *widget);

extern
GType
(*gw_gtk_widget_get_type)(void);

extern
GdkWindow *
(*gw_gtk_widget_get_window)(GtkWidget *widget);

extern
void
(*gw_gtk_widget_realize)(GtkWidget *widget);

extern
void
(*gw_gtk_widget_set_realized)(GtkWidget *widget, gboolean realized);

extern
void
(*gw_gtk_widget_set_sensitive)(GtkWidget *widget, gboolean sensitive);

extern
void
(*gw_gtk_widget_show)(GtkWidget *widget);

// end of function pointers list

void
gtk_wrapper_initialize(void);

int
gw_major_version(void);

int
gw_minor_version(void);

int
gw_gtk_available(void);
