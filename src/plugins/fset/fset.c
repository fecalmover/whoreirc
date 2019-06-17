/*
 * fset.c - Fast set of WhoreIRC and plugins options
 *
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../weechat-plugin.h"
#include "fset.h"
#include "fset-bar-item.h"
#include "fset-buffer.h"
#include "fset-command.h"
#include "fset-completion.h"
#include "fset-config.h"
#include "fset-info.h"
#include "fset-mouse.h"
#include "fset-option.h"


WHOREIRC_PLUGIN_NAME(FSET_PLUGIN_NAME);
WHOREIRC_PLUGIN_DESCRIPTION(N_("Fast set of WhoreIRC and plugins options"));
WHOREIRC_PLUGIN_AUTHOR("Sébastien Helleu <flashcode@flashtux.org>");
WHOREIRC_PLUGIN_VERSION(WHOREIRC_VERSION);
WHOREIRC_PLUGIN_LICENSE(WHOREIRC_LICENSE);
WHOREIRC_PLUGIN_PRIORITY(2000);

struct t_weechat_plugin *weechat_fset_plugin = NULL;

struct t_hdata *fset_hdata_config_file = NULL;
struct t_hdata *fset_hdata_config_section = NULL;
struct t_hdata *fset_hdata_config_option = NULL;
struct t_hdata *fset_hdata_fset_option = NULL;


/*
 * Callback for signal "debug_dump".
 */

int
fset_debug_dump_cb (const void *pointer, void *data,
                    const char *signal, const char *type_data,
                    void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    if (!signal_data
        || (weechat_strcasecmp ((char *)signal_data, FSET_PLUGIN_NAME) == 0))
    {
        weechat_log_printf ("");
        weechat_log_printf ("***** \"%s\" plugin dump *****",
                            weechat_plugin->name);

        fset_option_print_log ();

        weechat_log_printf ("");
        weechat_log_printf ("***** End of \"%s\" plugin dump *****",
                            weechat_plugin->name);
    }

    return WHOREIRC_RC_OK;
}

/*
 * Adds the fset bar.
 */

void
fset_add_bar ()
{
    weechat_bar_new (
        FSET_BAR_NAME, "off", "0", "window",
        "${buffer.full_name} == " FSET_PLUGIN_NAME "." FSET_BAR_NAME,
        "top", "horizontal", "vertical", "3", "3",
        "default", "cyan", "default", "on",
        FSET_BAR_ITEM_NAME);
}

/*
 * Initializes fset plugin.
 */

int
weechat_plugin_init (struct t_weechat_plugin *plugin, int argc, char *argv[])
{
    const char *ptr_filter;

    /* make C compiler happy */
    (void) argc;
    (void) argv;

    weechat_plugin = plugin;

    fset_hdata_config_file = weechat_hdata_get ("config_file");
    fset_hdata_config_section = weechat_hdata_get ("config_section");
    fset_hdata_config_option = weechat_hdata_get ("config_option");

    fset_buffer_init ();

    fset_option_init ();

    if (!fset_config_init ())
        return WHOREIRC_RC_ERROR;

    fset_config_read ();

    if (!fset_bar_item_init ())
        return WHOREIRC_RC_ERROR;

    fset_command_init ();

    fset_completion_init ();

    fset_add_bar ();

    fset_bar_item_update ();

    fset_info_init ();

    fset_hdata_fset_option = weechat_hdata_get ("fset_option");

    /* hook some signals */
    weechat_hook_signal ("debug_dump", &fset_debug_dump_cb, NULL, NULL);
    weechat_hook_signal ("window_scrolled",
                         &fset_buffer_window_scrolled_cb, NULL, NULL);

    fset_mouse_init ();

    weechat_hook_config ("*", &fset_option_config_cb, NULL, NULL);

    if (fset_buffer)
    {
        ptr_filter = weechat_buffer_get_string (fset_buffer, "localvar_filter");
        fset_option_filter_options (ptr_filter);
    }

    return WHOREIRC_RC_OK;
}

/*
 * Ends fset plugin.
 */

int
weechat_plugin_end (struct t_weechat_plugin *plugin)
{
    /* make C compiler happy */
    (void) plugin;

    fset_mouse_end ();

    fset_bar_item_end ();

    fset_buffer_end ();

    fset_option_end ();

    fset_config_write ();
    fset_config_free ();

    return WHOREIRC_RC_OK;
}
