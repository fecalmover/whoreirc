/*
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

#ifndef WHOREIRC_PLUGIN_BUFLIST_CONFIG_H
#define WHOREIRC_PLUGIN_BUFLIST_CONFIG_H

#define BUFLIST_CONFIG_NAME "buflist"

#define BUFLIST_CONFIG_SIGNALS_REFRESH                                  \
    "buffer_opened,buffer_closed,buffer_merged,buffer_unmerged,"        \
    "buffer_moved,buffer_renamed,buffer_switch,buffer_hidden,"          \
    "buffer_unhidden,buffer_localvar_added,buffer_localvar_changed,"    \
    "window_switch,hotlist_changed"
#define BUFLIST_CONFIG_SIGNALS_REFRESH_NICK_PREFIX                      \
    "nicklist_nick_*"

extern struct t_config_file *buflist_config_file;

extern struct t_config_option *buflist_config_look_add_newline;
extern struct t_config_option *buflist_config_look_auto_scroll;
extern struct t_config_option *buflist_config_look_display_conditions;
extern struct t_config_option *buflist_config_look_enabled;
extern struct t_config_option *buflist_config_look_mouse_jump_visited_buffer;
extern struct t_config_option *buflist_config_look_mouse_move_buffer;
extern struct t_config_option *buflist_config_look_mouse_wheel;
extern struct t_config_option *buflist_config_look_nick_prefix;
extern struct t_config_option *buflist_config_look_nick_prefix_empty;
extern struct t_config_option *buflist_config_look_signals_refresh;
extern struct t_config_option *buflist_config_look_sort;

extern struct t_config_option *buflist_config_format_buffer;
extern struct t_config_option *buflist_config_format_buffer_current;
extern struct t_config_option *buflist_config_format_hotlist;
extern struct t_config_option *buflist_config_format_hotlist_level[4];
extern struct t_config_option *buflist_config_format_hotlist_level_none;
extern struct t_config_option *buflist_config_format_hotlist_separator;
extern struct t_config_option *buflist_config_format_indent;
extern struct t_config_option *buflist_config_format_lag;
extern struct t_config_option *buflist_config_format_name;
extern struct t_config_option *buflist_config_format_nick_prefix;
extern struct t_config_option *buflist_config_format_number;

extern char **buflist_config_sort_fields;
extern int buflist_config_sort_fields_count;
extern char *buflist_config_format_buffer_eval;
extern char *buflist_config_format_buffer_current_eval;
extern char *buflist_config_format_hotlist_eval;

extern int buflist_config_init ();
extern int buflist_config_read ();
extern int buflist_config_write ();
extern void buflist_config_free ();

#endif /* WHOREIRC_PLUGIN_BUFLIST_CONFIG_H */
