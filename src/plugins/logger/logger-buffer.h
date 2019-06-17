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

#ifndef WHOREIRC_PLUGIN_LOGGER_BUFFER_H
#define WHOREIRC_PLUGIN_LOGGER_BUFFER_H

#include <stdio.h>

struct t_infolist;

struct t_logger_buffer
{
    struct t_gui_buffer *buffer;          /* pointer to buffer              */
    char *log_filename;                   /* log filename                   */
    FILE *log_file;                       /* log file                       */
    int log_enabled;                      /* log enabled ?                  */
    int log_level;                        /* log level (0..9)               */
    int write_start_info_line;            /* 1 if start info line must be   */
                                          /* written in file                */
    int flush_needed;                     /* flush needed?                  */
    struct t_logger_buffer *prev_buffer;  /* link to previous buffer        */
    struct t_logger_buffer *next_buffer;  /* link to next buffer            */
};

extern struct t_logger_buffer *logger_buffers;
extern struct t_logger_buffer *last_logger_buffer;

extern int logger_buffer_valid (struct t_logger_buffer *logger_buffer);
extern struct t_logger_buffer *logger_buffer_add (struct t_gui_buffer *,
                                                  int log_level);
extern struct t_logger_buffer *logger_buffer_search_buffer (struct t_gui_buffer *buffer);
extern struct t_logger_buffer *logger_buffer_search_log_filename (const char *log_filename);
extern void logger_buffer_free (struct t_logger_buffer *logger_buffer);
extern int logger_buffer_add_to_infolist (struct t_infolist *infolist,
                                          struct t_logger_buffer *logger_buffer);

#endif /* WHOREIRC_PLUGIN_LOGGER_BUFFER_H */
