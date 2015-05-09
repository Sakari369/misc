/*
 * $Id: ui.c,v 1.4 2003/02/09 09:36:22 sakari Exp $
 *
 * Tries to load the wanted UI and start it. When initializing UIs first
 * calls get_ui_func() to get the function pointers from the plugin.
 * After this is done, calls the resolved ui_main(). Now the UI takes over.
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

#include "umix_common.h"
#include "ui.h"
#ifdef HAVE_NCURSES
  #include "ui_ncurses.h"
#endif

int ui_init(const char *uiname, int argc, char *argv[])
{
	int retval;
	ui_func *ui_func_ptr;

	/* get all the ui function pointers */
#ifdef HAVE_NCURSES
	if( strcasecmp(uiname, "ncurses") == 0 )
		ui_func_ptr = ncurses_get_ui_func();
	else
		return -1;
#else
	return -1;
#endif
	/* ui_deinit must be registered before starting the ui */
	atexit(ui_func_ptr->ui_deinit);
	/* the user interface takes control from here */
	retval = ui_func_ptr->ui_main(argc, argv);
	
	return retval;
}
