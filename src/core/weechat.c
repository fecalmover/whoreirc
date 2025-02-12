/*
 * ##########################################################################
 * ##          ___       __         ______________        _____            ##
 * ##          __ |     / /___________  ____/__  /_______ __  /_           ##
 * ##          __ | /| / /_  _ \  _ \  /    __  __ \  __ `/  __/           ##
 * ##          __ |/ |/ / /  __/  __/ /___  _  / / / /_/ // /_             ##
 * ##          ____/|__/  \___/\___/\____/  /_/ /_/\__,_/ \__/             ##
 * ##                                                                      ##
 * ##             WhoreIRC - Wee Enhanced Environment for Chat              ##
 * ##                 Fast, light, extensible chat client                  ##
 * ##                                                                      ##
 * ##             By Sébastien Helleu <flashcode@flashtux.org>             ##
 * ##                                                                      ##
 * ##                         https://weechat.org/                         ##
 * ##                                                                      ##
 * ##########################################################################
 *
 * weechat.c - WhoreIRC main functions
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#ifdef HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#include "weechat.h"
#include "wee-command.h"
#include "wee-completion.h"
#include "wee-config.h"
#include "wee-debug.h"
#include "wee-eval.h"
#include "wee-hdata.h"
#include "wee-hook.h"
#include "wee-list.h"
#include "wee-log.h"
#include "wee-network.h"
#include "wee-proxy.h"
#include "wee-secure.h"
#include "wee-secure-config.h"
#include "wee-string.h"
#include "wee-upgrade.h"
#include "wee-utf8.h"
#include "wee-util.h"
#include "wee-version.h"
#include "../gui/gui-chat.h"
#include "../gui/gui-color.h"
#include "../gui/gui-completion.h"
#include "../gui/gui-key.h"
#include "../gui/gui-layout.h"
#include "../gui/gui-main.h"
#include "../plugins/plugin.h"
#include "../plugins/plugin-api.h"

/* debug command line options */
#define OPTION_NO_DLCLOSE 1000
#define OPTION_NO_GNUTLS  1001
#define OPTION_NO_GCRYPT  1002

int weechat_headless = 0;              /* 1 if running headless (no GUI)    */
int weechat_debug_core = 0;            /* debug level for core              */
char *weechat_argv0 = NULL;            /* WhoreIRC binary file name (argv[0])*/
int weechat_upgrading = 0;             /* =1 if WhoreIRC is upgrading        */
int weechat_first_start = 0;           /* first start of WhoreIRC?           */
time_t weechat_first_start_time = 0;   /* start time (used by /uptime cmd)  */
int weechat_upgrade_count = 0;         /* number of /upgrade done           */
struct timeval weechat_current_start_timeval; /* start time used to display */
                                       /* duration of /upgrade              */
volatile sig_atomic_t weechat_quit = 0;   /* = 1 if quit request from user  */
volatile sig_atomic_t weechat_quit_signal = 0; /* signal received,          */
                                       /* WhoreIRC must quit                 */
char *weechat_home = NULL;             /* home dir. (default: ~/.weechat)   */
int weechat_home_temp = 0;             /* 1 if using a temporary home       */
int weechat_home_delete_on_exit = 0;   /* 1 if home is deleted on exit      */
int weechat_locale_ok = 0;             /* is locale OK?                     */
char *weechat_local_charset = NULL;    /* example: ISO-8859-1, UTF-8        */
int weechat_server_cmd_line = 0;       /* at least 1 server on cmd line     */
char *weechat_force_plugin_autoload = NULL; /* force load of plugins        */
int weechat_plugin_no_dlclose = 0;     /* remove calls to dlclose for libs  */
                                       /* (useful with valgrind)            */
int weechat_no_gnutls = 0;             /* remove init/deinit of gnutls      */
                                       /* (useful with valgrind/electric-f.)*/
int weechat_no_gcrypt = 0;             /* remove init/deinit of gcrypt      */
                                       /* (useful with valgrind)            */
struct t_weelist *weechat_startup_commands = NULL; /* startup commands      */
                                                   /* (option -r)           */


/*
 * Displays WhoreIRC copyright on standard output.
 */

void
weechat_display_copyright ()
{
    string_fprintf (stdout, "\n");
    string_fprintf (
        stdout,
        /* TRANSLATORS: "%s %s" after "compiled on" is date and time */
        _("WhoreIRC %s Copyright %s, compiled on %s %s\n"
          "Developed by Sébastien Helleu <flashcode@flashtux.org> "
          "- %s"),
        version_get_version_with_git (),
        WHOREIRC_COPYRIGHT_DATE,
        version_get_compilation_date (),
        version_get_compilation_time (),
        WHOREIRC_WEBSITE);
    string_fprintf (stdout, "\n");
}

/*
 * Displays WhoreIRC usage on standard output.
 */

void
weechat_display_usage ()
{
    weechat_display_copyright ();
    string_fprintf (stdout, "\n");
    string_fprintf (stdout,
                    _("Usage: %s [option...] [plugin:option...]\n"),
                    weechat_argv0);
    string_fprintf (stdout, "\n");
    string_fprintf (
        stdout,
        _("  -a, --no-connect         disable auto-connect to servers at "
          "startup\n"
          "  -c, --colors             display default colors in terminal\n"
          "  -d, --dir <path>         set WhoreIRC home directory "
          "(default: ~/.weechat)\n"
          "                           (environment variable WHOREIRC_HOME is "
          "read if this option is not given)\n"
          "  -t, --temp-dir           create a temporary WhoreIRC home"
          "directory and delete it on exit\n"
          "                           (incompatible with option \"-d\")\n"
          "  -h, --help               display this help\n"
          "  -l, --license            display WhoreIRC license\n"
          "  -p, --no-plugin          don't load any plugin at startup\n"
          "  -P, --plugins <plugins>  load only these plugins at startup\n"
          "                           (see /help weechat.plugin.autoload)\n"
          "  -r, --run-command <cmd>  run command(s) after startup;\n"
          "                           many commands can be separated by "
          "semicolons,\n"
          "                           this option can be given multiple "
          "times\n"
          "  -s, --no-script          don't load any script at startup\n"
          "      --upgrade            upgrade WhoreIRC using session files "
          "(see /help upgrade in WhoreIRC)\n"
          "  -v, --version            display WhoreIRC version\n"
          "  plugin:option            option for plugin (see man weechat)\n"));
    string_fprintf (stdout, "\n");

    /* extra options in headless mode */
    if (weechat_headless)
    {
        string_fprintf (stdout, _("Extra options in headless mode:\n"));
        string_fprintf (
            stdout,
            _("      --daemon             run WhoreIRC as a daemon (fork, "
              "new process group, file descriptors closed);\n"));
        string_fprintf (
            stdout,
            _("                           (by default in headless mode "
              "WhoreIRC is blocking and does not run in background)\n"));
        string_fprintf (stdout, "\n");
    }

    /* debug options */
    string_fprintf (
        stdout,
        _("Debug options (for tools like valgrind, DO NOT USE IN PRODUCTION):\n"
          "      --no-dlclose         do not call function dlclose after "
          "plugins are unloaded\n"
          "      --no-gnutls          disable init/deinit of gnutls\n"
          "      --no-gcrypt          disable init/deinit of gcrypt\n"));
    string_fprintf (stdout, "\n");
}

/*
 * Parses command line arguments.
 *
 * Arguments argc and argv come from main() function.
 */

void
weechat_parse_args (int argc, char *argv[])
{
    int opt;
    struct option long_options[] = {
        /* standard options */
        { "no-connect",  no_argument,       NULL, 'a'  },
        { "colors",      no_argument,       NULL, 'c'  },
        { "dir",         required_argument, NULL, 'd'  },
        { "temp-dir",    no_argument,       NULL, 't'  },
        { "help",        no_argument,       NULL, 'h'  },
        { "license",     no_argument,       NULL, 'l'  },
        { "no-plugin",   no_argument,       NULL, 'p'  },
        { "plugins",     required_argument, NULL, 'P'  },
        { "run-command", required_argument, NULL, 'r'  },
        { "no-script",   no_argument,       NULL, 's'  },
        { "upgrade",     no_argument,       NULL, 'u'  },
        { "version",     no_argument,       NULL, 'v'  },
        /* debug options */
        { "no-dlclose",  no_argument,       NULL, OPTION_NO_DLCLOSE },
        { "no-gnutls",   no_argument,       NULL, OPTION_NO_GNUTLS  },
        { "no-gcrypt",   no_argument,       NULL, OPTION_NO_GCRYPT  },
        { NULL,          0,                 NULL, 0 },
    };

    weechat_argv0 = (argv[0]) ? strdup (argv[0]) : NULL;
    weechat_upgrading = 0;
    weechat_home = NULL;
    weechat_home_temp = 0;
    weechat_home_delete_on_exit = 0;
    weechat_server_cmd_line = 0;
    weechat_force_plugin_autoload = NULL;
    weechat_plugin_no_dlclose = 0;

    optind = 0;
    opterr = 0;

    while ((opt = getopt_long (argc, argv,
                               ":acd:thlpP:r:sv",
                               long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'a': /* -a / --no-connect */
                /* option ignored, it will be used by the irc plugin */
                break;
            case 'c': /* -c / --colors */
                gui_color_display_terminal_colors ();
                weechat_shutdown (EXIT_SUCCESS, 0);
                break;
            case 'd': /* -d / --dir */
                weechat_home_temp = 0;
                if (weechat_home)
                    free (weechat_home);
                weechat_home = strdup (optarg);
                break;
            case 't': /* -t / --temp-dir */
                weechat_home_temp = 1;
                if (weechat_home)
                {
                    free (weechat_home);
                    weechat_home = NULL;
                }
                break;
            case 'h': /* -h / --help */
                weechat_display_usage ();
                weechat_shutdown (EXIT_SUCCESS, 0);
                break;
            case 'l': /* -l / --license */
                weechat_display_copyright ();
                string_fprintf (stdout, "\n");
                string_fprintf (stdout, "%s%s", WHOREIRC_LICENSE_TEXT);
                weechat_shutdown (EXIT_SUCCESS, 0);
                break;
            case 'p': /* -p / --no-plugin */
                if (weechat_force_plugin_autoload)
                    free (weechat_force_plugin_autoload);
                weechat_force_plugin_autoload = strdup ("!*");
                break;
            case 'P': /* -P / --plugins */
                if (weechat_force_plugin_autoload)
                    free (weechat_force_plugin_autoload);
                weechat_force_plugin_autoload = strdup (optarg);
                break;
            case 'r': /* -r / --run-command */
                if (!weechat_startup_commands)
                    weechat_startup_commands = weelist_new ();
                weelist_add (weechat_startup_commands, optarg,
                             WHOREIRC_LIST_POS_END, NULL);
                break;
            case 's': /* -s / --no-script */
                /* option ignored, it will be used by the scripting plugins */
                break;
            case 'u': /* --upgrade */
                weechat_upgrading = 1;
                break;
            case 'v': /* -v / --version */
                string_fprintf (stdout, version_get_version ());
                fprintf (stdout, "\n");
                weechat_shutdown (EXIT_SUCCESS, 0);
                break;
            case OPTION_NO_DLCLOSE: /* --no-dlclose */
                /*
                 * Valgrind works better when dlclose() is not done after
                 * plugins are unloaded, it can display stack for plugins,*
                 * otherwise you'll see "???" in stack for functions of
                 * unloaded plugins.
                 * This option disables the call to dlclose(),
                 * it must NOT be used for other purposes!
                 */
                weechat_plugin_no_dlclose = 1;
                break;
            case OPTION_NO_GNUTLS: /* --no-gnutls */
                /*
                 * Electric-fence is not working fine when gnutls loads
                 * certificates and Valgrind reports many memory errors with
                 * gnutls.
                 * This option disables the init/deinit of gnutls,
                 * it must NOT be used for other purposes!
                 */
                weechat_no_gnutls = 1;
                break;
            case OPTION_NO_GCRYPT: /* --no-gcrypt */
                /*
                 * Valgrind reports many memory errors with gcrypt.
                 * This option disables the init/deinit of gcrypt,
                 * it must NOT be used for other purposes!
                 */
                weechat_no_gcrypt = 1;
                break;
            case ':':
                string_fprintf (stderr,
                                _("Error: missing argument for \"%s\" option\n"),
                                argv[optind - 1]);
                weechat_shutdown (EXIT_FAILURE, 0);
                break;
            case '?':
                /* ignore any unknown option; plugins can use them */
                break;
            default:
                /* ignore any other error */
                break;
        }
    }
}

/*
 * Expands and assigns given path to "weechat_home".
 */

void
weechat_set_home_path (char *home_path)
{
    char *ptr_home;
    int dir_length;

    if (home_path[0] == '~')
    {
        /* replace leading '~' by $HOME */
        ptr_home = getenv ("HOME");
        if (!ptr_home)
        {
            string_fprintf (stderr, _("Error: unable to get HOME directory\n"));
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
        dir_length = strlen (ptr_home) + strlen (home_path + 1) + 1;
        weechat_home = malloc (dir_length);
        if (weechat_home)
        {
            snprintf (weechat_home, dir_length,
                      "%s%s", ptr_home, home_path + 1);
        }
    }
    else
    {
        weechat_home = strdup (home_path);
    }

    if (!weechat_home)
    {
        string_fprintf (stderr,
                        _("Error: not enough memory for home directory\n"));
        weechat_shutdown (EXIT_FAILURE, 0);
        /* make C static analyzer happy (never executed) */
        return;
    }
}

/*
 * Creates WhoreIRC home directory (by default ~/.weechat).
 *
 * Any error in this function is fatal: WhoreIRC can not run without a home
 * directory.
 */

void
weechat_create_home_dir ()
{
    char *temp_dir, *temp_home_template, *ptr_weechat_home;
    char *config_weechat_home;
    int length, add_separator;
    struct stat statinfo;

    /* temporary WhoreIRC home */
    if (weechat_home_temp)
    {
        temp_dir = util_get_temp_dir ();
        if (!temp_dir || !temp_dir[0])
        {
            string_fprintf (stderr,
                            _("Error: not enough memory for home "
                              "directory\n"));
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
        length = strlen (temp_dir) + 32 + 1;
        temp_home_template = malloc (length);
        if (!temp_home_template)
        {
            free (temp_dir);
            string_fprintf (stderr,
                            _("Error: not enough memory for home "
                              "directory\n"));
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
        add_separator = (temp_dir[strlen (temp_dir) - 1] != DIR_SEPARATOR_CHAR);
        snprintf (temp_home_template, length,
                  "%s%sweechat_temp_XXXXXX",
                  temp_dir,
                  add_separator ? DIR_SEPARATOR : "");
        free (temp_dir);
        ptr_weechat_home = mkdtemp (temp_home_template);
        if (!ptr_weechat_home)
        {
            string_fprintf (stderr,
                            _("Error: unable to create a temporary "
                              "home directory (using template: \"%s\")\n"),
                            temp_home_template);
            free (temp_home_template);
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
        weechat_home = strdup (ptr_weechat_home);
        free (temp_home_template);
        weechat_home_delete_on_exit = 1;
        return;
    }

    /*
     * weechat_home is not set yet: look for environment variable
     * "WHOREIRC_HOME"
     */
    if (!weechat_home)
    {
        ptr_weechat_home = getenv ("WHOREIRC_HOME");
        if (ptr_weechat_home && ptr_weechat_home[0])
            weechat_set_home_path (ptr_weechat_home);
    }

    /* weechat_home is still not set: try to use compile time default */
    if (!weechat_home)
    {
        config_weechat_home = WHOREIRC_HOME;
        if (!config_weechat_home[0])
        {
            string_fprintf (stderr,
                            _("Error: WHOREIRC_HOME is undefined, check build "
                              "options\n"));
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
        weechat_set_home_path (config_weechat_home);
    }

    /* if home already exists, it has to be a directory */
    if (stat (weechat_home, &statinfo) == 0)
    {
        if (!S_ISDIR (statinfo.st_mode))
        {
            string_fprintf (stderr,
                            _("Error: home (%s) is not a directory\n"),
                            weechat_home);
            weechat_shutdown (EXIT_FAILURE, 0);
            /* make C static analyzer happy (never executed) */
            return;
        }
    }

    /* create home directory; error is fatal */
    if (!util_mkdir (weechat_home, 0755))
    {
        string_fprintf (stderr,
                        _("Error: cannot create directory \"%s\"\n"),
                        weechat_home);
        weechat_shutdown (EXIT_FAILURE, 0);
        /* make C static analyzer happy (never executed) */
        return;
    }
}

/*
 * Displays WhoreIRC startup message.
 */

void
weechat_startup_message ()
{
    if (weechat_headless)
    {
        string_fprintf (stdout,
                        _("WhoreIRC is running in headless mode "
                          "(Ctrl-C to quit)."));
        string_fprintf (stdout, "\n");
    }

    if (CONFIG_BOOLEAN(config_startup_display_logo))
    {
        gui_chat_printf (
            NULL,
            "%s  ___       __         ______________        _____ \n"
            "%s  __ |     / /___________  ____/__  /_______ __  /_\n"
            "%s  __ | /| / /_  _ \\  _ \\  /    __  __ \\  __ `/  __/\n"
            "%s  __ |/ |/ / /  __/  __/ /___  _  / / / /_/ // /_  \n"
            "%s  ____/|__/  \\___/\\___/\\____/  /_/ /_/\\__,_/ \\__/  ",
            GUI_COLOR(GUI_COLOR_CHAT_NICK),
            GUI_COLOR(GUI_COLOR_CHAT_NICK),
            GUI_COLOR(GUI_COLOR_CHAT_NICK),
            GUI_COLOR(GUI_COLOR_CHAT_NICK),
            GUI_COLOR(GUI_COLOR_CHAT_NICK));
    }
    if (CONFIG_BOOLEAN(config_startup_display_version))
    {
        command_version_display (NULL, 0, 0, 0);
    }
    if (CONFIG_BOOLEAN(config_startup_display_logo) ||
        CONFIG_BOOLEAN(config_startup_display_version))
    {
        gui_chat_printf (
            NULL,
            "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
    }

    if (weechat_first_start)
    {
        /* message on first run (when weechat.conf is created) */
        gui_chat_printf (NULL, "");
        gui_chat_printf (
            NULL,
            _("Welcome to WhoreIRC!\n"
              "\n"
              "If you are discovering WhoreIRC, it is recommended to read at "
              "least the quickstart guide, and the user's guide if you have "
              "some time; they explain main WhoreIRC concepts.\n"
              "All WhoreIRC docs are available at: https://weechat.org/doc\n"
              "\n"
              "Moreover, there is inline help with /help on all commands and "
              "options (use Tab key to complete the name).\n"
              "The command /fset can help to customize WhoreIRC.\n"
              "\n"
              "You can add and connect to an IRC server with /server and "
              "/connect commands (see /help server)."));
        gui_chat_printf (NULL, "");
        gui_chat_printf (NULL, "---");
        gui_chat_printf (NULL, "");
    }
}

/*
 * Displays warnings about $TERM if it is detected as wrong.
 *
 * If $TERM is different from "screen" or "screen-256color" and that $STY is
 * set (GNU screen) or $TMUX is set (tmux), then a warning is displayed.
 */

void
weechat_term_check ()
{
    char *term, *sty, *tmux;
    const char *screen_terms = "screen-256color, screen";
    const char *tmux_terms = "tmux-256color, tmux, screen-256color, screen";
    const char *ptr_terms;
    int is_term_ok, is_screen, is_tmux;

    term = getenv ("TERM");
    sty = getenv ("STY");
    tmux = getenv ("TMUX");

    is_screen = (sty && sty[0]);
    is_tmux = (tmux && tmux[0]);

    if (is_screen || is_tmux)
    {
        /* check if $TERM is OK (according to screen/tmux) */
        is_term_ok = 0;
        ptr_terms = NULL;
        if (is_screen)
        {
            is_term_ok = (term && (strncmp (term, "screen", 6) == 0));
            ptr_terms = screen_terms;
        }
        else if (is_tmux)
        {
            is_term_ok = (term
                          && ((strncmp (term, "screen", 6) == 0)
                              || (strncmp (term, "tmux", 4) == 0)));
            ptr_terms = tmux_terms;
        }

        /* display a warning if $TERM is NOT OK */
        if (!is_term_ok)
        {
            gui_chat_printf_date_tags (
                NULL,
                0,
                "term_warning",
                /* TRANSLATORS: the "under %s" can be "under screen" or "under tmux" */
                _("%sWarning: WhoreIRC is running under %s and $TERM is \"%s\", "
                  "which can cause display bugs; $TERM should be set to one "
                  "of these values: %s"),
                gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                (is_screen) ? "screen" : "tmux",
                (term) ? term : "",
                ptr_terms);
            gui_chat_printf_date_tags (
                NULL,
                0,
                "term_warning",
                _("%sYou should add this line in the file %s:  %s"),
                gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                (is_screen) ? "~/.screenrc" : "~/.tmux.conf",
                (is_screen) ?
                "term screen-256color" :
                "set -g default-terminal \"tmux-256color\"");
        }
    }
}

/*
 * Displays warning about wrong locale ($LANG and $LC_*) if they are detected
 * as wrong.
 */

void
weechat_locale_check ()
{
    if (!weechat_locale_ok)
    {
        gui_chat_printf (
            NULL,
            _("%sWarning: cannot set the locale; make sure $LANG and $LC_* "
              "variables are correct"),
            gui_chat_prefix[GUI_CHAT_PREFIX_ERROR]);
    }
}

/*
 * Callback for system signal SIGHUP: quits WhoreIRC.
 */

void
weechat_sighup ()
{
    weechat_quit_signal = SIGHUP;
}

/*
 * Callback for system signal SIGQUIT: quits WhoreIRC.
 */

void
weechat_sigquit ()
{
    weechat_quit_signal = SIGQUIT;
}

/*
 * Callback for system signal SIGTERM: quits WhoreIRC.
 */

void
weechat_sigterm ()
{
    weechat_quit_signal = SIGTERM;
}

/*
 * Shutdowns WhoreIRC.
 */

void
weechat_shutdown (int return_code, int crash)
{
    gui_chat_print_lines_waiting_buffer (stderr);

    log_close ();
    network_end ();
    debug_end ();

    if (!crash && weechat_home_delete_on_exit)
    {
        /* remove temporary home (only if not crashing) */
        util_rmtree (weechat_home);
    }

    if (weechat_argv0)
        free (weechat_argv0);
    if (weechat_home)
        free (weechat_home);
    if (weechat_local_charset)
        free (weechat_local_charset);
    if (weechat_force_plugin_autoload)
        free (weechat_force_plugin_autoload);
    if (weechat_startup_commands)
        weelist_free (weechat_startup_commands);

    if (crash)
        abort ();
    else if (return_code >= 0)
        exit (return_code);
}

/*
 * Initializes gettext.
 */

void
weechat_init_gettext ()
{
    weechat_locale_ok = (setlocale (LC_ALL, "") != NULL);   /* init gettext */
#ifdef ENABLE_NLS
    bindtextdomain (PACKAGE, LOCALEDIR);
    bind_textdomain_codeset (PACKAGE, "UTF-8");
    textdomain (PACKAGE);
#endif /* ENABLE_NLS */

#ifdef HAVE_LANGINFO_CODESET
    weechat_local_charset = strdup (nl_langinfo (CODESET));
#else
    weechat_local_charset = strdup ("");
#endif /* HAVE_LANGINFO_CODESET */
    utf8_init ();
}

/*
 * Initializes WhoreIRC.
 */

void
weechat_init (int argc, char *argv[], void (*gui_init_cb)())
{
    weechat_first_start_time = time (NULL); /* initialize start time        */
    gettimeofday (&weechat_current_start_timeval, NULL);

    /* catch signals */
    util_catch_signal (SIGINT, SIG_IGN);           /* signal ignored        */
    util_catch_signal (SIGPIPE, SIG_IGN);          /* signal ignored        */
    util_catch_signal (SIGSEGV, &debug_sigsegv);   /* crash dump            */
    util_catch_signal (SIGHUP, &weechat_sighup);   /* exit WhoreIRC          */
    util_catch_signal (SIGQUIT, &weechat_sigquit); /* exit WhoreIRC          */
    util_catch_signal (SIGTERM, &weechat_sigterm); /* exit WhoreIRC          */

    hdata_init ();                      /* initialize hdata                 */
    hook_init ();                       /* initialize hooks                 */
    debug_init ();                      /* hook signals for debug           */
    gui_color_init ();                  /* initialize colors                */
    gui_chat_init ();                   /* initialize chat                  */
    command_init ();                    /* initialize WhoreIRC commands      */
    completion_init ();                 /* add core completion hooks        */
    gui_key_init ();                    /* init keys                        */
    network_init_gcrypt ();             /* init gcrypt                      */
    if (!secure_init ())                /* init secured data                */
        weechat_shutdown (EXIT_FAILURE, 0);
    if (!secure_config_init ())         /* init secured data options (sec.*)*/
        weechat_shutdown (EXIT_FAILURE, 0);
    if (!config_weechat_init ())        /* init WhoreIRC options (weechat.*) */
        weechat_shutdown (EXIT_FAILURE, 0);
    weechat_parse_args (argc, argv);    /* parse command line args          */
    weechat_create_home_dir ();         /* create WhoreIRC home directory    */
    log_init ();                        /* init log file                    */
    plugin_api_init ();                 /* create some hooks (info,hdata,..)*/
    secure_config_read ();              /* read secured data options        */
    config_weechat_read ();             /* read WhoreIRC options             */
    network_init_gnutls ();             /* init GnuTLS                      */

    if (gui_init_cb)
        (*gui_init_cb) ();              /* init WhoreIRC interface           */

    if (weechat_upgrading)
    {
        upgrade_weechat_load ();        /* upgrade with session file        */
        weechat_upgrade_count++;        /* increase /upgrade count          */
    }
    weechat_startup_message ();         /* display WhoreIRC startup message  */
    gui_chat_print_lines_waiting_buffer (NULL); /* display lines waiting    */
    weechat_term_check ();              /* warning about wrong $TERM        */
    weechat_locale_check ();            /* warning about wrong locale       */
    command_startup (0);                /* command executed before plugins  */
    plugin_init (weechat_force_plugin_autoload, /* init plugin interface(s) */
                 argc, argv);
    command_startup (1);                /* commands executed after plugins  */
    if (!weechat_upgrading)
        gui_layout_window_apply (gui_layout_current, -1);
    if (weechat_upgrading)
        upgrade_weechat_end ();         /* remove .upgrade files + signal   */
}

/*
 * Ends WhoreIRC.
 */

void
weechat_end (void (*gui_end_cb)(int clean_exit))
{
    gui_layout_store_on_exit ();        /* store layout                     */
    plugin_end ();                      /* end plugin interface(s)          */
    if (CONFIG_BOOLEAN(config_look_save_config_on_exit))
        (void) config_weechat_write (); /* save WhoreIRC config file         */
    (void) secure_config_write ();      /* save secured data                */

    if (gui_end_cb)
        (*gui_end_cb) (1);              /* shut down WhoreIRC GUI            */

    proxy_free_all ();                  /* free all proxies                 */
    config_weechat_free ();             /* free WhoreIRC options             */
    secure_config_free ();              /* free secured data options        */
    config_file_free_all ();            /* free all configuration files     */
    gui_key_end ();                     /* remove all keys                  */
    unhook_all ();                      /* remove all hooks                 */
    hdata_end ();                       /* end hdata                        */
    secure_end ();                      /* end secured data                 */
    string_end ();                      /* end string                       */
    weechat_shutdown (-1, 0);           /* end other things                 */
}
