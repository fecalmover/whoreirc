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

#ifndef WHOREIRC_PLUGIN_FSET_BUFFER_H
#define WHOREIRC_PLUGIN_FSET_BUFFER_H

#define FSET_BUFFER_NAME "fset"

struct t_fset_option;

extern struct t_gui_buffer *fset_buffer;
extern int fset_buffer_selected_line;

extern void fset_buffer_set_title ();
extern void fset_buffer_display_option (struct t_fset_option *fset_option);
extern void fset_buffer_refresh (int clear);
extern void fset_buffer_set_current_line (int line);
extern void fset_buffer_check_line_outside_window ();
extern int fset_buffer_window_scrolled_cb (const void *pointer,
                                           void *data,
                                           const char *signal,
                                           const char *type_data,
                                           void *signal_data);
extern void fset_buffer_set_keys ();
extern void fset_buffer_set_localvar_filter ();
extern void fset_buffer_open ();
extern int fset_buffer_init ();
extern void fset_buffer_end ();

#endif /* WHOREIRC_PLUGIN_FSET_BUFFER_H */
