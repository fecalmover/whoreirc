/*
 * relay-config.c - relay configuration options (file relay.conf)
 *
 * Copyright (C) 2003-2019 Sébastien Helleu <flashcode@flashtux.org>
 *
 * This file is part of WeeChat, the extensible chat client.
 *
 * WeeChat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * WeeChat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with WeeChat.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <regex.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "../weechat-plugin.h"
#include "relay.h"
#include "relay-config.h"
#include "irc/relay-irc.h"
#include "relay-client.h"
#include "relay-buffer.h"
#include "relay-network.h"
#include "relay-server.h"


struct t_config_file *relay_config_file = NULL;
struct t_config_section *relay_config_section_port = NULL;
struct t_config_section *relay_config_section_path = NULL;

/* relay config, look section */

struct t_config_option *relay_config_look_auto_open_buffer;
struct t_config_option *relay_config_look_raw_messages;

/* relay config, color section */

struct t_config_option *relay_config_color_client;
struct t_config_option *relay_config_color_status[RELAY_NUM_STATUS];
struct t_config_option *relay_config_color_text;
struct t_config_option *relay_config_color_text_bg;
struct t_config_option *relay_config_color_text_selected;

/* relay config, network section */

struct t_config_option *relay_config_network_allow_empty_password;
struct t_config_option *relay_config_network_allowed_ips;
struct t_config_option *relay_config_network_bind_address;
struct t_config_option *relay_config_network_clients_purge_delay;
struct t_config_option *relay_config_network_compression_level;
struct t_config_option *relay_config_network_ipv6;
struct t_config_option *relay_config_network_max_clients;
struct t_config_option *relay_config_network_password;
struct t_config_option *relay_config_network_ssl_cert_key;
struct t_config_option *relay_config_network_ssl_priorities;
struct t_config_option *relay_config_network_totp_secret;
struct t_config_option *relay_config_network_totp_window;
struct t_config_option *relay_config_network_websocket_allowed_origins;

/* relay config, irc section */

struct t_config_option *relay_config_irc_backlog_max_minutes;
struct t_config_option *relay_config_irc_backlog_max_number;
struct t_config_option *relay_config_irc_backlog_since_last_disconnect;
struct t_config_option *relay_config_irc_backlog_since_last_message;
struct t_config_option *relay_config_irc_backlog_tags;
struct t_config_option *relay_config_irc_backlog_time_format;

/* relay config, weechat section */

struct t_config_option *relay_config_weechat_commands;

/* other */

regex_t *relay_config_regex_allowed_ips = NULL;
regex_t *relay_config_regex_websocket_allowed_origins = NULL;
struct t_hashtable *relay_config_hashtable_irc_backlog_tags = NULL;


/*
 * Callback for changes on options that require a refresh of relay list.
 */

void
relay_config_refresh_cb (const void *pointer, void *data,
                         struct t_config_option *option)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (relay_buffer)
        relay_buffer_refresh (NULL);
}

/*
 * Callback for changes on option "relay.network.allowed_ips".
 */

void
relay_config_change_network_allowed_ips (const void *pointer, void *data,
                                         struct t_config_option *option)
{
    const char *allowed_ips;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (relay_config_regex_allowed_ips)
    {
        regfree (relay_config_regex_allowed_ips);
        free (relay_config_regex_allowed_ips);
        relay_config_regex_allowed_ips = NULL;
    }

    allowed_ips = weechat_config_string (relay_config_network_allowed_ips);
    if (allowed_ips && allowed_ips[0])
    {
        relay_config_regex_allowed_ips = malloc (sizeof (*relay_config_regex_allowed_ips));
        if (relay_config_regex_allowed_ips)
        {
            if (weechat_string_regcomp (relay_config_regex_allowed_ips,
                                        allowed_ips,
                                        REG_EXTENDED | REG_ICASE) != 0)
            {
                free (relay_config_regex_allowed_ips);
                relay_config_regex_allowed_ips = NULL;
            }
        }
    }
}

/*
 * Callback for changes on option "relay.network.bind_address".
 */

void
relay_config_change_network_bind_address_cb (const void *pointer, void *data,
                                             struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    for (ptr_server = relay_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        relay_server_close_socket (ptr_server);
        relay_server_create_socket (ptr_server);
    }
}

/*
 * Callback for changes on option "relay.network.ipv6".
 */

void
relay_config_change_network_ipv6_cb (const void *pointer, void *data,
                                     struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    for (ptr_server = relay_servers; ptr_server;
         ptr_server = ptr_server->next_server)
    {
        relay_server_get_protocol_args (ptr_server->protocol_string,
                                        &ptr_server->ipv4, &ptr_server->ipv6,
                                        NULL, &ptr_server->unix_socket, NULL, NULL);
        relay_server_close_socket (ptr_server);
        relay_server_create_socket (ptr_server);
    }
}

/*
 * Callback for changes on option "relay.network.ssl_cert_key".
 */

void
relay_config_change_network_ssl_cert_key (const void *pointer, void *data,
                                          struct t_config_option *option)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (relay_network_init_ok)
        relay_network_set_ssl_cert_key (1);
}

/*
 * Checks if option "relay.network.totp_secret" is valid.
 *
 * Returns:
 *   1: value is valid
 *   0: value is not valid
 */

int
relay_config_check_network_totp_secret (const void *pointer, void *data,
                                        struct t_config_option *option,
                                        const char *value)
{
    char *totp_secret, *secret;
    int rc, length;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    totp_secret = NULL;
    secret = NULL;

    totp_secret = weechat_string_eval_expression (value, NULL, NULL, NULL);
    if (totp_secret && totp_secret[0])
    {
        secret = malloc (strlen (totp_secret) + 1);
        if (!secret)
            goto error;
        length = weechat_string_base_decode (32, totp_secret, secret);
        if (length < 0)
            goto error;
    }

    rc = 1;
    goto end;

error:
    rc = 0;
    weechat_printf (NULL,
                    _("%s%s: invalid value for option "
                      "\"relay.network.totp_secret\"; it must be a valid "
                      "string encoded in base32 "
                      "(only letters and digits from 2 to 7)"),
                    weechat_prefix ("error"), RELAY_PLUGIN_NAME);

end:
    if (totp_secret)
        free (totp_secret);
    if (secret)
        free (secret);
    return rc;
}

/*
 * Checks if option "relay.network.ssl_priorities" is valid.
 *
 * Returns:
 *   1: value is valid
 *   0: value is not valid
 */

int
relay_config_check_network_ssl_priorities (const void *pointer, void *data,
                                           struct t_config_option *option,
                                           const char *value)
{
#ifdef HAVE_GNUTLS
    gnutls_priority_t priority_cache;
    const char *pos_error;
    int rc;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    pos_error = value;

    if (value && value[0])
    {
        rc = gnutls_priority_init (&priority_cache, value, &pos_error);
        if (rc == GNUTLS_E_SUCCESS)
        {
            gnutls_priority_deinit (priority_cache);
            return 1;
        }
    }

    weechat_printf (NULL,
                    _("%s%s: invalid priorities string, error "
                      "at this position in string: \"%s\""),
                    weechat_prefix ("error"), RELAY_PLUGIN_NAME,
                    (pos_error) ? pos_error : value);

    return 0;
#else
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;
    (void) value;

    return 1;
#endif /* HAVE_GNUTLS */
}

/*
 * Callback for changes on option "relay.network.ssl_priorities".
 */

void
relay_config_change_network_ssl_priorities (const void *pointer, void *data,
                                            struct t_config_option *option)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

#ifdef HAVE_GNUTLS
    if (relay_network_init_ok && relay_gnutls_priority_cache)
    {
        gnutls_priority_deinit (*relay_gnutls_priority_cache);
        relay_network_set_priority ();
    }
#endif /* HAVE_GNUTLS */
}

/*
 * Callback for changes on option "relay.network.websocker_allowed_origins".
 */

void
relay_config_change_network_websocket_allowed_origins (const void *pointer, void *data,
                                                       struct t_config_option *option)
{
    const char *allowed_origins;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (relay_config_regex_websocket_allowed_origins)
    {
        regfree (relay_config_regex_websocket_allowed_origins);
        free (relay_config_regex_websocket_allowed_origins);
        relay_config_regex_websocket_allowed_origins = NULL;
    }

    allowed_origins = weechat_config_string (relay_config_network_websocket_allowed_origins);
    if (allowed_origins && allowed_origins[0])
    {
        relay_config_regex_websocket_allowed_origins = malloc (sizeof (*relay_config_regex_websocket_allowed_origins));
        if (relay_config_regex_websocket_allowed_origins)
        {
            if (weechat_string_regcomp (relay_config_regex_websocket_allowed_origins,
                                        allowed_origins,
                                        REG_EXTENDED | REG_ICASE) != 0)
            {
                free (relay_config_regex_websocket_allowed_origins);
                relay_config_regex_websocket_allowed_origins = NULL;
            }
        }
    }
}

/*
 * Checks if IRC backlog tags are valid.
 *
 * Returns:
 *   1: IRC backlog tags are valid
 *   0: IRC backlog tags are not valid
 */

int
relay_config_check_irc_backlog_tags (const void *pointer, void *data,
                                     struct t_config_option *option,
                                     const char *value)
{
    char **tags;
    int num_tags, i, rc;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    rc = 1;

    /* "*" means all tags */
    if (strcmp (value, "*") == 0)
        return rc;

    /* split tags and check them */
    tags = weechat_string_split (value, ",", NULL,
                                 WHOREIRC_STRING_SPLIT_STRIP_LEFT
                                 | WHOREIRC_STRING_SPLIT_STRIP_RIGHT
                                 | WHOREIRC_STRING_SPLIT_COLLAPSE_SEPS,
                                 0, &num_tags);
    if (tags)
    {
        for (i = 0; i < num_tags; i++)
        {
            if (relay_irc_search_backlog_commands_tags (tags[i]) < 0)
            {
                rc = 0;
                break;
            }
        }
        weechat_string_free_split (tags);
    }

    return rc;
}

/*
 * Callback for changes on option "relay.irc.backlog_tags".
 */

void
relay_config_change_irc_backlog_tags (const void *pointer, void *data,
                                      struct t_config_option *option)
{
    char **items;
    int num_items, i;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (!relay_config_hashtable_irc_backlog_tags)
    {
        relay_config_hashtable_irc_backlog_tags = weechat_hashtable_new (
            32,
            WHOREIRC_HASHTABLE_STRING,
            WHOREIRC_HASHTABLE_STRING,
            NULL, NULL);
    }
    else
        weechat_hashtable_remove_all (relay_config_hashtable_irc_backlog_tags);

    items = weechat_string_split (
        weechat_config_string (relay_config_irc_backlog_tags),
        ",",
        NULL,
        WHOREIRC_STRING_SPLIT_STRIP_LEFT
        | WHOREIRC_STRING_SPLIT_STRIP_RIGHT
        | WHOREIRC_STRING_SPLIT_COLLAPSE_SEPS,
        0,
        &num_items);
    if (items)
    {
        for (i = 0; i < num_items; i++)
        {
            weechat_hashtable_set (relay_config_hashtable_irc_backlog_tags,
                                   items[i],
                                   NULL);
        }
        weechat_string_free_split (items);
    }
}

/*
 * Checks if a port is valid.
 *
 * Returns:
 *   1: port is valid
 *   0: port is not valid
 */

int
relay_config_check_port_cb (const void *pointer, void *data,
                            struct t_config_option *option,
                            const char *value)
{
    char *error;
    long port;
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    error = NULL;
    port = strtol (value, &error, 10);
    ptr_server = relay_server_search_port ((int)port);
    if (ptr_server)
    {
        weechat_printf (NULL, _("%s%s: error: port \"%d\" is already used"),
                        weechat_prefix ("error"),
                        RELAY_PLUGIN_NAME, (int)port);
        return 0;
    }

    return 1;
}

/*
 * Checks if a UNIX path is too long or empty.
 *
 * Returns:
 *   1: path is valid
 *   0: path is empty or too long
 */

int
relay_config_check_path_length (const char *path)
{
    struct sockaddr_un addr;
    size_t length, max_length;

    length = strlen (path);
    if (length == 0)
    {
        weechat_printf (NULL, _("%s%s: error: path is empty"),
                        weechat_prefix ("error"), RELAY_PLUGIN_NAME);
        return 0;
    }

    max_length = sizeof (addr.sun_path);
    if (length + 1 > max_length)
    {
        weechat_printf (NULL,
                        _("%s%s: error: path \"%s\" too long (length: %d; max: %d)"),
                        weechat_prefix ("error"), RELAY_PLUGIN_NAME, path,
                        length, max_length);
        return 0;
    }

    return 1;
}

/*
 * Checks if a UNIX path is available: it is available if not existing, or
 * if a file of type socket already exists.
 *
 * Returns:
 *    0: path is available
 *   -1: path already exists and is not a socket
 *   -2: invalid path
 */

int
relay_config_check_path_available (const char *path)
{
    struct stat buf;
    int rc;

    rc = stat (path, &buf);

    /* OK if an existing file is a socket */
    if ((rc == 0) && S_ISSOCK(buf.st_mode))
        return 0;

    /* error if an existing file is NOT a socket */
    if (rc == 0)
        return -1;

    /* OK if the file does not exist */
    if (errno == ENOENT)
        return 0;

    /* on any other error, the path it considered as not available */
    return -2;
}

/*
 * Checks if a path is valid.
 *
 * Returns:
 *   1: path is valid
 *   0: path is not valid
 */

int
relay_config_check_path_cb (const void *pointer, void *data,
                            struct t_config_option *option,
                            const char *value)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;
    (void) option;

    if (!relay_config_check_path_length (value))
        return 0;

    if (relay_server_search_path (value))
    {
        weechat_printf (NULL, _("%s%s: error: path \"%s\" is already used"),
                        weechat_prefix ("error"), RELAY_PLUGIN_NAME, value);
        return 0;
    }

    return 1;
}

/*
 * Callback for changes on options in section "path".
 */

void
relay_config_change_path_cb (const void *pointer, void *data,
                             struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    ptr_server = relay_server_search (weechat_config_option_get_pointer (option, "name"));
    if (ptr_server)
    {
        relay_server_update_path (ptr_server,
                                  (const char *)weechat_config_option_get_pointer (option, "value"));
    }
}

/*
 * Callback called when an option is deleted in section "path".
 */

void
relay_config_delete_path_cb (const void *pointer, void *data,
                             struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    ptr_server = relay_server_search (weechat_config_option_get_pointer (option, "name"));
    if (ptr_server)
        relay_server_free (ptr_server);
}

/*
 * Callback for changes on options in section "port".
 */

void
relay_config_change_port_cb (const void *pointer, void *data,
                             struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    ptr_server = relay_server_search (weechat_config_option_get_pointer (option, "name"));
    if (ptr_server)
    {
        relay_server_update_port (ptr_server,
                                  *((int *)weechat_config_option_get_pointer (option, "value")));
    }
}

/*
 * Callback called when an option is deleted in section "port".
 */

void
relay_config_delete_port_cb (const void *pointer, void *data,
                             struct t_config_option *option)
{
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    ptr_server = relay_server_search (weechat_config_option_get_pointer (option, "name"));
    if (ptr_server)
        relay_server_free (ptr_server);
}

/*
 * Callback called when an option is created in section "port" or "path".
 */

int
relay_config_create_option_port_path (const void *pointer, void *data,
                                      struct t_config_file *config_file,
                                      struct t_config_section *section,
                                      const char *option_name,
                                      const char *value)
{
    int rc, protocol_number, ipv4, ipv6, ssl, unix_socket;
    char *error, *protocol, *protocol_args;
    long port;
    struct t_relay_server *ptr_server;

    /* make C compiler happy */
    (void) pointer;
    (void) data;

    rc = WHOREIRC_CONFIG_OPTION_SET_OK_SAME_VALUE;

    protocol_number = -1;
    port = -1;

    relay_server_get_protocol_args (option_name, &ipv4, &ipv6, &ssl,
                                    &unix_socket, &protocol, &protocol_args);

#ifndef HAVE_GNUTLS
    if (ssl)
    {
        weechat_printf (NULL,
                        _("%s%s: cannot use SSL because WeeChat was not built "
                          "with GnuTLS support"),
                        weechat_prefix ("error"), RELAY_PLUGIN_NAME);
        rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
    }
#endif /* HAVE_GNUTLS */

    if (rc != WHOREIRC_CONFIG_OPTION_SET_ERROR)
    {
        if (protocol)
            protocol_number = relay_protocol_search (protocol);

        if (protocol_number < 0)
        {
            weechat_printf (NULL, _("%s%s: error: unknown protocol \"%s\""),
                            weechat_prefix ("error"),
                            RELAY_PLUGIN_NAME, protocol);
            rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
        }

        if ((protocol_number == RELAY_PROTOCOL_WHOREIRC) && protocol_args)
        {
            weechat_printf (NULL, _("%s%s: error: name is not allowed for "
                                    "protocol \"%s\""),
                            weechat_prefix ("error"),
                            RELAY_PLUGIN_NAME, protocol);
            rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
        }
    }

    if (rc != WHOREIRC_CONFIG_OPTION_SET_ERROR)
    {
        if (weechat_config_search_option (config_file, section, option_name))
        {
            weechat_printf (NULL, _("%s%s: error: relay for \"%s\" already exists"),
                            weechat_prefix ("error"),
                            RELAY_PLUGIN_NAME, option_name);
            rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
        }
    }

    if (rc != WHOREIRC_CONFIG_OPTION_SET_ERROR)
    {
        if (unix_socket)
        {
            ptr_server = relay_server_search_path (value);
        }
        else
        {
            error = NULL;
            port = strtol (value, &error, 10);
            ptr_server = relay_server_search_port ((int)port);
        }
        if (ptr_server)
        {
            if (unix_socket)
            {
                weechat_printf (NULL,
                                _("%s%s: error: path \"%s\" is already used"),
                                weechat_prefix ("error"), RELAY_PLUGIN_NAME,
                                value);
            }
            else
            {
                weechat_printf (NULL,
                                _("%s%s: error: port \"%d\" is already used"),
                                weechat_prefix ("error"), RELAY_PLUGIN_NAME,
                                (int)port);
            }
            rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
        }
    }

    if (rc != WHOREIRC_CONFIG_OPTION_SET_ERROR)
    {
        if (relay_server_new (option_name, protocol_number, protocol_args,
                              port, value, ipv4, ipv6, ssl, unix_socket))
        {
            /* create configuration option */
            if (unix_socket)
            {
                weechat_config_new_option (
                    config_file, section,
                    option_name, "string",
                    _("path to a socket file; \"%h\" at beginning of string "
                      "is replaced by WeeChat home (\"~/.weechat\" by default), "
                      "content is evaluated (see /help eval)"),
                    NULL, 0, 0, "", value, 0,
                    &relay_config_check_path_cb, NULL, NULL,
                    &relay_config_change_path_cb, NULL, NULL,
                    &relay_config_delete_path_cb, NULL, NULL);
            }
            else
            {
                weechat_config_new_option (
                    config_file, section,
                    option_name, "integer",
                    _("port for relay"),
                    NULL, 0, 65535, "", value, 0,
                    &relay_config_check_port_cb, NULL, NULL,
                    &relay_config_change_port_cb, NULL, NULL,
                    &relay_config_delete_port_cb, NULL, NULL);
            }
            rc = WHOREIRC_CONFIG_OPTION_SET_OK_SAME_VALUE;
        }
        else
            rc = WHOREIRC_CONFIG_OPTION_SET_ERROR;
    }

    if (protocol)
        free (protocol);
    if (protocol_args)
        free (protocol_args);

    return rc;
}

/*
 * Reloads relay configuration file.
 */

int
relay_config_reload (const void *pointer, void *data,
                     struct t_config_file *config_file)
{
    /* make C compiler happy */
    (void) pointer;
    (void) data;

    weechat_config_section_free_options (relay_config_section_port);
    relay_server_free_all ();

    return weechat_config_reload (config_file);
}

/*
 * Initializes relay configuration file.
 *
 * Returns:
 *   1: OK
 *   0: error
 */

int
relay_config_init ()
{
    struct t_config_section *ptr_section;

    relay_config_file = weechat_config_new (RELAY_CONFIG_NAME,
                                            &relay_config_reload, NULL, NULL);
    if (!relay_config_file)
        return 0;

    /* section look */
    ptr_section = weechat_config_new_section (relay_config_file, "look",
                                              0, 0,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_look_auto_open_buffer = weechat_config_new_option (
        relay_config_file, ptr_section,
        "auto_open_buffer", "boolean",
        N_("auto open relay buffer when a new client is connecting"),
        NULL, 0, 0, "on", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_look_raw_messages = weechat_config_new_option (
        relay_config_file, ptr_section,
        "raw_messages", "integer",
        N_("number of raw messages to save in memory when raw data buffer is "
           "closed (messages will be displayed when opening raw data buffer)"),
        NULL, 0, 65535, "256", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* section color */
    ptr_section = weechat_config_new_section (relay_config_file, "color",
                                              0, 0,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_color_client = weechat_config_new_option (
        relay_config_file, ptr_section,
        "client", "color",
        N_("text color for client description"),
        NULL, 0, 0, "cyan", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_color_status[RELAY_STATUS_CONNECTING] = weechat_config_new_option (
        relay_config_file, ptr_section,
        "status_connecting", "color",
        N_("text color for \"connecting\" status"),
        NULL, 0, 0, "yellow", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_status[RELAY_STATUS_WAITING_AUTH] = weechat_config_new_option (
        relay_config_file, ptr_section,
        "status_waiting_auth", "color",
        N_("text color for \"waiting authentication\" status"),
        NULL, 0, 0, "brown", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_status[RELAY_STATUS_CONNECTED] = weechat_config_new_option (
        relay_config_file, ptr_section,
        "status_active", "color",
        N_("text color for \"connected\" status"),
        NULL, 0, 0, "lightblue", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_status[RELAY_STATUS_AUTH_FAILED] = weechat_config_new_option (
        relay_config_file, ptr_section,
        "status_auth_failed", "color",
        N_("text color for \"authentication failed\" status"),
        NULL, 0, 0, "lightred", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_status[RELAY_STATUS_DISCONNECTED] = weechat_config_new_option (
        relay_config_file, ptr_section,
        "status_disconnected", "color",
        N_("text color for \"disconnected\" status"),
        NULL, 0, 0, "lightred", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_text = weechat_config_new_option (
        relay_config_file, ptr_section,
        "text", "color",
        N_("text color in relay buffer"),
        NULL, 0, 0, "default", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_text_bg = weechat_config_new_option (
        relay_config_file, ptr_section,
        "text_bg", "color",
        N_("background color in relay buffer"),
        NULL, 0, 0, "default", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_color_text_selected = weechat_config_new_option (
        relay_config_file, ptr_section,
        "text_selected", "color",
        N_("text color of selected line in relay buffer"),
        NULL, 0, 0, "white", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_refresh_cb, NULL, NULL,
        NULL, NULL, NULL);

    /* section network */
    ptr_section = weechat_config_new_section (relay_config_file, "network",
                                              0, 0,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_network_allow_empty_password = weechat_config_new_option (
        relay_config_file, ptr_section,
        "allow_empty_password", "boolean",
        N_("allow empty password in relay (it should be enabled only for "
           "tests or local network)"),
        NULL, 0, 0, "off", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_allowed_ips = weechat_config_new_option (
        relay_config_file, ptr_section,
        "allowed_ips", "string",
        N_("POSIX extended regular expression with IPs allowed to use relay "
           "(case insensitive, use \"(?-i)\" at beginning to make it case "
           "sensitive), example: "
           "\"^(123\\.45\\.67\\.89|192\\.160\\..*)$\""),
        NULL, 0, 0, "", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_change_network_allowed_ips, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_bind_address = weechat_config_new_option (
        relay_config_file, ptr_section,
        "bind_address", "string",
        N_("address for bind (if empty, connection is possible on all "
           "interfaces, use \"127.0.0.1\" to allow connections from "
            "local machine only)"),
        NULL, 0, 0, "", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_change_network_bind_address_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_clients_purge_delay = weechat_config_new_option (
        relay_config_file, ptr_section,
        "clients_purge_delay", "integer",
        N_("delay for purging disconnected clients (in minutes, 0 = purge "
           "clients immediately, -1 = never purge)"),
        NULL, -1, 60 * 24 * 30, "0", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_compression_level = weechat_config_new_option (
        relay_config_file, ptr_section,
        "compression_level", "integer",
        N_("compression level for packets sent to client with WeeChat protocol "
           "(0 = disable compression, 1 = low compression ... 9 = best "
           "compression)"),
        NULL, 0, 9, "6", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_ipv6 = weechat_config_new_option (
        relay_config_file, ptr_section,
        "ipv6", "boolean",
        N_("listen on IPv6 socket by default (in addition to IPv4 which is "
            "default); protocols IPv4 and IPv6 can be forced (individually or "
           "together) in the protocol name (see /help relay)"),
        NULL, 0, 0, "on", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_change_network_ipv6_cb, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_max_clients = weechat_config_new_option (
        relay_config_file, ptr_section,
        "max_clients", "integer",
        N_("maximum number of clients connecting to a port (0 = no limit)"),
        NULL, 0, INT_MAX, "5", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_password = weechat_config_new_option (
        relay_config_file, ptr_section,
        "password", "string",
        N_("password required by clients to access this relay (empty value "
           "means no password required, see option "
           "relay.network.allow_empty_password) (note: content is evaluated, "
           "see /help eval)"),
        NULL, 0, 0, "", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_ssl_cert_key = weechat_config_new_option (
        relay_config_file, ptr_section,
        "ssl_cert_key", "string",
        N_("file with SSL certificate and private key (for serving clients "
           "with SSL)"),
        NULL, 0, 0, "%h/ssl/relay.pem", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_change_network_ssl_cert_key, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_ssl_priorities = weechat_config_new_option (
        relay_config_file, ptr_section,
        "ssl_priorities", "string",
        N_("string with priorities for gnutls (for syntax, see "
           "documentation of function gnutls_priority_init in gnutls "
           "manual, common strings are: \"PERFORMANCE\", \"NORMAL\", "
           "\"SECURE128\", \"SECURE256\", \"EXPORT\", \"NONE\")"),
        NULL, 0, 0, "NORMAL:-VERS-SSL3.0", NULL, 0,
        &relay_config_check_network_ssl_priorities, NULL, NULL,
        &relay_config_change_network_ssl_priorities, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_totp_secret = weechat_config_new_option (
        relay_config_file, ptr_section,
        "totp_secret", "string",
        N_("secret for the generation of the Time-based One-Time Password "
           "(TOTP), encoded in base32 (only letters and digits from 2 to 7); "
           "it is used as second factor in weechat protocol, in addition to "
           "the password, which must not be empty "
           "(empty value means no TOTP is required) "
           "(note: content is evaluated, see /help eval)"),
        NULL, 0, 0, "", NULL, 0,
        &relay_config_check_network_totp_secret, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_network_totp_window = weechat_config_new_option (
        relay_config_file, ptr_section,
        "totp_window", "integer",
        N_("number of Time-based One-Time Passwords to accept before and "
           "after the current one: "
           "0 = accept only the current password, "
           "1 = accept one password before, the current, and one after, "
           "2 = accept two passwords before, the current, and two after, "
           "...; a high number reduces the security level "
           "(0 or 1 are recommended values)"),
        NULL, 0, 256, "0", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_network_websocket_allowed_origins = weechat_config_new_option (
        relay_config_file, ptr_section,
        "websocket_allowed_origins", "string",
        N_("POSIX extended regular expression with origins allowed in "
           "websockets (case insensitive, use \"(?-i)\" at beginning to make "
           "it case sensitive), example: "
           "\"^https?://(www\\.)?example\\.(com|org)\""),
        NULL, 0, 0, "", NULL, 0,
        NULL, NULL, NULL,
        &relay_config_change_network_websocket_allowed_origins, NULL, NULL,
        NULL, NULL, NULL);

    /* section irc */
    ptr_section = weechat_config_new_section (relay_config_file, "irc",
                                              0, 0,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_irc_backlog_max_minutes = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_max_minutes", "integer",
        N_("maximum number of minutes in backlog per IRC channel "
           "(0 = unlimited, examples: 1440 = one day, 10080 = one week, "
           "43200 = one month, 525600 = one year)"),
        NULL, 0, INT_MAX, "1440", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_irc_backlog_max_number = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_max_number", "integer",
        N_("maximum number of lines in backlog per IRC channel "
           "(0 = unlimited)"),
        NULL, 0, INT_MAX, "256", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_irc_backlog_since_last_disconnect = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_since_last_disconnect", "boolean",
        N_("display backlog starting from last client disconnect"),
        NULL, 0, 0, "on", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_irc_backlog_since_last_message = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_since_last_message", "boolean",
        N_("display backlog starting from your last message"),
        NULL, 0, 0, "off", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    relay_config_irc_backlog_tags = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_tags", "string",
        N_("comma-separated list of messages tags which are displayed in "
           "backlog per IRC channel (supported tags: \"irc_join\", "
           "\"irc_part\", \"irc_quit\", \"irc_nick\", \"irc_privmsg\"), "
           "\"*\" = all supported tags"),
        NULL, 0, 0, "irc_privmsg", NULL, 0,
        &relay_config_check_irc_backlog_tags, NULL, NULL,
        &relay_config_change_irc_backlog_tags, NULL, NULL,
        NULL, NULL, NULL);
    relay_config_irc_backlog_time_format = weechat_config_new_option (
        relay_config_file, ptr_section,
        "backlog_time_format", "string",
        N_("format for time in backlog messages (see man strftime for format) "
           "(not used if server capability \"server-time\" was enabled by "
           "client, because time is sent as irc tag); empty string = disable "
           "time in backlog messages"),
        NULL, 0, 0, "[%H:%M] ", NULL, 0,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    /* section weechat */
    ptr_section = weechat_config_new_section (relay_config_file, "weechat",
                                              0, 0,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL,
                                              NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_weechat_commands = weechat_config_new_option (
        relay_config_file, ptr_section,
        "commands", "string",
        N_("comma-separated list of commands allowed/denied when input "
           "data (text or command) is received from a client; "
           "\"*\" means any command, a name beginning with \"!\" is "
           "a negative value to prevent a command from being executed, "
           "wildcard \"*\" is allowed in names; this option should be set if "
           "the relay client is not safe (someone could use it to run "
           "commands); for example \"*,!exec,!quit\" allows any command "
           "except /exec and /quit"),
        NULL, 0, 0, "", NULL, 0,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL);

    /* section port */
    ptr_section = weechat_config_new_section (
        relay_config_file, "port",
        1, 1,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        &relay_config_create_option_port_path, NULL, NULL,
        NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_section_port = ptr_section;

    /* section path */
    ptr_section = weechat_config_new_section (
        relay_config_file, "path",
        1, 1,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        NULL, NULL, NULL,
        &relay_config_create_option_port_path, NULL, NULL,
        NULL, NULL, NULL);
    if (!ptr_section)
    {
        weechat_config_free (relay_config_file);
        relay_config_file = NULL;
        return 0;
    }

    relay_config_section_path = ptr_section;

    return 1;
}

/*
 * Reads relay configuration file.
 */

int
relay_config_read ()
{
    int rc;

    rc = weechat_config_read (relay_config_file);
    if (rc == WHOREIRC_CONFIG_READ_OK)
    {
        relay_config_change_network_allowed_ips (NULL, NULL, NULL);
        relay_config_change_irc_backlog_tags (NULL, NULL, NULL);
    }
    return rc;
}

/*
 * Writes relay configuration file.
 */

int
relay_config_write ()
{
    return weechat_config_write (relay_config_file);
}

/*
 * Frees relay configuration.
 */

void
relay_config_free ()
{
    weechat_config_free (relay_config_file);

    if (relay_config_regex_allowed_ips)
    {
        regfree (relay_config_regex_allowed_ips);
        free (relay_config_regex_allowed_ips);
        relay_config_regex_allowed_ips = NULL;
    }

    if (relay_config_regex_websocket_allowed_origins)
    {
        regfree (relay_config_regex_websocket_allowed_origins);
        free (relay_config_regex_websocket_allowed_origins);
        relay_config_regex_websocket_allowed_origins = NULL;
    }

    if (relay_config_hashtable_irc_backlog_tags)
    {
        weechat_hashtable_free (relay_config_hashtable_irc_backlog_tags);
        relay_config_hashtable_irc_backlog_tags = NULL;
    }
}
