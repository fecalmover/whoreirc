/*
 * Copyright (C) 2006 Emmanuel Bouthenot <kolter@openics.org>
 * Copyright (C) 2006-2019 Sébastien Helleu <flashcode@flashtux.org>
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

#ifndef WHOREIRC_PLUGIN_SPELL_H
#define WHOREIRC_PLUGIN_SPELL_H

#ifdef USE_ENCHANT
#include <enchant.h>
#else
#include <aspell.h>
#endif /* USE_ENCHANT */

#define weechat_plugin weechat_spell_plugin
#define SPELL_PLUGIN_NAME "spell"

struct t_spell_code
{
    char *code;
    char *name;
};

#ifdef USE_ENCHANT
extern EnchantBroker *broker;
#endif /* USE_ENCHANT */

extern struct t_weechat_plugin *weechat_spell_plugin;
extern int spell_enabled;
extern struct t_spell_code spell_langs[];
extern struct t_spell_code spell_countries[];

extern char *spell_build_option_name (struct t_gui_buffer *buffer);
extern const char *spell_get_dict_with_buffer_name (const char *name);
extern const char *spell_get_dict (struct t_gui_buffer *buffer);

#endif /* WHOREIRC_PLUGIN_SPELL_H */
