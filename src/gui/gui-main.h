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

#ifndef WHOREIRC_GUI_MAIN_H
#define WHOREIRC_GUI_MAIN_H

/* main functions (GUI dependent) */

extern void gui_main_get_password (const char **prompt,
                                   char *password, int size);
extern void gui_main_debug_libs ();
extern void gui_main_end (int clean_exit);

/* terminal functions (GUI dependent) */
extern void gui_term_set_eat_newline_glitch (int value);

#endif /* WHOREIRC_GUI_MAIN_H */
