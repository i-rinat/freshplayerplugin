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

#include "ppb_flash_clipboard.h"
#include <stdlib.h>
#include "trace.h"
#include "tables.h"
#include "pp_resource.h"
#include "reverse_constant.h"
#include "ppb_message_loop.h"
#include "ppb_var.h"
#include "ppb_core.h"
#include <ppapi/c/pp_errors.h>


uint32_t
ppb_flash_clipboard_register_custom_format(PP_Instance instance_id, const char *format_name)
{
    return 0;
}

static
int
clipboard_type_and_format_are_supported(PP_Flash_Clipboard_Type clipboard_type, uint32_t format,
                                        const char *func_name)
{
    if (clipboard_type != PP_FLASH_CLIPBOARD_TYPE_STANDARD
        && clipboard_type != PP_FLASH_CLIPBOARD_TYPE_SELECTION)
    {
        trace_error("%s, bad clipboard_type (= %d)\n", func_name, clipboard_type);
        return 0;
    }

    if (format != PP_FLASH_CLIPBOARD_FORMAT_PLAINTEXT
        && format != PP_FLASH_CLIPBOARD_FORMAT_HTML
        && format != PP_FLASH_CLIPBOARD_FORMAT_RTF)
    {
        trace_error("%s, unknown format (= %d)\n", func_name, format);
        return 0;
    }

    return 1;
}

static
GtkClipboard *
get_clipboard_of_type(PP_Flash_Clipboard_Type clipboard_type)
{
    switch (clipboard_type) {
    case PP_FLASH_CLIPBOARD_TYPE_STANDARD:
        return gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    case PP_FLASH_CLIPBOARD_TYPE_SELECTION:
        return gtk_clipboard_get(GDK_SELECTION_PRIMARY);
    default:
        return NULL;
    }
}

static
GdkAtom
get_clipboard_target_atom(uint32_t format)
{
    switch (format) {
    case PP_FLASH_CLIPBOARD_FORMAT_PLAINTEXT:
        return GDK_TARGET_STRING;
    case PP_FLASH_CLIPBOARD_FORMAT_HTML:
        return gdk_atom_intern("text/html", FALSE);
    case PP_FLASH_CLIPBOARD_FORMAT_RTF:
        return gdk_atom_intern("text/rtf", FALSE);
    default:
        return GDK_NONE;
    }
}

struct clipboard_is_format_available_param_s {
    PP_Flash_Clipboard_Type clipboard_type;
    uint32_t                format;
    PP_Bool                 result;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
_clipboard_is_format_available_ptac(void *user_data)
{
    struct clipboard_is_format_available_param_s *p = user_data;

    GtkClipboard *clipboard = get_clipboard_of_type(p->clipboard_type);
    if (!clipboard)
        goto quit;

    GdkAtom target = get_clipboard_target_atom(p->format);
    if (target == GDK_NONE)
        goto quit;

    p->result = gtk_clipboard_wait_is_target_available(clipboard, target);

quit:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
_clipboard_is_format_available_comt(void *user_data, int32_t result)
{
    ppb_core_call_on_browser_thread(_clipboard_is_format_available_ptac, user_data);
}

PP_Bool
ppb_flash_clipboard_is_format_available(PP_Instance instance_id,
                                        PP_Flash_Clipboard_Type clipboard_type, uint32_t format)
{
    if (!clipboard_type_and_format_are_supported(clipboard_type, format, __func__))
        return PP_FALSE;

    struct clipboard_is_format_available_param_s p;
    p.clipboard_type =  clipboard_type;
    p.format =          format;
    p.result =          PP_FALSE;
    p.m_loop =          ppb_message_loop_get_current();
    p.depth =           ppb_message_loop_get_depth(p.m_loop) + 1;

    ppb_message_loop_post_work(p.m_loop, PP_MakeCCB(_clipboard_is_format_available_comt, &p), 0);
    ppb_message_loop_run_nested(p.m_loop);

    return p.result;
}

struct clipboard_read_data_param_s {
    PP_Flash_Clipboard_Type clipboard_type;
    uint32_t                format;
    struct PP_Var           result;
    PP_Resource             m_loop;
    int                     depth;
};

static
void
_clipboard_read_data_ptac(void *user_data)
{
    struct clipboard_read_data_param_s *p = user_data;

    GtkClipboard *clipboard = get_clipboard_of_type(p->clipboard_type);
    if (!clipboard)
        goto quit;

    GdkAtom target = get_clipboard_target_atom(p->format);
    if (target == GDK_NONE)
        goto quit;

    GtkSelectionData *sd = gtk_clipboard_wait_for_contents(clipboard, target);
    if (sd) {
        p->result = ppb_var_var_from_utf8((char *)sd->data, sd->length);
        gtk_selection_data_free(sd);
    }

quit:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
_clipboard_read_data_comt(void *user_data, int32_t result)
{
    ppb_core_call_on_browser_thread(_clipboard_read_data_ptac, user_data);
}

struct PP_Var
ppb_flash_clipboard_read_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                              uint32_t format)
{
    if (!clipboard_type_and_format_are_supported(clipboard_type, format, __func__))
        return PP_MakeUndefined();

    struct clipboard_read_data_param_s p;
    p.clipboard_type =  clipboard_type;
    p.format =          format;
    p.result =          PP_MakeUndefined();
    p.m_loop =          ppb_message_loop_get_current();
    p.depth =           ppb_message_loop_get_depth(p.m_loop) + 1;

    ppb_message_loop_post_work(p.m_loop, PP_MakeCCB(_clipboard_read_data_comt, &p), 0);
    ppb_message_loop_run_nested(p.m_loop);

    return p.result;
}

struct clipboard_write_data_param_s {
    PP_Flash_Clipboard_Type     clipboard_type;
    uint32_t                    data_item_count;
    const uint32_t             *formats;
    const struct PP_Var        *data_items;
    int32_t                     result;
    PP_Resource                 m_loop;
    int                         depth;
};

struct selection_entry_s {
    GdkAtom         type;
    struct PP_Var   var;
};

static
void
_get_func(GtkClipboard *clipboard, GtkSelectionData *selection_data, guint info, gpointer user_data)
{
    GArray *items = user_data;
    struct selection_entry_s *item = &g_array_index(items, struct selection_entry_s, info);
    uint32_t len = 0;
    const char *data = ppb_var_var_to_utf8(item->var, &len);

    gtk_selection_data_set(selection_data, item->type, 8, (const guchar *)data, len);
}

static
void
_clear_func(GtkClipboard *clipboard, gpointer user_data)
{
    GArray *items = user_data;

    for (uint32_t k = 0; k < items->len; k ++) {
        struct selection_entry_s *item = &g_array_index(items, struct selection_entry_s, k);
        ppb_var_release(item->var);
    }

    g_array_unref(items);
}

static
void
_clipboard_write_data_ptac(void *user_data)
{
    struct clipboard_write_data_param_s *p = user_data;
    struct selection_entry_s item;

    GtkClipboard *clipboard = get_clipboard_of_type(p->clipboard_type);
    if (!clipboard)
        goto quit;

    GArray *items = g_array_new(FALSE /* no zero teminator */, TRUE /* zero items */, sizeof(item));

    for (uint32_t k = 0; k < p->data_item_count; k ++) {
        const char *data;
        uint32_t len;

        item.var = p->data_items[k];
        data = ppb_var_var_to_utf8(p->data_items[k], &len);
        if (!data)
            continue;

        switch (p->formats[k]) {
        case PP_FLASH_CLIPBOARD_FORMAT_PLAINTEXT:
            item.type = gdk_atom_intern("text/html", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);

            item.type = gdk_atom_intern("TEXT", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);

            item.type = gdk_atom_intern("STRING", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);

            item.type = gdk_atom_intern("UTF8_STRING", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);

            item.type = gdk_atom_intern("COMPOUND_TEXT", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);
            break;
        case PP_FLASH_CLIPBOARD_FORMAT_HTML:
            item.type = gdk_atom_intern("text/html", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);
            break;
        case PP_FLASH_CLIPBOARD_FORMAT_RTF:
            item.type = gdk_atom_intern("text/rtf", FALSE);
            ppb_var_add_ref(item.var);
            g_array_append_val(items, item);
            break;
        default:
            break;
        }
    }

    GtkTargetEntry *targets = g_new0(GtkTargetEntry, items->len);
    for (uint32_t k = 0; k < items->len; k ++) {
        targets[k].target = gdk_atom_name(g_array_index(items, struct selection_entry_s, k).type);
        targets[k].info = k;
    }

    if (gtk_clipboard_set_with_data(clipboard, targets, items->len, _get_func, _clear_func, items))
    {
        gtk_clipboard_set_can_store(clipboard, targets, items->len);
    } else {
        p->result = PP_ERROR_FAILED;
    }

    for (uint32_t k = 0; k < items->len; k ++)
        g_free(targets[k].target);
    g_free(targets);

quit:
    ppb_message_loop_post_quit_depth(p->m_loop, PP_FALSE, p->depth);
}

static
void
_clipboard_write_data_comt(void *user_data, int32_t result)
{
    ppb_core_call_on_browser_thread(_clipboard_write_data_ptac, user_data);
}

int32_t
ppb_flash_clipboard_write_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                               uint32_t data_item_count, const uint32_t formats[],
                               const struct PP_Var data_items[])
{
    for (uint32_t k = 0; k < data_item_count; k ++) {
        if (!clipboard_type_and_format_are_supported(clipboard_type, formats[k], __func__))
            return PP_ERROR_FAILED;
    }

    struct clipboard_write_data_param_s p;
    p.clipboard_type =  clipboard_type;
    p.data_item_count = data_item_count;
    p.formats =         formats;
    p.data_items =      data_items;
    p.result =          PP_OK;
    p.m_loop =          ppb_message_loop_get_current();
    p.depth =           ppb_message_loop_get_depth(p.m_loop) + 1;

    ppb_message_loop_post_work(p.m_loop, PP_MakeCCB(_clipboard_write_data_comt, &p), 0);
    ppb_message_loop_run_nested(p.m_loop);

    return p.result;
}

PP_Bool
ppb_flash_clipboard_get_sequence_number(PP_Instance instance_id,
                                        PP_Flash_Clipboard_Type clipboard_type,
                                        uint64_t *sequence_number)
{
    return PP_FALSE;
}


// trace wrappers
TRACE_WRAPPER
uint32_t
trace_ppb_flash_clipboard_register_custom_format(PP_Instance instance_id, const char *format_name)
{
    trace_info("[PPB] {zilch} %s instance_id=%d, format_name=%s\n", __func__+6, instance_id,
               format_name);
    return ppb_flash_clipboard_register_custom_format(instance_id, format_name);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_clipboard_is_format_available(PP_Instance instance_id,
                                              PP_Flash_Clipboard_Type clipboard_type,
                                              uint32_t format)
{
    trace_info("[PPB] {full} %s instance_id=%d, clipboard_type=%s, format=%s(%u)\n", __func__+6,
               instance_id, reverse_clipboard_type(clipboard_type),
               reverse_clipboard_format(format), format);
    return ppb_flash_clipboard_is_format_available(instance_id, clipboard_type, format);
}

TRACE_WRAPPER
struct PP_Var
trace_ppb_flash_clipboard_read_data(PP_Instance instance_id, PP_Flash_Clipboard_Type clipboard_type,
                                    uint32_t format)
{
    trace_info("[PPB] {full} %s instance_id=%d, clipboard_type=%s, format=%s(%u)\n", __func__+6,
               instance_id, reverse_clipboard_type(clipboard_type),
               reverse_clipboard_format(format), format);
    return ppb_flash_clipboard_read_data(instance_id, clipboard_type, format);
}

TRACE_WRAPPER
int32_t
trace_ppb_flash_clipboard_write_data(PP_Instance instance_id,
                                     PP_Flash_Clipboard_Type clipboard_type,
                                     uint32_t data_item_count, const uint32_t formats[],
                                     const struct PP_Var data_items[])
{
    trace_info("[PPB] {full} %s instance_id=%d, clipboard_type=%s, data_item_count=%u, "
               "formats[]=%p, data_items[]=%p\n", __func__+6, instance_id,
               reverse_clipboard_type(clipboard_type), data_item_count, formats, data_items);
    return ppb_flash_clipboard_write_data(instance_id, clipboard_type, data_item_count, formats,
                                          data_items);
}

TRACE_WRAPPER
PP_Bool
trace_ppb_flash_clipboard_get_sequence_number(PP_Instance instance_id,
                                              PP_Flash_Clipboard_Type clipboard_type,
                                              uint64_t *sequence_number)
{
    trace_info("[PPB] {zilch} %s instance_id=%d, clipboard_type=%s\n", __func__, instance_id,
               reverse_clipboard_type(clipboard_type));
    return ppb_flash_clipboard_get_sequence_number(instance_id, clipboard_type, sequence_number);
}



const struct PPB_Flash_Clipboard_5_1 ppb_flash_clipboard_interface_5_1 = {
    .RegisterCustomFormat = TWRAPZ(ppb_flash_clipboard_register_custom_format),
    .IsFormatAvailable =    TWRAPF(ppb_flash_clipboard_is_format_available),
    .ReadData =             TWRAPF(ppb_flash_clipboard_read_data),
    .WriteData =            TWRAPF(ppb_flash_clipboard_write_data),
    .GetSequenceNumber =    TWRAPZ(ppb_flash_clipboard_get_sequence_number),
};

const struct PPB_Flash_Clipboard_5_0 ppb_flash_clipboard_interface_5_0 = {
    .RegisterCustomFormat = TWRAPZ(ppb_flash_clipboard_register_custom_format),
    .IsFormatAvailable =    TWRAPF(ppb_flash_clipboard_is_format_available),
    .ReadData =             TWRAPF(ppb_flash_clipboard_read_data),
    .WriteData =            TWRAPF(ppb_flash_clipboard_write_data),
};
