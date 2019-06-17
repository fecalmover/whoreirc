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

#ifndef WHOREIRC_PLUGIN_BUFLIST_H
#define WHOREIRC_PLUGIN_BUFLIST_H

#define weechat_plugin weechat_buflist_plugin
#define BUFLIST_PLUGIN_NAME "buflist"

#define BUFLIST_BAR_NAME "buflist"

extern struct t_weechat_plugin *weechat_buflist_plugin;

extern struct t_hdata *buflist_hdata_window;
extern struct t_hdata *buflist_hdata_buffer;
extern struct t_hdata *buflist_hdata_hotlist;
extern struct t_hdata *buflist_hdata_bar;
extern struct t_hdata *buflist_hdata_bar_item;
extern struct t_hdata *buflist_hdata_bar_window;

extern void buflist_add_bar ();
extern void buflist_buffer_get_irc_pointers (struct t_gui_buffer *buffer,
                                             void **irc_server,
                                             void **irc_channel);
extern struct t_arraylist *buflist_sort_buffers ();

#endif /* WHOREIRC_PLUGIN_BUFLIST_H */
