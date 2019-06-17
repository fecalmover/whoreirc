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

#ifndef WHOREIRC_PLUGIN_IRC_MSGBUFFER_H
#define WHOREIRC_PLUGIN_IRC_MSGBUFFER_H

enum t_irc_msgbuffer_target
{
    IRC_MSGBUFFER_TARGET_WHOREIRC = 0,
    IRC_MSGBUFFER_TARGET_SERVER,
    IRC_MSGBUFFER_TARGET_CURRENT,
    IRC_MSGBUFFER_TARGET_PRIVATE,
    /* number of msgbuffer targets */
    IRC_MSGBUFFER_NUM_TARGETS,
};

struct t_irc_server;

extern struct t_gui_buffer *irc_msgbuffer_get_target_buffer (struct t_irc_server *server,
                                                             const char *nick,
                                                             const char *message,
                                                             const char *alias,
                                                             struct t_gui_buffer *default_buffer);

#endif /* WHOREIRC_PLUGIN_IRC_MSGBUFFER_H */
