/*
 * Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
 * Copyright (C) 2006 Emmanuel Bouthenot <kolter@openics.org>
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

#ifndef WHOREIRC_H
#define WHOREIRC_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

#include <locale.h>

#if defined(ENABLE_NLS) && !defined(_)
    #ifdef HAVE_LIBINTL_H
        #include <libintl.h>
    #else
        #include "../../intl/libintl.h"
    #endif /* HAVE_LIBINTL_H */
    #define _(string) gettext(string)
    #define NG_(single,plural,number) ngettext(single,plural,number)
    #ifdef gettext_noop
        #define N_(string) gettext_noop(string)
    #else
        #define N_(string) (string)
    #endif /* gettext_noop */
#endif /* defined(ENABLE_NLS) && !defined(_) */
#if !defined(_)
    #define _(string) (string)
    #define NG_(single,plural,number) (plural)
    #define N_(string) (string)
#endif /* !defined(_) */


#define WHOREIRC_COPYRIGHT_DATE   "(C) 2003-2019"
#define WHOREIRC_WEBSITE          "https://weechat.org/"
#define WHOREIRC_WEBSITE_DOWNLOAD "https://weechat.org/download"

/* log file */
#define WHOREIRC_LOG_NAME "weechat.log"

/* license */
#define WHOREIRC_LICENSE_TEXT \
    "WhoreIRC is free software; you can redistribute it and/or modify\n" \
    "it under the terms of the GNU General Public License as published by\n" \
    "the Free Software Foundation; either version 3 of the License, or\n" \
    "(at your option) any later version.\n" \
    "\n", \
    \
    "WhoreIRC is distributed in the hope that it will be useful,\n" \
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" \
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" \
    "GNU General Public License for more details.\n" \
    "\n" \
    "You should have received a copy of the GNU General Public License\n" \
    "along with WhoreIRC.  If not, see <https://www.gnu.org/licenses/>.\n\n"

/* directory separator, depending on OS */
#ifdef _WIN32
    #define DIR_SEPARATOR       "\\"
    #define DIR_SEPARATOR_CHAR  '\\'
#else
    #define DIR_SEPARATOR       "/"
    #define DIR_SEPARATOR_CHAR  '/'
#endif /* _WIN32 */

/* some systems like GNU/Hurd do not define PATH_MAX */
#ifndef PATH_MAX
    #define PATH_MAX 4096
#endif /* PATH_MAX */

/* internal charset */
#define WHOREIRC_INTERNAL_CHARSET "UTF-8"

/* name of environment variable with an extra lib dir */
#define WHOREIRC_EXTRA_LIBDIR "WHOREIRC_EXTRA_LIBDIR"

struct t_weelist;

/* global variables and functions */
extern int weechat_headless;
extern int weechat_debug_core;
extern char *weechat_argv0;
extern int weechat_upgrading;
extern int weechat_first_start;
extern time_t weechat_first_start_time;
extern struct timeval weechat_current_start_timeval;
extern int weechat_upgrade_count;
extern volatile sig_atomic_t weechat_quit;
extern volatile sig_atomic_t weechat_quit_signal;
extern char *weechat_home;
extern int weechat_home_temp;
extern char *weechat_local_charset;
extern int weechat_plugin_no_dlclose;
extern int weechat_no_gnutls;
extern int weechat_no_gcrypt;
extern struct t_weelist *weechat_startup_commands;

extern void weechat_term_check ();
extern void weechat_shutdown (int return_code, int crash);
extern void weechat_init_gettext ();
extern void weechat_init (int argc, char *argv[], void (*gui_init_cb)());
extern void weechat_end (void (*gui_end_cb)(int clean_exit));

#endif /* WHOREIRC_H */
