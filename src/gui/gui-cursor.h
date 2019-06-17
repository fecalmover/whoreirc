/*
 * Copyright (C) 2011-2019 Sébastien Helleu <flashcode@flashtux.org>
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

#ifndef WHOREIRC_GUI_CURSOR_H
#define WHOREIRC_GUI_CURSOR_H

/* cursor variables */

extern int gui_cursor_mode;
extern int gui_cursor_debug;
extern int gui_cursor_x;
extern int gui_cursor_y;

/* cursor functions */

extern void gui_cursor_mode_toggle ();
extern void gui_cursor_mode_stop ();
extern void gui_cursor_debug_set (int debug);
extern void gui_cursor_move_xy (int x, int y);
extern void gui_cursor_move_add_xy (int add_x, int add_y);
extern void gui_cursor_move_area_add_xy (int add_x, int add_y);
extern void gui_cursor_move_area (const char *area);

#endif /* WHOREIRC_GUI_CURSOR_H */
