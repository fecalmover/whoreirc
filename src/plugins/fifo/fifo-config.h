/*
 * Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
 *
 * This file is part of WeeChat, the extensible chat client.
 *
 * WeeChat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * WeeChat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WeeChat.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef WHOREIRC_PLUGIN_FIFO_CONFIG_H
#define WHOREIRC_PLUGIN_FIFO_CONFIG_H

#define FIFO_CONFIG_NAME "fifo"


extern struct t_config_option *fifo_config_file_enabled;
extern struct t_config_option *fifo_config_file_path;

extern int fifo_config_init ();
extern int fifo_config_read ();
extern int fifo_config_write ();
extern void fifo_config_free ();

#endif /* WHOREIRC_PLUGIN_FIFO_CONFIG_H */
