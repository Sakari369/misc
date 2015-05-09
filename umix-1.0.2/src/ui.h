/*
 * $Id: ui.h,v 1.4 2003/02/09 09:36:22 sakari Exp $
 *
 * Umix user interface headers.
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

#ifndef UMIX_UI_H
#define UMIX_UI_H

/* General error strings, defined here so we can use the same strings 
 * easily in different UI`s */

/* These are used in different UI`s when saving / loading settings */
/* load strings */
#define UMIX_UI_FAILED_DEVICE_STR _("Failed initializing mixer devices")
#define UMIX_UI_EINVALID_STR _("Invalid config file")
#define UMIX_UI_ENOTFOUND_STR _("Volumes for current mixer not found in config")
#define UMIX_UI_ESETBINARY_STR _("Binary config file detected, not loading")
#define UMIX_UI_LOADOK_STRFMT _("Settings loaded from %s")
/* save strings */
#define UMIX_UI_SAVEFAIL_STRFMT _("Failed saving %s: %s")
#define UMIX_UI_SAVEOK_STRFMT _("Settings saved to %s")
/* set config path strings */
#define UMIX_UI_CFGSET_STRFMT _("Config path set to \"%s\"")
#define UMIX_UI_CFGNOTSET_STRFMT _("Config path \"%s\" not changed")

/* These are used in places where channel options are parsed */
/* channel option parsing error strings */
#define UMIX_EOPTGENERAL_STRFMT _("invalid value for channel %s: %s")
#define UMIX_EOPTLIMIT_STR _("value over limits")
#define UMIX_EOPTINVALID_STR _("unknown characters")
#define UMIX_EOPTNOTREC_STR _("channel cannot be a recording source")

/* Misc UI strings used all over the place */
#define UMIX_UI_MUTE _("Mute")
#define UMIX_UI_REC _("Rec")

typedef struct ui_func ui_func;
struct ui_func
{
	int (*ui_main)(int argc, char *argv[]);
	void (*ui_deinit)(void);
};

int ui_init(const char *uiname, int argc, char *argv[]);

#endif /* UMIX_UI_H */
