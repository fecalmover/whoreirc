/*
 * test-core-hook.cpp - test hook functions
 *
 * Copyright (C) 2018-2019 Sébastien Helleu <flashcode@flashtux.org>
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

#include "CppUTest/TestHarness.h"

extern "C"
{
#include <string.h>
#include "src/core/wee-hook.h"
#include "src/core/wee-string.h"
#include "src/gui/gui-buffer.h"
#include "src/gui/gui-chat.h"
#include "src/gui/gui-line.h"
#include "src/plugins/plugin.h"
}

#define TEST_BUFFER_NAME "test"

TEST_GROUP(CoreHook)
{
};

/*
 * Tests functions:
 *   hook_command_run
 */

TEST(CoreHook, CommandRun)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_command
 */

TEST(CoreHook, Command)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_completion
 */

TEST(CoreHook, Completion)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_config
 */

TEST(CoreHook, Config)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_connect
 */

TEST(CoreHook, Connect)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_fd
 */

TEST(CoreHook, Fd)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_focus
 */

TEST(CoreHook, Focus)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_hdata
 */

TEST(CoreHook, Hdata)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_hsignal
 */

TEST(CoreHook, Hsignal)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_info_hashtable
 */

TEST(CoreHook, InfoHashtable)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_info
 */

TEST(CoreHook, Info)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_infolist
 */

TEST(CoreHook, Infolist)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_line
 */

TEST(CoreHook, Line)
{
    /* TODO: write tests */
}

char *
test_modifier_cb (const void *pointer, void *data,
                  const char *modifier, const char *modifier_data,
                  const char *string)
{
    char **items, *new_string;
    const char *ptr_plugin, *ptr_name, *ptr_tags, *ptr_msg;
    int num_items, length;

    /* make C++ compiler happy */
    (void) pointer;
    (void) data;
    (void) modifier;

    /* split modifier_data, which is: "plugin;name;tags" */
    items = string_split (modifier_data, ";", NULL,
                          WHOREIRC_STRING_SPLIT_STRIP_LEFT
                          | WHOREIRC_STRING_SPLIT_STRIP_RIGHT
                          | WHOREIRC_STRING_SPLIT_COLLAPSE_SEPS,
                          3, &num_items);
    if (num_items < 2)
        return NULL;

    ptr_plugin = items[0];
    ptr_name = items[1];
    ptr_tags = (num_items > 2) ? items[2] : NULL;

    /* do nothing on a buffer different from "core.test" */
    if ((strcmp (ptr_plugin, "core") != 0)
        || (strcmp (ptr_name, TEST_BUFFER_NAME) != 0))
    {
        return NULL;
    }

    if (strncmp (string, "\t\t", 2) == 0)
    {
        ptr_msg = string + 2;
    }
    else
    {
        ptr_msg = strchr (string, '\t');
        if (!ptr_msg)
            return NULL;
        ptr_msg++;
    }

    length = strlen (string) + 128;
    new_string = (char *)malloc (length);
    if (!new_string)
        return NULL;
    new_string[0] = '\0';

    if (ptr_tags && strstr (ptr_tags, "add_prefix"))
    {
        /* add a prefix in message */
        snprintf (new_string, length, "new prefix\t%s (modified)", ptr_msg);
    }
    else if (ptr_tags && strstr (ptr_tags, "add_date_prefix"))
    {
        /* add a date/prefix in message */
        snprintf (new_string, length, "new prefix\t%s (modified)", ptr_msg);
    }
    else if (ptr_tags && strstr (ptr_tags, "update_prefix"))
    {
        /* update the prefix */
        snprintf (new_string, length, "new prefix\t%s (modified)", ptr_msg);
    }
    else if (ptr_tags && strstr (ptr_tags, "remove_prefix"))
    {
        /* remove the prefix */
        snprintf (new_string, length, " \t%s (modified)", ptr_msg);
    }
    else if (ptr_tags && strstr (ptr_tags, "remove_date_prefix"))
    {
        /* remove the date/prefix */
        snprintf (new_string, length, "\t\t%s (modified)", ptr_msg);
    }

    if (!new_string[0])
    {
        /* default message returned: just add " (modified)" to the string */
        snprintf (new_string, length, "%s (modified)", string);
    }

    return new_string;
}

/*
 * Tests functions:
 *   hook_modifier
 */

TEST(CoreHook, Modifier)
{
    struct t_gui_buffer *test_buffer;
    struct t_gui_line *ptr_line;
    struct t_hook *hook;

    /* create/open a test buffer */
    test_buffer = gui_buffer_new (NULL, TEST_BUFFER_NAME,
                                  NULL, NULL, NULL,
                                  NULL, NULL, NULL);
    CHECK(test_buffer);

    hook = hook_modifier (NULL, "weechat_print", &test_modifier_cb, NULL, NULL);

    /* check hook contents */
    CHECK(hook);
    POINTERS_EQUAL(NULL, hook->plugin);
    POINTERS_EQUAL(NULL, hook->subplugin);
    LONGS_EQUAL(HOOK_TYPE_MODIFIER, hook->type);
    LONGS_EQUAL(0, hook->deleted);
    LONGS_EQUAL(0, hook->running);
    LONGS_EQUAL(HOOK_PRIORITY_DEFAULT, hook->priority);
    POINTERS_EQUAL(NULL, hook->callback_pointer);
    POINTERS_EQUAL(NULL, hook->callback_data);
    CHECK(hook->hook_data);
    POINTERS_EQUAL(&test_modifier_cb, HOOK_MODIFIER(hook, callback));
    STRCMP_EQUAL("weechat_print", HOOK_MODIFIER(hook, modifier));

    /* message without prefix: unchanged */
    gui_chat_printf_date_tags (test_buffer, 0, NULL, " \tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* message without prefix: add a prefix */
    gui_chat_printf_date_tags (test_buffer, 0, "add_prefix", " \tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("new prefix", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* message without date: unchanged */
    gui_chat_printf_date_tags (test_buffer, 0, NULL, "\t\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    LONGS_EQUAL(0, ptr_line->data->date);
    POINTERS_EQUAL(NULL, ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* message without date: add a date/prefix */
    gui_chat_printf_date_tags (test_buffer, 0, "add_date_prefix",
                               "\t\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("new prefix", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* standard message: unchanged */
    gui_chat_printf_date_tags (test_buffer, 0, NULL, "prefix\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("prefix", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* standard message: update the prefix */
    gui_chat_printf_date_tags (test_buffer, 0, "update_prefix",
                               "prefix\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("new prefix", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* standard message: remove the prefix */
    gui_chat_printf_date_tags (test_buffer, 0, "remove_prefix",
                               "prefix\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    CHECK(ptr_line->data->date > 0);
    STRCMP_EQUAL("", ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* standard message: remove the date/prefix */
    gui_chat_printf_date_tags (test_buffer, 0, "remove_date_prefix",
                               "prefix\tmessage");
    ptr_line = test_buffer->own_lines->last_line;
    LONGS_EQUAL(0, ptr_line->data->date);
    POINTERS_EQUAL(NULL, ptr_line->data->prefix);
    STRCMP_EQUAL("message (modified)", ptr_line->data->message);

    /* close the test buffer */
    gui_buffer_close (test_buffer);
}

/*
 * Tests functions:
 *   hook_print
 */

TEST(CoreHook, Print)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_process
 *   hook_process_hashtable
 */

TEST(CoreHook, Process)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_signal
 */

TEST(CoreHook, Signal)
{
    /* TODO: write tests */
}

/*
 * Tests functions:
 *   hook_timer
 */

TEST(CoreHook, Timer)
{
    /* TODO: write tests */
}
