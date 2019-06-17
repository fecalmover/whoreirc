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

#ifndef WHOREIRC_GUI_MOUSE_H
#define WHOREIRC_GUI_MOUSE_H

/* mouse variables */

extern int gui_mouse_enabled;
extern int gui_mouse_debug;
extern int gui_mouse_grab;
extern int gui_mouse_event_pending;
extern struct t_hook *gui_mouse_event_timer;
extern int gui_mouse_event_index;
extern int gui_mouse_event_x[2];
extern int gui_mouse_event_y[2];
extern char gui_mouse_event_button;

/* mouse functions */

extern void gui_mouse_debug_set (int debug);
extern void gui_mouse_event_reset ();

/* mouse functions (GUI dependent) */

extern void gui_mouse_enable ();
extern void gui_mouse_disable ();
extern void gui_mouse_display_state ();
extern void gui_mouse_grab_init (int area);
extern void gui_mouse_event_init ();
extern void gui_mouse_event_end ();

#endif /* WHOREIRC_GUI_MOUSE_H */
