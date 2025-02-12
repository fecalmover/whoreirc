/*
 * wee-util.c - some useful functions
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

/* for P_tmpdir in stdio.h */
#ifndef __USE_XOPEN
#define __USE_XOPEN
#endif

/* for nftw() */
#define _XOPEN_SOURCE 700

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>
#include <ftw.h>

#include "weechat.h"
#include "wee-util.h"
#include "wee-config.h"
#include "wee-log.h"
#include "wee-string.h"
#include "wee-utf8.h"
#include "../gui/gui-chat.h"
#include "../gui/gui-window.h"
#include "../plugins/plugin.h"


#ifdef HAVE_SYS_RESOURCE_H
struct t_rlimit_resource rlimit_resource[] =
{
#ifdef RLIMIT_AS
    { "as", RLIMIT_AS },
#endif
#ifdef RLIMIT_CORE
    { "core", RLIMIT_CORE },
#endif
#ifdef RLIMIT_CPU
    { "cpu", RLIMIT_CPU },
#endif
#ifdef RLIMIT_DATA
    { "data", RLIMIT_DATA },
#endif
#ifdef RLIMIT_FSIZE
    { "fsize", RLIMIT_FSIZE },
#endif
#ifdef RLIMIT_LOCKS
    { "locks", RLIMIT_LOCKS },
#endif
#ifdef RLIMIT_MEMLOCK
    { "memlock", RLIMIT_MEMLOCK },
#endif
#ifdef RLIMIT_MSGQUEUE
    { "msgqueue", RLIMIT_MSGQUEUE },
#endif
#ifdef RLIMIT_NICE
    { "nice", RLIMIT_NICE },
#endif
#ifdef RLIMIT_NOFILE
    { "nofile", RLIMIT_NOFILE },
#endif
#ifdef RLIMIT_NPROC
    { "nproc", RLIMIT_NPROC },
#endif
#ifdef RLIMIT_RSS
    { "rss", RLIMIT_RSS },
#endif
#ifdef RLIMIT_RTPRIO
    { "rtprio", RLIMIT_RTPRIO },
#endif
#ifdef RLIMIT_RTTIME
    { "rttime", RLIMIT_RTTIME },
#endif
#ifdef RLIMIT_SIGPENDING
    { "sigpending", RLIMIT_SIGPENDING },
#endif
#ifdef RLIMIT_STACK
    { "stack", RLIMIT_STACK },
#endif
    { NULL, 0 },
};
#endif /* HAVE_SYS_RESOURCE_H */

struct t_util_signal util_signals[] =
{ { "hup", SIGHUP },
  { "int", SIGINT },
  { "quit", SIGQUIT },
  { "kill", SIGKILL },
  { "term", SIGTERM },
  { "usr1", SIGUSR1 },
  { "usr2", SIGUSR2 },
  { NULL, 0 },
};

/*
 * Sets resource limit.
 */

#ifdef HAVE_SYS_RESOURCE_H
void
util_setrlimit_resource (const char *resource_name, long limit)
{
    int i;
    struct rlimit rlim;
    char str_limit[64];

    if (!resource_name)
        return;

    if (limit == -1)
        snprintf (str_limit, sizeof (str_limit), "unlimited");
    else
        snprintf (str_limit, sizeof (str_limit), "%ld", limit);

    for (i = 0; rlimit_resource[i].name; i++)
    {
        if (strcmp (rlimit_resource[i].name, resource_name) == 0)
        {
            if (limit < -1)
            {
                gui_chat_printf (NULL,
                                 _("%sError: invalid limit for resource \"%s\": "
                                   "%s (must be >= -1)"),
                                 gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                                 resource_name, str_limit);
                return;
            }
            rlim.rlim_cur = (limit >= 0) ? (rlim_t)limit : RLIM_INFINITY;
            rlim.rlim_max = rlim.rlim_cur;
            if (setrlimit (rlimit_resource[i].resource, &rlim) == 0)
            {
                log_printf (_("Limit for resource \"%s\" has been set to %s"),
                            resource_name, str_limit);
                if (gui_init_ok)
                {
                    gui_chat_printf (NULL,
                                     _("Limit for resource \"%s\" has been set "
                                       "to %s"),
                                     resource_name, str_limit);
                }
            }
            else
            {
                gui_chat_printf (NULL,
                                 _("%sError: unable to set resource limit "
                                   "\"%s\" to %s: error %d %s"),
                                 gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                                 resource_name, str_limit,
                                 errno, strerror (errno));
            }
            return;
        }
    }

    gui_chat_printf (NULL,
                     _("%sError: unknown resource limit \"%s\" (see /help "
                       "weechat.startup.sys_rlimit)"),
                     gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                     resource_name);
}
#endif /* HAVE_SYS_RESOURCE_H */

/*
 * Sets resource limits using value of option "weechat.startup.sys_rlimit".
 */

void
util_setrlimit ()
{
#ifdef HAVE_SYS_RESOURCE_H
    char **items, *pos, *error;
    int num_items, i;
    long number;

    items = string_split (CONFIG_STRING(config_startup_sys_rlimit), ",", NULL,
                          WHOREIRC_STRING_SPLIT_STRIP_LEFT
                          | WHOREIRC_STRING_SPLIT_STRIP_RIGHT
                          | WHOREIRC_STRING_SPLIT_COLLAPSE_SEPS,
                          0, &num_items);
    if (items)
    {
        for (i = 0; i < num_items; i++)
        {
            pos = strchr (items[i], ':');
            if (pos)
            {
                pos[0] = '\0';
                error = NULL;
                number = strtol (pos + 1, &error, 10);
                if (error && !error[0])
                {
                    util_setrlimit_resource (items[i], number);
                }
                else
                {
                    gui_chat_printf (NULL,
                                     _("%sError: invalid limit for resource "
                                       "\"%s\": %s (must be >= -1)"),
                                     gui_chat_prefix[GUI_CHAT_PREFIX_ERROR],
                                     items[i], pos + 1);
                }
            }
        }
        string_free_split (items);
    }
#endif /* HAVE_SYS_RESOURCE_H */
}

/*
 * Compares two timeval structures.
 *
 * Returns:
 *   -1: tv1 < tv2
 *    0: tv1 == tv2
 *    1: tv1 > tv2
 */

int
util_timeval_cmp (struct timeval *tv1, struct timeval *tv2)
{
    if (!tv1 || !tv2)
        return (tv1) ? 1 : ((tv2) ? -1 : 0);

    if (tv1->tv_sec < tv2->tv_sec)
        return -1;
    if (tv1->tv_sec > tv2->tv_sec)
        return 1;
    if (tv1->tv_usec < tv2->tv_usec)
        return -1;
    if (tv1->tv_usec > tv2->tv_usec)
        return 1;
    return 0;
}

/*
 * Calculates difference between two timeval structures.
 *
 * Returns difference in microseconds.
 */

long long
util_timeval_diff (struct timeval *tv1, struct timeval *tv2)
{
    long long diff_sec, diff_usec;

    if (!tv1 || !tv2)
        return 0;

    diff_sec = tv2->tv_sec - tv1->tv_sec;
    diff_usec = tv2->tv_usec - tv1->tv_usec;

    if (diff_usec < 0)
    {
        diff_usec += 1000000;
        diff_sec--;
    }

    return (diff_sec * 1000000) + diff_usec;
}

/*
 * Adds interval (in microseconds) to a timeval structure.
 */

void
util_timeval_add (struct timeval *tv, long long interval)
{
    long long usec;

    if (!tv)
        return;

    tv->tv_sec += (interval / 1000000);
    usec = tv->tv_usec + (interval % 1000000);
    if (usec > 1000000)
    {
        tv->tv_usec = usec % 1000000;
        tv->tv_sec++;
    }
    else
        tv->tv_usec = usec;
}

/*
 * Converts date to a string, using format of option "weechat.look.time_format"
 * (can be localized).
 */

const char *
util_get_time_string (const time_t *date)
{
    struct tm *local_time;
    static char text_time[128];

    text_time[0] = '\0';
    local_time = localtime (date);
    if (local_time)
    {
        if (strftime (text_time, sizeof (text_time),
                      CONFIG_STRING(config_look_time_format), local_time) == 0)
            text_time[0] = '\0';
    }

    return text_time;
}

/*
 * Parses a string with a delay and optional unit, returns the delay in
 * milliseconds.
 *
 * The delay is a number followed by a unit which can be:
 *   - "ms": milliseconds
 *   - "s": seconds
 *   - "m": minutes
 *   - "h": hours
 *
 * The default factor sets the default unit:
 *   - 1: milliseconds
 *   - 1000: seconds
 *   - 60000: minutes
 *   - 3600000: hours
 *
 * Returns the delay in milliseconds, -1 if error.
 */

long
util_parse_delay (const char *string_delay, long default_factor)
{
    const char *pos;
    char *str_number, *error;
    long factor, delay;

    if (!string_delay || !string_delay[0] || (default_factor < 1))
        return -1;

    factor = default_factor;

    pos = string_delay;
    while (pos[0] && isdigit ((unsigned char)pos[0]))
    {
        pos++;
    }

    if ((pos > string_delay) && pos[0])
    {
        str_number = string_strndup (string_delay, pos - string_delay);
        if (strcmp (pos, "ms") == 0)
            factor = 1;
        else if (strcmp (pos, "s") == 0)
            factor = 1000;
        else if (strcmp (pos, "m") == 0)
            factor = 1000 * 60;
        else if (strcmp (pos, "h") == 0)
            factor = 1000 * 60 * 60;
        else
            return -1;
    }
    else
    {
        str_number = strdup (string_delay);
    }

    if (!str_number)
        return -1;

    error = NULL;
    delay = strtol (str_number, &error, 10);
    if (!error || error[0] || (delay < 0))
    {
        free (str_number);
        return -1;
    }

    free (str_number);

    return delay * factor;
}

/*
 * Gets a signal number with a name; only some commonly used signal names are
 * supported here (see declaration of util_signals[]).
 *
 * Returns the signal number, -1 if not found.
 */

int
util_signal_search (const char *name)
{
    int i;

    if (!name)
        return -1;

    for (i = 0; util_signals[i].name; i++)
    {
        if (string_strcasecmp (util_signals[i].name, name) == 0)
            return util_signals[i].signal;
    }

    /* signal not found */
    return -1;
}

/*
 * Catches a system signal.
 */

void
util_catch_signal (int signum, void (*handler)(int))
{
    struct sigaction act;

    sigemptyset (&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handler;
    sigaction (signum, &act, NULL);
}

/*
 * Returns the path to a temporary directory, the first valid directory in
 * this list:
 *   - content of environment variable "TMPDIR"
 *   - P_tmpdir (from stdio.h)
 *   - content of environment variable "HOME" (user home directory)
 *   - "." (current directory)
 */

char *
util_get_temp_dir()
{
    char *tmpdir;
    struct stat buf;
    int rc;

    /* get directory from $TMPDIR */
    tmpdir = getenv ("TMPDIR");
    if (tmpdir && tmpdir[0])
    {
        rc = stat (tmpdir, &buf);
        if ((rc == 0) && S_ISDIR(buf.st_mode))
            return strdup (tmpdir);
    }

    /* get directory from P_tmpdir */
    rc = stat (P_tmpdir, &buf);
    if ((rc == 0) && S_ISDIR(buf.st_mode))
        return strdup (P_tmpdir);

    /* get directory from $HOME */
    tmpdir = getenv ("HOME");
    if (tmpdir && tmpdir[0])
    {
        rc = stat (tmpdir, &buf);
        if ((rc == 0) && S_ISDIR(buf.st_mode))
            return strdup (tmpdir);
    }

    /* fallback on current directory */
    return strdup (".");
}

/*
 * Creates a directory in WhoreIRC home.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
util_mkdir_home (const char *directory, int mode)
{
    char *dir_name;
    int dir_length;

    if (!directory)
        return 0;

    /* build directory, adding WhoreIRC home */
    dir_length = strlen (weechat_home) + strlen (directory) + 2;
    dir_name = malloc (dir_length);
    if (!dir_name)
        return 0;

    snprintf (dir_name, dir_length, "%s/%s", weechat_home, directory);

    if (mkdir (dir_name, mode) < 0)
    {
        if (errno != EEXIST)
        {
            free (dir_name);
            return 0;
        }
    }

    free (dir_name);
    return 1;
}

/*
 * Creates a directory.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
util_mkdir (const char *directory, int mode)
{
    if (!directory)
        return 0;

    if (mkdir (directory, mode) < 0)
    {
        if (errno != EEXIST)
            return 0;
    }

    return 1;
}

/*
 * Creates a directory and makes parent directories as needed.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
util_mkdir_parents (const char *directory, int mode)
{
    char *string, *ptr_string, *pos_sep;
    struct stat buf;
    int rc;

    if (!directory)
        return 0;

    string = strdup (directory);
    if (!string)
        return 0;

    ptr_string = string;
    while (ptr_string[0] == DIR_SEPARATOR_CHAR)
    {
        ptr_string++;
    }

    while (ptr_string && ptr_string[0])
    {
        pos_sep = strchr (ptr_string, DIR_SEPARATOR_CHAR);
        if (pos_sep)
            pos_sep[0] = '\0';

        rc = stat (string, &buf);
        if ((rc < 0) || !S_ISDIR(buf.st_mode))
        {
            /* try to create directory */
            if (!util_mkdir (string, mode))
            {
                free (string);
                return 0;
            }
        }

        if (pos_sep)
        {
            pos_sep[0] = DIR_SEPARATOR_CHAR;
            ptr_string = pos_sep + 1;
        }
        else
            ptr_string = NULL;
    }

    free (string);

    return 1;
}

/*
 * Unlinks a file or directory; callback called by function util_rmtree().
 *
 * Returns the return code of remove():
 *   0: OK
 *   -1: error
 */

int
util_unlink_cb (const char *fpath, const struct stat *sb, int typeflag,
                struct FTW *ftwbuf)
{
    /* make C compiler happy */
    (void) sb;
    (void) typeflag;
    (void) ftwbuf;

    return remove (fpath);
}

/*
 * Removes a directory and all files inside recursively.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
util_rmtree (const char *directory)
{
    int rc;

    if (!directory)
        return 0;

    rc = nftw (directory, util_unlink_cb, 64, FTW_DEPTH | FTW_PHYS);

    return (rc == 0) ? 1 : 0;
}

/*
 * Finds files in a directory and executes a function on each file.
 */

void
util_exec_on_files (const char *directory, int recurse_subdirs,
                    int hidden_files,
                    void (*callback)(void *data, const char *filename),
                    void *callback_data)
{
    char complete_filename[PATH_MAX];
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if (!directory || !callback)
        return;

    dir = opendir (directory);
    if (dir)
    {
        while ((entry = readdir (dir)))
        {
            if (hidden_files || (entry->d_name[0] != '.'))
            {
                snprintf (complete_filename, sizeof (complete_filename),
                          "%s/%s", directory, entry->d_name);
                lstat (complete_filename, &statbuf);
                if (S_ISDIR(statbuf.st_mode))
                {
                    if (recurse_subdirs
                        && (strcmp (entry->d_name, ".") != 0)
                        && (strcmp (entry->d_name, "..") != 0))
                    {
                        util_exec_on_files (complete_filename, 1, hidden_files,
                                            callback, callback_data);
                    }
                }
                else
                {
                    (*callback) (callback_data, complete_filename);
                }
            }
        }
        closedir (dir);
    }
}

/*
 * Searches for the full name of a WhoreIRC library with name and extension
 * (searches first in WhoreIRC user's dir, then WhoreIRC global lib directory).
 *
 * Returns name of library found, NULL if not found.
 *
 * Note: result must be freed after use (if not NULL).
 */

char *
util_search_full_lib_name_ext (const char *filename, const char *extension,
                               const char *plugins_dir)
{
    char *name_with_ext, *final_name, *extra_libdir;
    int length;
    struct stat st;

    length = strlen (filename) + strlen (extension) + 1;
    name_with_ext = malloc (length);
    if (!name_with_ext)
        return NULL;
    snprintf (name_with_ext, length,
              "%s%s",
              filename,
              (strchr (filename, '.')) ? "" : extension);

    /* try libdir from environment variable WHOREIRC_EXTRA_LIBDIR */
    extra_libdir = getenv (WHOREIRC_EXTRA_LIBDIR);
    if (extra_libdir && extra_libdir[0])
    {
        length = strlen (extra_libdir) + strlen (name_with_ext) +
            strlen (plugins_dir) + 16;
        final_name = malloc (length);
        if (!final_name)
        {
            free (name_with_ext);
            return NULL;
        }
        snprintf (final_name, length,
                  "%s%s%s%s%s",
                  extra_libdir,
                  DIR_SEPARATOR,
                  plugins_dir,
                  DIR_SEPARATOR,
                  name_with_ext);
        if ((stat (final_name, &st) == 0) && (st.st_size > 0))
        {
            free (name_with_ext);
            return final_name;
        }
        free (final_name);
    }

    /* try WhoreIRC user's dir */
    length = strlen (weechat_home) + strlen (name_with_ext) +
        strlen (plugins_dir) + 16;
    final_name = malloc (length);
    if (!final_name)
    {
        free (name_with_ext);
        return NULL;
    }
    snprintf (final_name, length,
              "%s%s%s%s%s",
              weechat_home,
              DIR_SEPARATOR,
              plugins_dir,
              DIR_SEPARATOR,
              name_with_ext);
    if ((stat (final_name, &st) == 0) && (st.st_size > 0))
    {
        free (name_with_ext);
        return final_name;
    }
    free (final_name);

    /* try WhoreIRC global lib dir */
    length = strlen (WHOREIRC_LIBDIR) + strlen (name_with_ext) +
        strlen (plugins_dir) + 16;
    final_name = malloc (length);
    if (!final_name)
    {
        free (name_with_ext);
        return NULL;
    }
    snprintf (final_name, length,
              "%s%s%s%s%s",
              WHOREIRC_LIBDIR,
              DIR_SEPARATOR,
              plugins_dir,
              DIR_SEPARATOR,
              name_with_ext);
    if ((stat (final_name, &st) == 0) && (st.st_size > 0))
    {
        free (name_with_ext);
        return final_name;
    }
    free (final_name);

    free (name_with_ext);

    return NULL;
}

/*
 * Searches for the full name of a WhoreIRC library with name.
 *
 * All extensions listed in option "weechat.plugin.extension" are tested.
 *
 * Note: result must be freed after use (if not NULL).
 */

char *
util_search_full_lib_name (const char *filename, const char *plugins_dir)
{
    char *filename2, *full_name;
    int i;

    /* expand home in filename */
    filename2 = string_expand_home (filename);
    if (!filename2)
        return NULL;

    /* if full path, return it */
    if (strchr (filename2, '/') || strchr (filename2, '\\'))
        return filename2;

    if (config_plugin_extensions)
    {
        for (i = 0; i < config_num_plugin_extensions; i++)
        {
            full_name = util_search_full_lib_name_ext (filename2,
                                                       config_plugin_extensions[i],
                                                       plugins_dir);
            if (full_name)
            {
                free (filename2);
                return full_name;
            }
        }
    }
    else
    {
        full_name = util_search_full_lib_name_ext (filename2, "", plugins_dir);
        if (full_name)
        {
            free (filename2);
            return full_name;
        }
    }

    free (filename2);

    return strdup (filename);
}

/*
 * Reads content of a file.
 *
 * Returns an allocated buffer with the content of file, NULL if error.
 *
 * Note: result must be freed after use.
 */

char *
util_file_get_content (const char *filename)
{
    char *buffer, *buffer2;
    FILE *f;
    size_t count, fp;

    if (!filename)
        return NULL;

    buffer = NULL;
    fp = 0;

    f = fopen (filename, "r");
    if (!f)
        goto error;

    while (!feof (f))
    {
        if (fp > SIZE_MAX - (1024 * sizeof (char)))
            goto error;
        buffer2 = (char *) realloc (buffer, (fp + (1024 * sizeof (char))));
        if (!buffer2)
            goto error;
        buffer = buffer2;
        count = fread (&buffer[fp], sizeof (char), 1024, f);
        if (count <= 0)
            goto error;
        fp += count;
    }
    if (fp > SIZE_MAX - sizeof (char))
        goto error;
    buffer2 = (char *) realloc (buffer, fp + sizeof (char));
    if (!buffer2)
        goto error;
    buffer = buffer2;
    buffer[fp] = '\0';
    fclose (f);

    return buffer;

error:
    if (buffer)
	free (buffer);
    if (f)
	fclose (f);
    return NULL;
}

/*
 * Gets version number (integer) with a version as string.
 *
 * Non-digit chars like "-dev" are ignored.
 *
 * Examples:
 *   "0.3.2-dev" ==> 197120 (== 0x00030200)
 *   "0.3.2-rc1" ==> 197120 (== 0x00030200)
 *   "0.3.2"     ==> 197120 (== 0x00030200)
 *   "0.3.1.1"   ==> 196865 (== 0x00030101)
 *   "0.3.1"     ==> 196864 (== 0x00030100)
 *   "0.3.0"     ==> 196608 (== 0x00030000)
 */

int
util_version_number (const char *version)
{
    char **items, buf[64], *error;
    const char *ptr_item;
    int num_items, i, version_int[4], index_buf;
    long number;

    items = string_split (version, ".", NULL,
                          WHOREIRC_STRING_SPLIT_STRIP_LEFT
                          | WHOREIRC_STRING_SPLIT_STRIP_RIGHT
                          | WHOREIRC_STRING_SPLIT_COLLAPSE_SEPS,
                          4, &num_items);
    for (i = 0; i < 4; i++)
    {
        version_int[i] = 0;
        if (items && (i < num_items))
        {
            ptr_item = items[i];
            index_buf = 0;
            while (ptr_item && ptr_item[0] && (index_buf < (int)sizeof (buf) - 1))
            {
                if (ptr_item[0] == '-')
                    break;
                if (isdigit ((unsigned char)ptr_item[0]))
                {
                    buf[index_buf] = ptr_item[0];
                    index_buf++;
                }
                ptr_item = utf8_next_char (ptr_item);
            }
            buf[index_buf] = '\0';
            if (buf[0])
            {
                error = NULL;
                number = strtol (buf, &error, 10);
                if (error && !error[0])
                {
                    if (number < 0)
                        number = 0;
                    else if (number > 0xFF)
                        number = 0xFF;
                    version_int[i] = number;
                }
            }
        }
    }
    if (items)
        string_free_split (items);

    return (version_int[0] << 24) | (version_int[1] << 16)
        | (version_int[2] << 8) | version_int[3];
}

/*
 * Returns uptime as number of days, hours, minutes, seconds.
 */

void
util_get_uptime (time_t *total_seconds, int *days,
                 int *hours, int *minutes, int *seconds)
{
    time_t running_time;

    running_time = time (NULL) - weechat_first_start_time;

    if (total_seconds)
        *total_seconds = running_time;
    if (days)
        *days = running_time / (60 * 60 * 24);
    if (hours)
        *hours = (running_time % (60 * 60 * 24)) / (60 * 60);
    if (minutes)
        *minutes = ((running_time % (60 * 60 * 24)) % (60 * 60)) / 60;
    if (seconds)
        *seconds = ((running_time % (60 * 60 * 24)) % (60 * 60)) % 60;
}
