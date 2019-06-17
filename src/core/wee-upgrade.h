/*
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

#ifndef WHOREIRC_UPGRADE_H
#define WHOREIRC_UPGRADE_H

#include "wee-upgrade-file.h"

#define WHOREIRC_UPGRADE_FILENAME "weechat"

/* For developers: please add new values ONLY AT THE END of enums */

enum t_upgrade_weechat_type
{
    UPGRADE_WHOREIRC_TYPE_HISTORY = 0,
    UPGRADE_WHOREIRC_TYPE_BUFFER,
    UPGRADE_WHOREIRC_TYPE_NICKLIST,
    UPGRADE_WHOREIRC_TYPE_BUFFER_LINE,
    UPGRADE_WHOREIRC_TYPE_MISC,
    UPGRADE_WHOREIRC_TYPE_HOTLIST,
    UPGRADE_WHOREIRC_TYPE_LAYOUT_WINDOW,
};

int upgrade_weechat_save ();
int upgrade_weechat_load ();
void upgrade_weechat_end ();

#endif /* WHOREIRC_UPGRADE_H */
