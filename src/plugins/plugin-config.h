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

#ifndef WHOREIRC_PLUGIN_PLUGIN_CONFIG_H
#define WHOREIRC_PLUGIN_PLUGIN_CONFIG_H

#define PLUGIN_CONFIG_NAME "plugins"

extern struct t_config_file *plugin_config;
extern struct t_config_option *plugin_options;

extern struct t_config_option *plugin_config_search (const char *plugin_name,
                                                     const char *option_name);
extern int plugin_config_set (const char *plugin_name, const char *option_name,
                              const char *value);
extern void plugin_config_set_desc (const char *plugin_name,
                                    const char *option_name,
                                    const char *description);
extern void plugin_config_init ();
extern int plugin_config_read ();
extern int plugin_config_write ();
extern void plugin_config_end ();

#endif /* WHOREIRC_PLUGIN_PLUGIN_CONFIG_H */
