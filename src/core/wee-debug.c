/*
 * wee-debug.c - debug functions
 *
 * Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
 *
 * This file is part of WhoreIRC, the extensible chat client.
 *
 * WhoreIRC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * WhoreIRC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WhoreIRC.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#ifdef HAVE_MALLINFO
#include <malloc.h>
#endif
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <gcrypt.h>
#include <curl/curl.h>
#include <zlib.h>

#ifdef HAVE_GNUTLS
#include <gnutls/gnutls.h>
#endif

#include "weechat.h"
#include "wee-backtrace.h"
#include "wee-config-file.h"
#include "wee-hashtable.h"
#include "wee-hdata.h"
#include "wee-hook.h"
#include "wee-infolist.h"
#include "wee-list.h"
#include "wee-log.h"
#include "wee-proxy.h"
#include "wee-string.h"
#include "wee-util.h"
#include "../gui/gui-bar.h"
#include "../gui/gui-bar-item.h"
#include "../gui/gui-buffer.h"
#include "../gui/gui-chat.h"
#include "../gui/gui-filter.h"
#include "../gui/gui-hotlist.h"
#include "../gui/gui-key.h"
#include "../gui/gui-layout.h"
#include "../gui/gui-main.h"
#include "../gui/gui-window.h"
#include "../plugins/plugin.h"


int debug_dump_active = 0;


/*
 * Writes dump of data to WhoreIRC log file.
 */

void
debug_dump (int crash)
{
    /* prevent reentrancy */
    if (debug_dump_active)
        exit (EXIT_FAILURE);

    if (crash)
    {
        debug_dump_active = 1;
        log_printf ("Very bad, WhoreIRC is crashing (SIGSEGV received)...");
        weechat_log_use_time = 0;
    }

    log_printf ("");
    if (crash)
        log_printf ("****** WhoreIRC CRASH DUMP ******");
    else
        log_printf ("****** WhoreIRC dump request ******");

    gui_window_print_log ();
    gui_buffer_print_log ();
    gui_layout_print_log ();
    gui_key_print_log (NULL);
    gui_filter_print_log ();
    gui_bar_print_log ();
    gui_bar_item_print_log ();
    gui_hotlist_print_log ();

    hdata_print_log ();

    infolist_print_log ();

    hook_print_log ();

    config_file_print_log ();

    proxy_print_log ();

    plugin_print_log ();

    log_printf ("");
    log_printf ("****** End of WhoreIRC dump ******");
    log_printf ("");
}

/*
 * Callback for signal "debug_dump".
 *
 * This function is called when WhoreIRC is crashing or when command
 * "/debug dump" is issued.
 */

int
debug_dump_cb (const void *pointer, void *data,
               const char *signal, const char *type_data,
               void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    if (!signal_data || (string_strcasecmp ((char *)signal_data, "core") == 0))
        debug_dump (0);

    return WHOREIRC_RC_OK;
}

/*
 * Callback for system signal SIGSEGV handler.
 *
 * Writes dump of data and backtrace to WhoreIRC log file, then exit.
 */

void
debug_sigsegv ()
{
    debug_dump (1);
    unhook_all ();
    gui_main_end (0);

    string_fprintf (stderr,
                    "\n*** Very bad! WhoreIRC is crashing (SIGSEGV received)"
                    "\n");
    if (!log_crash_rename ())
    {
        string_fprintf (stderr,
                        "*** Full crash dump was saved to %s/weechat.log file."
                        "\n",
                        weechat_home);
    }
    string_fprintf (
        stderr,
        "***\n"
        "*** Please help WhoreIRC developers to fix this bug:\n"
        "***\n"
        "***   1. If you have a core file, please run: gdb /path/to/weechat core\n"
        "***      then issue command: \"bt full\" and send result to developers.\n"
        "***      See the user's guide for more info about enabling the core files\n"
        "***      and reporting crashes:\n"
        "***      https://weechat.org/doc/stable/user#report_crashes\n"
        "***\n"
        "***   2. Otherwise send the backtrace (below), only if it is a complete trace.\n"
        "***      Keep the crash log file, just in case developers ask you some info\n"
        "***      (be careful, private info like passwords may be in this file).\n\n");

    weechat_backtrace ();

    /* shutdown with error code */
    weechat_shutdown (EXIT_FAILURE, 1);
}

/*
 * Displays tree of windows (this function must not be called directly).
 */

void
debug_windows_tree_display (struct t_gui_window_tree *tree, int indent)
{
    char format[128];

    if (tree)
    {
        if (tree->window)
        {
            /* leaf */
            snprintf (format,
                      sizeof (format),
                      "%%-%dsleaf: 0x%%lx, parent:0x%%lx, child1=0x%%lx, "
                      "child2=0x%%lx, win=0x%%lx (%%d,%%d %%dx%%d"
                      " %%d%%%%x%%d%%%%)",
                      indent * 2);
            gui_chat_printf (NULL,
                             format,
                             " ", tree, tree->parent_node,
                             tree->child1, tree->child2,
                             tree->window,
                             tree->window->win_x, tree->window->win_y,
                             tree->window->win_width, tree->window->win_height,
                             tree->window->win_width_pct,
                             tree->window->win_height_pct);
        }
        else
        {
            /* node */
            snprintf (format,
                      sizeof (format),
                      "%%-%dsnode: 0x%%lx, parent:0x%%lx, pct:%%d, "
                      "horizontal:%%d, child1=0x%%lx, child2=0x%%lx",
                      indent * 2);
            gui_chat_printf (NULL,
                             format,
                             " ", tree, tree->parent_node, tree->split_pct,
                             tree->split_horizontal,
                             tree->child1, tree->child2);
        }

        if (tree->child1)
            debug_windows_tree_display (tree->child1, indent + 1);
        if (tree->child2)
            debug_windows_tree_display (tree->child2, indent + 1);
    }
}

/*
 * Displays tree of windows.
 */

void
debug_windows_tree ()
{
    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, _("Windows tree:"));
    debug_windows_tree_display (gui_windows_tree, 1);
}

/*
 * Displays information about dynamic memory allocation.
 */

void
debug_memory ()
{
#ifdef HAVE_MALLINFO
    struct mallinfo info;

    info = mallinfo ();

    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, _("Memory usage (see \"man mallinfo\" for help):"));
    gui_chat_printf (NULL, "  arena   :%10d", info.arena);
    gui_chat_printf (NULL, "  ordblks :%10d", info.ordblks);
    gui_chat_printf (NULL, "  smblks  :%10d", info.smblks);
    gui_chat_printf (NULL, "  hblks   :%10d", info.hblks);
    gui_chat_printf (NULL, "  hblkhd  :%10d", info.hblkhd);
    gui_chat_printf (NULL, "  usmblks :%10d", info.usmblks);
    gui_chat_printf (NULL, "  fsmblks :%10d", info.fsmblks);
    gui_chat_printf (NULL, "  uordblks:%10d", info.uordblks);
    gui_chat_printf (NULL, "  fordblks:%10d", info.fordblks);
    gui_chat_printf (NULL, "  keepcost:%10d", info.keepcost);
#else
    gui_chat_printf (NULL,
                     _("Memory usage not available (function \"mallinfo\" not "
                       "found)"));
#endif /* HAVE_MALLINFO */
}

/*
 * Callback called for each variable in hdata.
 */

void
debug_hdata_hash_var_map_cb (void *data,
                             struct t_hashtable *hashtable,
                             const void *key, const void *value)
{
    struct t_weelist *list;
    struct t_hdata_var *var;
    char str_offset[16];

    /* make C compiler happy */
    (void) hashtable;

    list = (struct t_weelist *)data;
    var = (struct t_hdata_var *)value;

    snprintf (str_offset, sizeof (str_offset), "%12d", var->offset);
    weelist_add (list, str_offset, WHOREIRC_LIST_POS_SORT, (void *)key);
}

/*
 * Callback called for each list in hdata.
 */

void
debug_hdata_hash_list_map_cb (void *data,
                              struct t_hashtable *hashtable,
                              const void *key, const void *value)
{
    /* make C compiler happy */
    (void) data;
    (void) hashtable;

    gui_chat_printf (NULL,
                     "    list: %s -> 0x%lx",
                     (char *)key,
                     *((void **)value));
}

/*
 * Callback called for each hdata in memory.
 */

void
debug_hdata_map_cb (void *data,
                    struct t_hashtable *hashtable,
                    const void *key, const void *value)
{
    struct t_hdata *ptr_hdata;
    struct t_hdata_var *ptr_var;
    struct t_weelist *list;
    struct t_weelist_item *ptr_item;

    /* make C compiler happy */
    (void) data;
    (void) hashtable;

    ptr_hdata = (struct t_hdata *)value;

    gui_chat_printf (NULL,
                     "  hdata 0x%lx: \"%s\", %d vars, %d lists:",
                     ptr_hdata, (const char *)key,
                     ptr_hdata->hash_var->items_count,
                     ptr_hdata->hash_list->items_count);

    /* display lists */
    hashtable_map (ptr_hdata->hash_list, &debug_hdata_hash_list_map_cb, NULL);

    /* display vars */
    list = weelist_new ();
    hashtable_map (ptr_hdata->hash_var, &debug_hdata_hash_var_map_cb, list);
    for (ptr_item = list->items; ptr_item;
         ptr_item = ptr_item->next_item)
    {
        ptr_var = hashtable_get (ptr_hdata->hash_var, ptr_item->user_data);
        if (ptr_var)
        {
            gui_chat_printf (NULL,
                             "    %04d -> %s (%s%s%s%s%s%s)",
                             ptr_var->offset,
                             (char *)ptr_item->user_data,
                             hdata_type_string[(int)ptr_var->type],
                             (ptr_var->update_allowed) ? ", R/W" : "",
                             (ptr_var->array_size) ? ", array size: " : "",
                             (ptr_var->array_size) ? ptr_var->array_size : "",
                             (ptr_var->hdata_name) ? ", hdata: " : "",
                             (ptr_var->hdata_name) ? ptr_var->hdata_name : "");
        }
    }
    weelist_free (list);
}

/*
 * Displays a list of hdata in memory.
 */

void
debug_hdata ()
{
    int count;

    count = weechat_hdata->items_count;

    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, "%d hdata in memory", count);

    if (count > 0)
        hashtable_map (weechat_hdata, &debug_hdata_map_cb, NULL);
}

/*
 * Displays info about hooks.
 */

void
debug_hooks ()
{
    int i;

    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, "hooks in memory:");

    for (i = 0; i < HOOK_NUM_TYPES; i++)
    {
        gui_chat_printf (NULL, "%17s:%5d",
                         hook_type_string[i], hooks_count[i]);
    }
    gui_chat_printf (NULL, "%17s------", "---------");
    gui_chat_printf (NULL, "%17s:%5d", "total", hooks_count_total);
}

/*
 * Displays a list of infolists in memory.
 */

void
debug_infolists ()
{
    struct t_infolist *ptr_infolist;
    struct t_infolist_item *ptr_item;
    struct t_infolist_var *ptr_var;
    int i, count, count_items, count_vars, size_structs, size_data;
    int total_items, total_vars, total_size;

    count = 0;
    for (ptr_infolist = weechat_infolists; ptr_infolist;
         ptr_infolist = ptr_infolist->next_infolist)
    {
        count++;
    }

    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, "%d infolists in memory (%s)", count,
                     (count == 0) ?
                     "this is OK!" :
                     "WARNING: this is probably a memory leak in WhoreIRC or "
                     "plugins/scripts!");

    if (count > 0)
    {
        i = 0;
        total_items = 0;
        total_vars = 0;
        total_size = 0;
        for (ptr_infolist = weechat_infolists; ptr_infolist;
             ptr_infolist = ptr_infolist->next_infolist)
        {
            count_items = 0;
            count_vars = 0;
            size_structs = sizeof (*ptr_infolist);
            size_data = 0;
            for (ptr_item = ptr_infolist->items; ptr_item;
                 ptr_item = ptr_item->next_item)
            {
                count_items++;
                total_items++;
                size_structs += sizeof (*ptr_item);
                for (ptr_var = ptr_item->vars; ptr_var;
                     ptr_var = ptr_var->next_var)
                {
                    count_vars++;
                    total_vars++;
                    size_structs += sizeof (*ptr_var);
                    if (ptr_var->value)
                    {
                        switch (ptr_var->type)
                        {
                            case INFOLIST_INTEGER:
                                size_data += sizeof (int);
                                break;
                            case INFOLIST_STRING:
                                size_data += strlen ((char *)(ptr_var->value));
                                break;
                            case INFOLIST_POINTER:
                                size_data += sizeof (void *);
                                break;
                            case INFOLIST_BUFFER:
                                size_data += ptr_var->size;
                                break;
                            case INFOLIST_TIME:
                                size_data += sizeof (time_t);
                                break;
                        }
                    }
                }
            }
            gui_chat_printf (NULL,
                             "%4d: infolist 0x%lx: %d items, %d vars - "
                             "structs: %d, data: %d (total: %d bytes)",
                             i + 1, ptr_infolist, count_items, count_vars,
                             size_structs, size_data, size_structs + size_data);
            total_size += size_structs + size_data;
            i++;
        }
        gui_chat_printf (NULL,
                         "Total: %d items, %d vars - %d bytes",
                         total_items, total_vars, total_size);
    }
}

/*
 * Callback for signal "debug_libs": displays infos about external libraries
 * used (called when command "/debug libs" is issued).
 *
 * Note: this function displays libraries for WhoreIRC core only: plugins can
 * catch this signal to display their external libraries.
 */

int
debug_libs_cb (const void *pointer, void *data,
               const char *signal, const char *type_data,
               void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;
    (void) signal_data;

    gui_chat_printf (NULL, "  core:");

    /* display ncurses version */
    gui_main_debug_libs ();

    /* display gcrypt version */
#ifdef GCRYPT_VERSION
    gui_chat_printf (NULL, "    gcrypt: %s%s",
                     GCRYPT_VERSION,
                     (weechat_no_gcrypt) ? " (not initialized)" : "");
#else
    gui_chat_printf (NULL, "    gcrypt: (?)%s",
                     (weechat_no_gcrypt) ? " (not initialized)" : "");
#endif /* GCRYPT_VERSION */

    /* display gnutls version */
#ifdef HAVE_GNUTLS
#ifdef GNUTLS_VERSION
    gui_chat_printf (NULL, "    gnutls: %s%s",
                     GNUTLS_VERSION,
                     (weechat_no_gnutls) ? " (not initialized)" : "");
#else
    gui_chat_printf (NULL, "    gnutls: (?)%s",
                     (weechat_no_gnutls) ? " (not initialized)" : "");
#endif /* GNUTLS_VERSION */
#else
    gui_chat_printf (NULL, "    gnutls: (not available)");
#endif /* HAVE_GNUTLS */

    /* display curl version */
#ifdef LIBCURL_VERSION
    gui_chat_printf (NULL, "    curl: %s", LIBCURL_VERSION);
#else
    gui_chat_printf (NULL, "    curl: (?)");
#endif /* LIBCURL_VERSION */

    /* display zlib version */
#ifdef ZLIB_VERSION
    gui_chat_printf (NULL, "    zlib: %s", ZLIB_VERSION);
#else
    gui_chat_printf (NULL, "    zlib: (?)");
#endif /* ZLIB_VERSION */

    return WHOREIRC_RC_OK;
}

/*
 * Displays WhoreIRC directories.
 */

void
debug_directories ()
{
    char *extra_libdir;

    extra_libdir = getenv (WHOREIRC_EXTRA_LIBDIR);

    gui_chat_printf (NULL, "");
    gui_chat_printf (NULL, _("Directories:"));
    gui_chat_printf (NULL, "  home: %s%s%s",
                     weechat_home,
                     (weechat_home_temp) ? " " : "",
                     (weechat_home_temp) ? _("(TEMPORARY, deleted on exit)") : "");
    gui_chat_printf (NULL, _("        (default: %s)"),
                     WHOREIRC_HOME);
    gui_chat_printf (NULL, "  lib: %s", WHOREIRC_LIBDIR);
    gui_chat_printf (NULL, "  lib (extra): %s",
                     (extra_libdir && extra_libdir[0]) ? extra_libdir : "-");
    gui_chat_printf (NULL, "  share: %s", WHOREIRC_SHAREDIR);
    gui_chat_printf (NULL, "  locale: %s", LOCALEDIR);
}

/*
 * Display time elapsed between two times.
 *
 * If display is 1, the message is displayed in core buffer, otherwise it's
 * written in log file.
 */

void
debug_display_time_elapsed (struct timeval *time1, struct timeval *time2,
                            const char *message, int display)
{
    struct timeval debug_timeval_end;
    long long diff, diff_hour, diff_min, diff_sec, diff_usec;

    gettimeofday (&debug_timeval_end, NULL);
    diff = util_timeval_diff (time1, time2);

    diff_usec = diff % 1000000;
    diff_sec = (diff / 1000000) % 60;
    diff_min = ((diff / 1000000) / 60) % 60;
    diff_hour = (diff / 1000000) / 3600;

    if (display)
    {
        gui_chat_printf (NULL,
                         "debug: time[%s] -> %lld:%02lld:%02lld.%06lld",
                         (message) ? message : "?",
                         diff_hour, diff_min, diff_sec, diff_usec);
    }
    else
    {
        log_printf ("debug: time[%s] -> %lld:%02lld:%02lld.%06lld",
                    (message) ? message : "?",
                    diff_hour, diff_min, diff_sec, diff_usec);
    }
}

/*
 * Initializes debug.
 */

void
debug_init ()
{
    /*
     * hook signals with high priority, to be sure they will be used before
     * plugins (they should anyway because this function is called before load
     * of plugins)
     */
    hook_signal (NULL, "2000|debug_dump", &debug_dump_cb, NULL, NULL);
    hook_signal (NULL, "2000|debug_libs", &debug_libs_cb, NULL, NULL);
}

/*
 * Ends debug.
 */

void
debug_end ()
{
}
