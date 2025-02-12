/*
 * logger.c - logger plugin for WhoreIRC: save buffer lines to disk files
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

/* this define is needed for strptime() (not on OpenBSD/Sun) */
#if !defined(__OpenBSD__) && !defined(__sun)
#define _XOPEN_SOURCE 700
#endif

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <time.h>

#include "../weechat-plugin.h"
#include "logger.h"
#include "logger-buffer.h"
#include "logger-command.h"
#include "logger-config.h"
#include "logger-info.h"
#include "logger-tail.h"


WHOREIRC_PLUGIN_NAME(LOGGER_PLUGIN_NAME);
WHOREIRC_PLUGIN_DESCRIPTION(N_("Log buffers to files"));
WHOREIRC_PLUGIN_AUTHOR("Sébastien Helleu <flashcode@flashtux.org>");
WHOREIRC_PLUGIN_VERSION(WHOREIRC_VERSION);
WHOREIRC_PLUGIN_LICENSE(WHOREIRC_LICENSE);
WHOREIRC_PLUGIN_PRIORITY(14000);

struct t_weechat_plugin *weechat_logger_plugin = NULL;

struct t_hook *logger_timer = NULL;    /* timer to flush log files          */


/*
 * Gets logger file path option.
 *
 * Special vars are replaced:
 *   - "%h" (at beginning of string): WhoreIRC home
 *   - "~": user home
 *   - date/time specifiers (see man strftime)
 *
 * Note: result must be freed after use.
 */

char *
logger_get_file_path ()
{
    char *path, *path2;
    int length;
    time_t seconds;
    struct tm *date_tmp;

    path = NULL;
    path2 = NULL;

    /* replace %h and "~", evaluate path */
    path = weechat_string_eval_path_home (
        weechat_config_string (logger_config_file_path), NULL, NULL, NULL);
    if (!path)
        goto end;

    /* replace date/time specifiers in path */
    length = strlen (path) + 256 + 1;
    path2 = malloc (length);
    if (!path2)
        goto end;
    seconds = time (NULL);
    date_tmp = localtime (&seconds);
    path2[0] = '\0';
    if (strftime (path2, length - 1, path, date_tmp) == 0)
        path2[0] = '\0';

    if (weechat_logger_plugin->debug)
    {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  "%s: file path = \"%s\"",
                                  LOGGER_PLUGIN_NAME, path2);
    }

end:
    if (path)
        free (path);
    return path2;
}

/*
 * Creates logger directory.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
logger_create_directory ()
{
    int rc;
    char *file_path;

    rc = 1;

    file_path = logger_get_file_path ();
    if (file_path)
    {
        if (!weechat_mkdir_parents (file_path, 0700))
            rc = 0;
        free (file_path);
    }
    else
        rc = 0;

    return rc;
}

/*
 * Builds full name of buffer.
 *
 * Note: result must be freed after use.
 */

char *
logger_build_option_name (struct t_gui_buffer *buffer)
{
    const char *plugin_name, *name;
    char *option_name;
    int length;

    if (!buffer)
        return NULL;

    plugin_name = weechat_buffer_get_string (buffer, "plugin");
    name = weechat_buffer_get_string (buffer, "name");

    length = strlen (plugin_name) + 1 + strlen (name) + 1;
    option_name = malloc (length);
    if (!option_name)
        return NULL;

    snprintf (option_name, length, "%s.%s", plugin_name, name);

    return option_name;
}

/*
 * Gets logging level for buffer.
 *
 * Returns level between 0 and 9 (0 = logging disabled).
 */

int
logger_get_level_for_buffer (struct t_gui_buffer *buffer)
{
    const char *no_log;
    char *name, *option_name, *ptr_end;
    struct t_config_option *ptr_option;

    /* no log for buffer if local variable "no_log" is defined for buffer */
    no_log = weechat_buffer_get_string (buffer, "localvar_no_log");
    if (no_log && no_log[0])
        return 0;

    name = logger_build_option_name (buffer);
    if (!name)
        return LOGGER_LEVEL_DEFAULT;

    option_name = strdup (name);
    if (option_name)
    {
        ptr_end = option_name + strlen (option_name);
        while (ptr_end >= option_name)
        {
            ptr_option = logger_config_get_level (option_name);
            if (ptr_option)
            {
                free (option_name);
                free (name);
                return weechat_config_integer (ptr_option);
            }
            ptr_end--;
            while ((ptr_end >= option_name) && (ptr_end[0] != '.'))
            {
                ptr_end--;
            }
            if ((ptr_end >= option_name) && (ptr_end[0] == '.'))
                ptr_end[0] = '\0';
        }
        ptr_option = logger_config_get_level (option_name);

        free (option_name);
        free (name);

        if (ptr_option)
            return weechat_config_integer (ptr_option);
    }
    else
        free (name);

    /* nothing found => return default level */
    return LOGGER_LEVEL_DEFAULT;
}

/*
 * Gets filename mask for a buffer.
 *
 * First tries with all arguments, then removes one by one to find mask (from
 * specific to general mask).
 */

const char *
logger_get_mask_for_buffer (struct t_gui_buffer *buffer)
{
    char *name, *option_name, *ptr_end;
    struct t_config_option *ptr_option;

    name = logger_build_option_name (buffer);
    if (!name)
        return NULL;

    option_name = strdup (name);
    if (option_name)
    {
        ptr_end = option_name + strlen (option_name);
        while (ptr_end >= option_name)
        {
            ptr_option = logger_config_get_mask (option_name);
            if (ptr_option)
            {
                free (option_name);
                free (name);
                return weechat_config_string (ptr_option);
            }
            ptr_end--;
            while ((ptr_end >= option_name) && (ptr_end[0] != '.'))
            {
                ptr_end--;
            }
            if ((ptr_end >= option_name) && (ptr_end[0] == '.'))
                ptr_end[0] = '\0';
        }
        ptr_option = logger_config_get_mask (option_name);

        free (option_name);
        free (name);

        if (ptr_option)
            return weechat_config_string (ptr_option);
    }
    else
        free (name);

    /* nothing found => return default mask (if set) */
    if (weechat_config_string (logger_config_file_mask)
        && weechat_config_string (logger_config_file_mask)[0])
        return weechat_config_string (logger_config_file_mask);

    /* no default mask set */
    return NULL;
}

/*
 * Gets expanded mask for a buffer.
 *
 * Special vars are replaced:
 *   - local variables of buffer ($plugin, $name, ..)
 *   - date/time specifiers (see man strftime)
 *
 * Note: result must be freed after use.
 */

char *
logger_get_mask_expanded (struct t_gui_buffer *buffer, const char *mask)
{
    char *mask2, *mask3, *mask4, *mask5, *mask6, *mask7, *dir_separator;
    int length;
    time_t seconds;
    struct tm *date_tmp;

    mask2 = NULL;
    mask3 = NULL;
    mask4 = NULL;
    mask5 = NULL;
    mask6 = NULL;
    mask7 = NULL;

    dir_separator = weechat_info_get ("dir_separator", "");
    if (!dir_separator)
        return NULL;

    /* replace date/time specifiers in mask */
    length = strlen (mask) + 256 + 1;
    mask2 = malloc (length);
    if (!mask2)
        goto end;
    seconds = time (NULL);
    date_tmp = localtime (&seconds);
    mask2[0] = '\0';
    if (strftime (mask2, length - 1, mask, date_tmp) == 0)
        mask2[0] = '\0';

    /*
     * we first replace directory separator (commonly '/') by \01 because
     * buffer mask can contain this char, and will be replaced by replacement
     * char ('_' by default)
     */
    mask3 = weechat_string_replace (mask2, dir_separator, "\01");
    if (!mask3)
        goto end;

    mask4 = weechat_buffer_string_replace_local_var (buffer, mask3);
    if (!mask4)
        goto end;

    mask5 = weechat_string_replace (mask4,
                                    dir_separator,
                                    weechat_config_string (logger_config_file_replacement_char));
    if (!mask5)
        goto end;

#ifdef __CYGWIN__
    mask6 = weechat_string_replace (mask5, "\\",
                                    weechat_config_string (logger_config_file_replacement_char));
#else
    mask6 = strdup (mask5);
#endif /* __CYGWIN__ */
    if (!mask6)
        goto end;

    /* restore directory separator */
    mask7 = weechat_string_replace (mask6,
                                    "\01", dir_separator);
    if (!mask7)
        goto end;

    /* convert to lower case? */
    if (weechat_config_boolean (logger_config_file_name_lower_case))
        weechat_string_tolower (mask7);

    if (weechat_logger_plugin->debug)
    {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  "%s: buffer = \"%s\", mask = \"%s\", "
                                  "decoded mask = \"%s\"",
                                  LOGGER_PLUGIN_NAME,
                                  weechat_buffer_get_string (buffer, "name"),
                                  mask, mask7);
    }

end:
    free (dir_separator);
    if (mask2)
        free (mask2);
    if (mask3)
        free (mask3);
    if (mask4)
        free (mask4);
    if (mask5)
        free (mask5);
    if (mask6)
        free (mask6);

    return mask7;
}

/*
 * Builds log filename for a buffer.
 *
 * Note: result must be freed after use.
 */

char *
logger_get_filename (struct t_gui_buffer *buffer)
{
    char *res, *mask_expanded, *file_path, *dir_separator, *weechat_dir;
    const char *mask;
    int length;

    res = NULL;
    mask_expanded = NULL;
    file_path = NULL;

    dir_separator = weechat_info_get ("dir_separator", "");
    if (!dir_separator)
        return NULL;
    weechat_dir = weechat_info_get ("weechat_dir", "");
    if (!weechat_dir)
    {
        free (dir_separator);
        return NULL;
    }

    /* get filename mask for buffer */
    mask = logger_get_mask_for_buffer (buffer);
    if (!mask)
    {
        weechat_printf_date_tags (
            NULL, 0, "no_log",
            _("%s%s: unable to find filename mask for buffer "
              "\"%s\", logging is disabled for this buffer"),
            weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
            weechat_buffer_get_string (buffer, "name"));
        free (dir_separator);
        free (weechat_dir);
        return NULL;
    }

    mask_expanded = logger_get_mask_expanded (buffer, mask);
    if (!mask_expanded)
        goto end;

    file_path = logger_get_file_path ();
    if (!file_path)
        goto end;

    /* build string with path + mask */
    length = strlen (file_path) + strlen (dir_separator) +
        strlen (mask_expanded) + 1;
    res = malloc (length);
    if (res)
    {
        snprintf (res, length, "%s%s%s",
                  file_path,
                  (file_path[strlen (file_path) - 1] == dir_separator[0]) ? "" : dir_separator,
                  mask_expanded);
    }

end:
    free (dir_separator);
    free (weechat_dir);
    if (mask_expanded)
        free (mask_expanded);
    if (file_path)
        free (file_path);

    return res;
}

/*
 * Sets log filename for a logger buffer.
 */

void
logger_set_log_filename (struct t_logger_buffer *logger_buffer)
{
    char *log_filename, *pos_last_sep;
    char *dir_separator;
    struct t_logger_buffer *ptr_logger_buffer;

    /* get log filename for buffer */
    log_filename = logger_get_filename (logger_buffer->buffer);
    if (!log_filename)
    {
        weechat_printf_date_tags (NULL, 0, "no_log",
                                  _("%s%s: not enough memory"),
                                  weechat_prefix ("error"),
                                  LOGGER_PLUGIN_NAME);
        return;
    }

    /* log file is already used by another buffer? */
    ptr_logger_buffer = logger_buffer_search_log_filename (log_filename);
    if (ptr_logger_buffer)
    {
        weechat_printf_date_tags (
            NULL, 0, "no_log",
            _("%s%s: unable to start logging for buffer "
              "\"%s\": filename \"%s\" is already used by "
              "another buffer (check your log settings)"),
            weechat_prefix ("error"),
            LOGGER_PLUGIN_NAME,
            weechat_buffer_get_string (logger_buffer->buffer, "name"),
            log_filename);
        free (log_filename);
        return;
    }

    /* create directory for path in "log_filename" */
    dir_separator = weechat_info_get ("dir_separator", "");
    if (dir_separator)
    {
        pos_last_sep = strrchr (log_filename, dir_separator[0]);
        if (pos_last_sep)
        {
            pos_last_sep[0] = '\0';
            weechat_mkdir_parents (log_filename, 0700);
            pos_last_sep[0] = dir_separator[0];
        }
        free (dir_separator);
    }

    /* set log filename */
    logger_buffer->log_filename = log_filename;
}

/*
 * Writes a line to log file.
 */

void
logger_write_line (struct t_logger_buffer *logger_buffer,
                   const char *format, ...)
{
    char *message, buf_time[256], buf_beginning[1024], *charset;
    time_t seconds;
    struct tm *date_tmp;
    int log_level;

    if (!logger_buffer->log_file)
    {
        log_level = logger_get_level_for_buffer (logger_buffer->buffer);
        if (log_level == 0)
        {
            logger_buffer_free (logger_buffer);
            return;
        }
        if (!logger_create_directory ())
        {
            weechat_printf_date_tags (
                NULL, 0, "no_log",
                _("%s%s: unable to create directory for logs "
                  "(\"%s\")"),
                weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
                weechat_config_string (logger_config_file_path));
            logger_buffer_free (logger_buffer);
            return;
        }
        if (!logger_buffer->log_filename)
            logger_set_log_filename (logger_buffer);
        if (!logger_buffer->log_filename)
        {
            logger_buffer_free (logger_buffer);
            return;
        }

        logger_buffer->log_file =
            fopen (logger_buffer->log_filename, "a");
        if (!logger_buffer->log_file)
        {
            weechat_printf_date_tags (
                NULL, 0, "no_log",
                _("%s%s: unable to write log file \"%s\": %s"),
                weechat_prefix ("error"), LOGGER_PLUGIN_NAME,
                logger_buffer->log_filename, strerror (errno));
            logger_buffer_free (logger_buffer);
            return;
        }

        if (weechat_config_boolean (logger_config_file_info_lines)
            && logger_buffer->write_start_info_line)
        {
            buf_time[0] = '\0';
            seconds = time (NULL);
            date_tmp = localtime (&seconds);
            if (date_tmp)
            {
                if (strftime (buf_time, sizeof (buf_time) - 1,
                              weechat_config_string (logger_config_file_time_format),
                              date_tmp) == 0)
                    buf_time[0] = '\0';
            }
            snprintf (buf_beginning, sizeof (buf_beginning),
                      _("%s\t****  Beginning of log  ****"),
                      buf_time);
            charset = weechat_info_get ("charset_terminal", "");
            message = (charset) ?
                weechat_iconv_from_internal (charset, buf_beginning) : NULL;
            fprintf (logger_buffer->log_file,
                     "%s\n", (message) ? message : buf_beginning);
            if (charset)
                free (charset);
            if (message)
                free (message);
            logger_buffer->flush_needed = 1;
        }
        logger_buffer->write_start_info_line = 0;
    }

    weechat_va_format (format);
    if (vbuffer)
    {
        charset = weechat_info_get ("charset_terminal", "");
        message = (charset) ?
            weechat_iconv_from_internal (charset, vbuffer) : NULL;
        fprintf (logger_buffer->log_file,
                 "%s\n", (message) ? message : vbuffer);
        if (charset)
            free (charset);
        if (message)
            free (message);
        logger_buffer->flush_needed = 1;
        if (!logger_timer)
        {
            fflush (logger_buffer->log_file);
            if (weechat_config_boolean (logger_config_file_fsync))
                fsync (fileno (logger_buffer->log_file));
            logger_buffer->flush_needed = 0;
        }
        free (vbuffer);
    }
}

/*
 * Stops log for a logger buffer.
 */

void
logger_stop (struct t_logger_buffer *logger_buffer, int write_info_line)
{
    time_t seconds;
    struct tm *date_tmp;
    char buf_time[256];

    if (!logger_buffer)
        return;

    if (logger_buffer->log_enabled && logger_buffer->log_file)
    {
        if (write_info_line && weechat_config_boolean (logger_config_file_info_lines))
        {
            buf_time[0] = '\0';
            seconds = time (NULL);
            date_tmp = localtime (&seconds);
            if (date_tmp)
            {
                if (strftime (buf_time, sizeof (buf_time) - 1,
                              weechat_config_string (logger_config_file_time_format),
                              date_tmp) == 0)
                    buf_time[0] = '\0';
            }
            logger_write_line (logger_buffer,
                               _("%s\t****  End of log  ****"),
                               buf_time);
        }
        fclose (logger_buffer->log_file);
        logger_buffer->log_file = NULL;
    }
    logger_buffer_free (logger_buffer);
}

/*
 * Ends log for all buffers.
 */

void
logger_stop_all (int write_info_line)
{
    while (logger_buffers)
    {
        logger_stop (logger_buffers, write_info_line);
    }
}

/*
 * Starts logging for a buffer.
 */

void
logger_start_buffer (struct t_gui_buffer *buffer, int write_info_line)
{
    struct t_logger_buffer *ptr_logger_buffer;
    int log_level, log_enabled;

    if (!buffer)
        return;

    log_level = logger_get_level_for_buffer (buffer);
    log_enabled =  weechat_config_boolean (logger_config_file_auto_log)
        && (log_level > 0);

    ptr_logger_buffer = logger_buffer_search_buffer (buffer);

    /* logging is disabled for buffer */
    if (!log_enabled)
    {
        /* stop logger if it is active */
        if (ptr_logger_buffer)
            logger_stop (ptr_logger_buffer, 1);
    }
    else
    {
        /* logging is enabled for buffer */
        if (ptr_logger_buffer)
            ptr_logger_buffer->log_level = log_level;
        else
        {
            ptr_logger_buffer = logger_buffer_add (buffer, log_level);

            if (ptr_logger_buffer)
            {
                if (ptr_logger_buffer->log_filename)
                {
                    if (ptr_logger_buffer->log_file)
                    {
                        fclose (ptr_logger_buffer->log_file);
                        ptr_logger_buffer->log_file = NULL;
                    }
                }
            }
        }
        if (ptr_logger_buffer)
            ptr_logger_buffer->write_start_info_line = write_info_line;
    }
}

/*
 * Starts logging for all buffers.
 */

void
logger_start_buffer_all (int write_info_line)
{
    struct t_infolist *ptr_infolist;

    ptr_infolist = weechat_infolist_get ("buffer", NULL, NULL);
    if (ptr_infolist)
    {
        while (weechat_infolist_next (ptr_infolist))
        {
            logger_start_buffer (weechat_infolist_pointer (ptr_infolist,
                                                           "pointer"),
                                 write_info_line);
        }
        weechat_infolist_free (ptr_infolist);
    }
}

/*
 * Flushes all log files.
 */

void
logger_flush ()
{
    struct t_logger_buffer *ptr_logger_buffer;

    for (ptr_logger_buffer = logger_buffers; ptr_logger_buffer;
         ptr_logger_buffer = ptr_logger_buffer->next_buffer)
    {
        if (ptr_logger_buffer->log_file && ptr_logger_buffer->flush_needed)
        {
            if (weechat_logger_plugin->debug >= 2)
            {
                weechat_printf_date_tags (NULL, 0, "no_log",
                                          "%s: flush file %s",
                                          LOGGER_PLUGIN_NAME,
                                          ptr_logger_buffer->log_filename);
            }
            fflush (ptr_logger_buffer->log_file);
            if (weechat_config_boolean (logger_config_file_fsync))
                fsync (fileno (ptr_logger_buffer->log_file));
            ptr_logger_buffer->flush_needed = 0;
        }
    }
}

/*
 * Callback for signal "buffer_opened".
 */

int
logger_buffer_opened_signal_cb (const void *pointer, void *data,
                                const char *signal,
                                const char *type_data, void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    logger_start_buffer (signal_data, 1);

    return WHOREIRC_RC_OK;
}

/*
 * Callback for signal "buffer_closing".
 */

int
logger_buffer_closing_signal_cb (const void *pointer, void *data,
                                 const char *signal,
                                 const char *type_data, void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    logger_stop (logger_buffer_search_buffer (signal_data), 1);

    return WHOREIRC_RC_OK;
}

/*
 * Callback for signal "buffer_renamed".
 */

int
logger_buffer_renamed_signal_cb (const void *pointer, void *data,
                                 const char *signal,
                                 const char *type_data, void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    logger_stop (logger_buffer_search_buffer (signal_data), 1);
    logger_start_buffer (signal_data, 1);

    return WHOREIRC_RC_OK;
}

/*
 * Checks conditions to display the backlog.
 *
 * Returns:
 *   1: conditions OK (backlog is displayed)
 *   0: conditions not OK (backlog is NOT displayed)
 */

int
logger_backlog_check_conditions (struct t_gui_buffer *buffer)
{
    struct t_hashtable *pointers, *options;
    const char *ptr_condition;
    char *result;
    int condition_ok;

    ptr_condition = weechat_config_string (logger_config_look_backlog_conditions);

    /* empty condition displays the backlog everywhere */
    if (!ptr_condition || !ptr_condition[0])
        return 1;

    pointers = weechat_hashtable_new (32,
                                      WHOREIRC_HASHTABLE_STRING,
                                      WHOREIRC_HASHTABLE_POINTER,
                                      NULL,
                                      NULL);
    if (pointers)
    {
        weechat_hashtable_set (pointers, "window",
                               weechat_window_search_with_buffer (buffer));
        weechat_hashtable_set (pointers, "buffer", buffer);
    }

    options = weechat_hashtable_new (32,
                                     WHOREIRC_HASHTABLE_STRING,
                                     WHOREIRC_HASHTABLE_STRING,
                                     NULL,
                                     NULL);
    if (options)
        weechat_hashtable_set (options, "type", "condition");

    result = weechat_string_eval_expression (ptr_condition,
                                             pointers, NULL, options);
    condition_ok = (result && (strcmp (result, "1") == 0));
    if (result)
        free (result);

    if (pointers)
        weechat_hashtable_free (pointers);
    if (options)
        weechat_hashtable_free (options);

    return condition_ok;
}

/*
 * Displays backlog for a buffer (by reading end of log file).
 */

void
logger_backlog (struct t_gui_buffer *buffer, const char *filename, int lines)
{
    struct t_logger_line *last_lines, *ptr_lines;
    char *charset, *pos_message, *pos_tab, *error, *message;
    time_t datetime, time_now;
    struct tm tm_line;
    int num_lines;

    weechat_buffer_set (buffer, "print_hooks_enabled", "0");

    num_lines = 0;
    last_lines = logger_tail_file (filename, lines);
    ptr_lines = last_lines;
    while (ptr_lines)
    {
        datetime = 0;
        pos_message = strchr (ptr_lines->data, '\t');
        if (pos_message)
        {
            /* initialize structure, because strptime does not do it */
            memset (&tm_line, 0, sizeof (struct tm));
            /*
             * we get current time to initialize daylight saving time in
             * structure tm_line, otherwise printed time will be shifted
             * and will not use DST used on machine
             */
            time_now = time (NULL);
            localtime_r (&time_now, &tm_line);
            pos_message[0] = '\0';
            error = strptime (ptr_lines->data,
                              weechat_config_string (logger_config_file_time_format),
                              &tm_line);
            if (error && !error[0] && (tm_line.tm_year > 0))
                datetime = mktime (&tm_line);
            pos_message[0] = '\t';
        }
        pos_message = (pos_message && (datetime != 0)) ?
            pos_message + 1 : ptr_lines->data;
        charset = weechat_info_get ("charset_terminal", "");
        message = (charset) ?
            weechat_iconv_to_internal (charset, pos_message) : strdup (pos_message);
        if (charset)
            free (charset);
        if (message)
        {
            pos_tab = strchr (message, '\t');
            if (pos_tab)
                pos_tab[0] = '\0';
            weechat_printf_date_tags (buffer, datetime,
                                      "no_highlight,notify_none,logger_backlog",
                                      "%s%s%s%s%s",
                                      weechat_color (weechat_config_string (logger_config_color_backlog_line)),
                                      message,
                                      (pos_tab) ? "\t" : "",
                                      (pos_tab) ? weechat_color (weechat_config_string (logger_config_color_backlog_line)) : "",
                                      (pos_tab) ? pos_tab + 1 : "");
            if (pos_tab)
                pos_tab[0] = '\t';
            free (message);
        }
        num_lines++;
        ptr_lines = ptr_lines->next_line;
    }
    if (last_lines)
        logger_tail_free (last_lines);
    if (num_lines > 0)
    {
        weechat_printf_date_tags (buffer, datetime,
                                  "no_highlight,notify_none,logger_backlog_end",
                                  _("%s===\t%s========== End of backlog (%d lines) =========="),
                                  weechat_color (weechat_config_string (logger_config_color_backlog_end)),
                                  weechat_color (weechat_config_string (logger_config_color_backlog_end)),
                                  num_lines);
        weechat_buffer_set (buffer, "unread", "");
    }
    weechat_buffer_set (buffer, "print_hooks_enabled", "1");
}

/*
 * Callback for signal "logger_backlog".
 */

int
logger_backlog_signal_cb (const void *pointer, void *data,
                          const char *signal,
                          const char *type_data, void *signal_data)
{
    struct t_logger_buffer *ptr_logger_buffer;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    if (weechat_config_integer (logger_config_look_backlog) == 0)
        return WHOREIRC_RC_OK;

    if (!logger_backlog_check_conditions (signal_data))
        return WHOREIRC_RC_OK;

    ptr_logger_buffer = logger_buffer_search_buffer (signal_data);
    if (ptr_logger_buffer && ptr_logger_buffer->log_enabled)
    {
        if (!ptr_logger_buffer->log_filename)
            logger_set_log_filename (ptr_logger_buffer);
        if (ptr_logger_buffer->log_filename)
        {
            ptr_logger_buffer->log_enabled = 0;
            logger_backlog (signal_data,
                            ptr_logger_buffer->log_filename,
                            weechat_config_integer (logger_config_look_backlog));
            ptr_logger_buffer->log_enabled = 1;
        }
    }

    return WHOREIRC_RC_OK;
}

/*
 * Callback for signal "logger_start".
 */

int
logger_start_signal_cb (const void *pointer, void *data,
                        const char *signal, const char *type_data,
                        void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    logger_start_buffer (signal_data, 1);

    return WHOREIRC_RC_OK;
}

/*
 * Callback for signal "logger_stop".
 */

int
logger_stop_signal_cb (const void *pointer, void *data,
                       const char *signal, const char *type_data,
                       void *signal_data)
{
    struct t_logger_buffer *ptr_logger_buffer;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;

    ptr_logger_buffer = logger_buffer_search_buffer (signal_data);
    if (ptr_logger_buffer)
        logger_stop (ptr_logger_buffer, 0);

    return WHOREIRC_RC_OK;
}

/*
 * Adjusts log filenames for all buffers.
 *
 * Filename can change if configuration option is changed, or if day of system
 * date has changed.
 */

void
logger_adjust_log_filenames ()
{
    struct t_infolist *ptr_infolist;
    struct t_logger_buffer *ptr_logger_buffer;
    struct t_gui_buffer *ptr_buffer;
    char *log_filename;

    ptr_infolist = weechat_infolist_get ("buffer", NULL, NULL);
    if (ptr_infolist)
    {
        while (weechat_infolist_next (ptr_infolist))
        {
            ptr_buffer = weechat_infolist_pointer (ptr_infolist, "pointer");
            ptr_logger_buffer = logger_buffer_search_buffer (ptr_buffer);
            if (ptr_logger_buffer && ptr_logger_buffer->log_filename)
            {
                log_filename = logger_get_filename (ptr_logger_buffer->buffer);
                if (log_filename)
                {
                    if (strcmp (log_filename, ptr_logger_buffer->log_filename) != 0)
                    {
                        /*
                         * log filename has changed (probably due to day
                         * change),then we'll use new filename
                         */
                        logger_stop (ptr_logger_buffer, 1);
                        logger_start_buffer (ptr_buffer, 1);
                    }
                    free (log_filename);
                }
            }
        }
        weechat_infolist_free (ptr_infolist);
    }
}

/*
 * Callback for signal "day_changed".
 */

int
logger_day_changed_signal_cb (const void *pointer, void *data,
                              const char *signal,
                              const char *type_data, void *signal_data)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) signal;
    (void) type_data;
    (void) signal_data;

    logger_adjust_log_filenames ();

    return WHOREIRC_RC_OK;
}

/*
 * Gets info with tags of line: log level and if prefix is a nick.
 */

void
logger_get_line_tag_info (int tags_count, const char **tags,
                          int *log_level, int *prefix_is_nick)
{
    int i, log_level_set, prefix_is_nick_set;

    if (log_level)
        *log_level = LOGGER_LEVEL_DEFAULT;
    if (prefix_is_nick)
        *prefix_is_nick = 0;

    log_level_set = 0;
    prefix_is_nick_set = 0;

    for (i = 0; i < tags_count; i++)
    {
        if (log_level && !log_level_set)
        {
            if (strcmp (tags[i], "no_log") == 0)
            {
                /* log disabled on line: set level to -1 */
                *log_level = -1;
                log_level_set = 1;
            }
            else if (strncmp (tags[i], "log", 3) == 0)
            {
                /* set log level for line */
                if (isdigit ((unsigned char)tags[i][3]))
                {
                    *log_level = (tags[i][3] - '0');
                    log_level_set = 1;
                }
            }
        }
        if (prefix_is_nick && !prefix_is_nick_set)
        {
            if (strncmp (tags[i], "prefix_nick", 11) == 0)
            {
                *prefix_is_nick = 1;
                prefix_is_nick_set = 1;
            }
        }
    }
}

/*
 * Callback for print hooked.
 */

int
logger_print_cb (const void *pointer, void *data,
                 struct t_gui_buffer *buffer, time_t date,
                 int tags_count, const char **tags,
                 int displayed, int highlight,
                 const char *prefix, const char *message)
{
    struct t_logger_buffer *ptr_logger_buffer;
    struct tm *date_tmp;
    char buf_time[256];
    int line_log_level, prefix_is_nick;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) displayed;
    (void) highlight;

    logger_get_line_tag_info (tags_count, tags, &line_log_level,
                              &prefix_is_nick);
    if (line_log_level >= 0)
    {
        ptr_logger_buffer = logger_buffer_search_buffer (buffer);
        if (ptr_logger_buffer
            && ptr_logger_buffer->log_enabled
            && (date > 0)
            && (line_log_level <= ptr_logger_buffer->log_level))
        {
            buf_time[0] = '\0';
            date_tmp = localtime (&date);
            if (date_tmp)
            {
                if (strftime (buf_time, sizeof (buf_time) - 1,
                              weechat_config_string (logger_config_file_time_format),
                              date_tmp) == 0)
                    buf_time[0] = '\0';
            }

            logger_write_line (ptr_logger_buffer,
                               "%s\t%s%s%s\t%s",
                               buf_time,
                               (prefix && prefix_is_nick) ? weechat_config_string (logger_config_file_nick_prefix) : "",
                               (prefix) ? prefix : "",
                               (prefix && prefix_is_nick) ? weechat_config_string (logger_config_file_nick_suffix) : "",
                               message);
        }
    }

    return WHOREIRC_RC_OK;
}

/*
 * Callback for logger timer.
 */

int
logger_timer_cb (const void *pointer, void *data, int remaining_calls)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) remaining_calls;

    logger_flush ();

    return WHOREIRC_RC_OK;
}

/*
 * Initializes logger plugin.
 */

int
weechat_plugin_init (struct t_weechat_plugin *plugin, int argc, char *argv[])
{
    /* make C compiler happy */
    (void) argc;
    (void) argv;

    weechat_plugin = plugin;

    if (!logger_config_init ())
        return WHOREIRC_RC_ERROR;

    logger_config_read ();

    logger_command_init ();

    logger_start_buffer_all (1);

    weechat_hook_signal ("buffer_opened",
                         &logger_buffer_opened_signal_cb, NULL, NULL);
    weechat_hook_signal ("buffer_closing",
                         &logger_buffer_closing_signal_cb, NULL, NULL);
    weechat_hook_signal ("buffer_renamed",
                         &logger_buffer_renamed_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_backlog",
                         &logger_backlog_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_start",
                         &logger_start_signal_cb, NULL, NULL);
    weechat_hook_signal ("logger_stop",
                         &logger_stop_signal_cb, NULL, NULL);
    weechat_hook_signal ("day_changed",
                         &logger_day_changed_signal_cb, NULL, NULL);

    weechat_hook_print (NULL, NULL, NULL, 1, &logger_print_cb, NULL, NULL);

    logger_info_init ();

    return WHOREIRC_RC_OK;
}

/*
 * Ends logger plugin.
 */

int
weechat_plugin_end (struct t_weechat_plugin *plugin)
{
    /* make C compiler happy */
    (void) plugin;

    if (logger_timer)
    {
        weechat_unhook (logger_timer);
        logger_timer = NULL;
    }

    logger_config_write ();

    logger_stop_all (1);

    logger_config_free ();

    return WHOREIRC_RC_OK;
}
