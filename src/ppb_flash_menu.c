/*
 * Copyright © 2013-2014  Rinat Ibragimov
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

#include "ppb_flash_menu.h"
#include <stdlib.h>
#include "trace.h"
#include "pp_resource.h"
#include <ppapi/c/pp_errors.h>
#include <gtk/gtk.h>


static int32_t                     *popup_menu_result = NULL;
static struct PP_CompletionCallback popup_menu_ccb = { 0 };
static int                          popup_menu_sentinel = 0;
static int                          popup_menu_canceled = 0;


// called when used selects menu item
static
void
menu_item_activated(GtkMenuItem *mi, gpointer user_data)
{
    if (popup_menu_result)
        *popup_menu_result = (size_t)user_data;

    // set the flag indicating user selected something, not just aborted
    popup_menu_canceled = 0;
}

// called when menu is closed
static
void
menu_closed(GtkWidget *object, gboolean user_data)
{
    int32_t code = popup_menu_canceled ? PP_ERROR_USERCANCEL : PP_OK;

    if (popup_menu_ccb.func)
        popup_menu_ccb.func(popup_menu_ccb.user_data, code);

    popup_menu_sentinel = 0;
    popup_menu_result = NULL;
}

static
GtkWidget *
convert_menu(const struct PP_Flash_Menu *pp_menu)
{
    GtkWidget *menu = gtk_menu_new();

    for (uintptr_t k = 0; k < pp_menu->count; k ++) {
        const struct PP_Flash_MenuItem pp_mi = pp_menu->items[k];
        GtkWidget *mi;

        switch (pp_mi.type) {
        case PP_FLASH_MENUITEM_TYPE_NORMAL:
            mi = gtk_menu_item_new_with_label(pp_mi.name);
            break;
        case PP_FLASH_MENUITEM_TYPE_CHECKBOX:
            mi = gtk_check_menu_item_new_with_label(pp_mi.name);
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), pp_mi.checked != PP_FALSE);
            break;
        case PP_FLASH_MENUITEM_TYPE_SEPARATOR:
            mi = gtk_separator_menu_item_new();
            break;
        case PP_FLASH_MENUITEM_TYPE_SUBMENU:
            mi = gtk_menu_item_new_with_label(pp_mi.name);
            break;
        }

        gtk_widget_set_sensitive(mi, pp_mi.enabled != PP_FALSE);

        gtk_widget_show(mi);
        gtk_menu_append(GTK_MENU(menu), mi);

        if (pp_mi.type == PP_FLASH_MENUITEM_TYPE_SUBMENU)
            gtk_menu_item_set_submenu(GTK_MENU_ITEM(mi), convert_menu(pp_mi.submenu));

        // each menu item have specific id associated
        g_signal_connect(G_OBJECT(mi), "activate", G_CALLBACK(menu_item_activated),
                         (void*)(size_t)pp_mi.id);
    }

    return menu;
}

PP_Resource
ppb_flash_menu_create(PP_Instance instance_id, const struct PP_Flash_Menu *menu_data)
{
    PP_Resource flash_menu = pp_resource_allocate(PP_RESOURCE_FLASH_MENU, instance_id);
    struct pp_flash_menu_s *fm = pp_resource_acquire(flash_menu, PP_RESOURCE_FLASH_MENU);

    // recursively construct menu
    fm->menu = convert_menu(menu_data);

    // we need notification on menu destroy
    g_signal_connect(G_OBJECT(fm->menu), "destroy", G_CALLBACK(menu_closed), NULL);

    pp_resource_release(flash_menu);
    return flash_menu;
}

void
ppb_flash_menu_destroy(void *p)
{
    struct pp_flash_menu_s *fm = p;
    g_object_ref_sink(fm->menu);
    g_object_unref(fm->menu);
}

PP_Bool
ppb_flash_menu_is_flash_menu(PP_Resource resource_id)
{
    return pp_resource_get_type(resource_id) == PP_RESOURCE_FLASH_MENU;
}

int32_t
ppb_flash_menu_show(PP_Resource menu_id, const struct PP_Point *location, int32_t *selected_id,
                    struct PP_CompletionCallback callback)
{
    struct pp_flash_menu_s *fm = pp_resource_acquire(menu_id, PP_RESOURCE_FLASH_MENU);
    if (!fm)
        return PP_ERROR_BADRESOURCE;

    if (popup_menu_sentinel)
        trace_error("%s, two context menus at the same time\n", __func__);

    (void)location; // TODO: handle location

    popup_menu_sentinel = 1;
    popup_menu_canceled = 1;
    popup_menu_ccb = callback;
    popup_menu_result = selected_id;

    gtk_menu_popup(GTK_MENU(fm->menu), NULL, NULL, NULL, NULL, 3, gtk_get_current_event_time());

    pp_resource_release(menu_id);
    return PP_OK_COMPLETIONPENDING;
}


// trace wrappers
static
PP_Resource
trace_ppb_flash_menu_create(PP_Instance instance_id, const struct PP_Flash_Menu *menu_data)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_flash_menu_create(instance_id, menu_data);
}

static
PP_Bool
trace_ppb_flash_menu_is_flash_menu(PP_Resource resource_id)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_flash_menu_is_flash_menu(resource_id);
}

static
int32_t
trace_ppb_flash_menu_show(PP_Resource menu_id, const struct PP_Point *location,
                          int32_t *selected_id, struct PP_CompletionCallback callback)
{
    trace_info("[PPB] {full} %s\n", __func__+6);
    return ppb_flash_menu_show(menu_id, location, selected_id, callback);
}

const struct PPB_Flash_Menu_0_2 ppb_flash_menu_interface_0_2 = {
    .Create =       trace_ppb_flash_menu_create,
    .IsFlashMenu =  trace_ppb_flash_menu_is_flash_menu,
    .Show =         trace_ppb_flash_menu_show,
};
