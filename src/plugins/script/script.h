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

#ifndef WHOREIRC_PLUGIN_SCRIPT_H
#define WHOREIRC_PLUGIN_SCRIPT_H

#define weechat_plugin weechat_script_plugin
#define SCRIPT_PLUGIN_NAME "script"

#define SCRIPT_NUM_LANGUAGES 8

extern struct t_weechat_plugin *weechat_script_plugin;

extern char *script_language[SCRIPT_NUM_LANGUAGES];
extern char *script_extension[SCRIPT_NUM_LANGUAGES];
extern int script_plugin_loaded[SCRIPT_NUM_LANGUAGES];
extern struct t_hashtable *script_loaded;

extern int script_language_search (const char *language);
extern int script_language_search_by_extension (const char *extension);
extern char *script_build_download_url (const char *url);
extern void script_get_loaded_plugins ();
extern void script_get_scripts ();

#endif /* WHOREIRC_PLUGIN_SCRIPT_H */
