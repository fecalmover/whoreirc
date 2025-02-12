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

#ifndef WHOREIRC_EVAL_H
#define WHOREIRC_EVAL_H

#include <regex.h>

#define EVAL_STR_FALSE      "0"
#define EVAL_STR_TRUE       "1"

#define EVAL_DEFAULT_PREFIX "${"
#define EVAL_DEFAULT_SUFFIX "}"

#define EVAL_RECURSION_MAX  32

struct t_hashtable;

enum t_eval_logical_op
{
    EVAL_LOGICAL_OP_OR = 0,
    EVAL_LOGICAL_OP_AND,
    /* number of comparison strings */
    EVAL_NUM_LOGICAL_OPS,
};

enum t_eval_comparison
{
    EVAL_COMPARE_REGEX_MATCHING = 0,
    EVAL_COMPARE_REGEX_NOT_MATCHING,
    EVAL_COMPARE_STRING_MATCHING,
    EVAL_COMPARE_STRING_NOT_MATCHING,
    EVAL_COMPARE_EQUAL,
    EVAL_COMPARE_NOT_EQUAL,
    EVAL_COMPARE_LESS_EQUAL,
    EVAL_COMPARE_LESS,
    EVAL_COMPARE_GREATER_EQUAL,
    EVAL_COMPARE_GREATER,
    /* number of comparison strings */
    EVAL_NUM_COMPARISONS,
};

struct t_eval_regex
{
    const char *result;
    regmatch_t match[100];
    int last_match;
};

struct t_eval_context
{
    struct t_hashtable *pointers;
    struct t_hashtable *extra_vars;
    int extra_vars_eval;
    const char *prefix;
    const char *suffix;
    struct t_eval_regex *regex;
    int recursion_count;
};

extern int eval_is_true (const char *value);
extern char *eval_expression (const char *expr,
                              struct t_hashtable *pointers,
                              struct t_hashtable *extra_vars,
                              struct t_hashtable *options);

#endif /* WHOREIRC_EVAL_H */
