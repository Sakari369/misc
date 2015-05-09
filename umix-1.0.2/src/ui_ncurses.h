/*
 * $Id: ui_ncurses.h,v 1.1 2003/02/09 09:36:22 sakari Exp $
 *
 * Ncurses user interface for Umix. Probably could use a total rewrite. But
 * hey, this works for me.
 *
 * Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef UMIX_UI_NCURSES_H
#define UMIX_UI_NCURSES_H

#include "ui.h"

ui_func *ncurses_get_ui_func(void);

#endif /* UMIX_UI_NCURSES_H */
