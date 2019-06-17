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

#ifndef WHOREIRC_PLUGIN_IRC_CTCP_H
#define WHOREIRC_PLUGIN_IRC_CTCP_H

#include <time.h>

struct t_irc_server;
struct t_irc_channel;

struct t_irc_ctcp_reply
{
    char *name;                     /* CTCP name                             */
    char *reply;                    /* CTCP reply format                     */
};

extern const char *irc_ctcp_get_default_reply (const char *ctcp);
extern void irc_ctcp_display_reply_from_nick (struct t_irc_server *server,
                                              time_t date,
                                              const char *command,
                                              const char *nick,
                                              const char *address,
                                              char *arguments);
extern void irc_ctcp_recv (struct t_irc_server *server, time_t date,
                           const char *command, struct t_irc_channel *channel,
                           const char *address, const char *nick,
                           const char *remote_nick, char *arguments,
                           char *message);

#endif /* WHOREIRC_PLUGIN_IRC_CTCP_H */
