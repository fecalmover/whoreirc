/*
 * Copyright (C) 2012-2019 Sébastien Helleu <flashcode@flashtux.org>
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

#ifndef WHOREIRC_URL_H
#define WHOREIRC_URL_H

#include <stdio.h>

struct t_hashtable;
struct t_infolist;

enum t_url_type
{
    URL_TYPE_STRING = 0,
    URL_TYPE_LONG,
    URL_TYPE_LONGLONG,
    URL_TYPE_MASK,
    URL_TYPE_LIST,
};

struct t_url_constant
{
    char *name;                        /* string with name of constant      */
    long value;                        /* value of constant                 */
};

struct t_url_option
{
    char *name;                        /* name of option                    */
    int option;                        /* option (for curl_easy_setopt())   */
    enum t_url_type type;              /* type of argument expected         */
    struct t_url_constant *constants;  /* constants allowed for this option */
};

struct t_url_file
{
    char *filename;                    /* filename                          */
    FILE *stream;                      /* file stream                       */
};

extern struct t_url_option url_options[];

extern int weeurl_download (const char *url, struct t_hashtable *options);
extern int weeurl_option_add_to_infolist (struct t_infolist *infolist,
                                          struct t_url_option *option);

#endif /* WHOREIRC_URL_H */
