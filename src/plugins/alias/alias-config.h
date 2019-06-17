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

#ifndef WHOREIRC_PLUGIN_ALIAS_CONFIG_H
#define WHOREIRC_PLUGIN_ALIAS_CONFIG_H

extern struct t_config_file *alias_config_file;
extern struct t_config_section *alias_config_section_cmd;
extern struct t_config_section *alias_config_section_completion;

extern char *alias_default[][3];

extern void alias_config_cmd_new_option (const char *name,
                                         const char *command);
extern void alias_config_completion_new_option (const char *name,
                                                const char *completion);
extern int alias_config_init ();
extern int alias_config_read ();
extern int alias_config_write ();

#endif /* WHOREIRC_PLUGIN_ALIAS_CONFIG_H */
