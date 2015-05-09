/*
 * $Id: umix_common.h,v 1.4 2003/02/09 09:36:22 sakari Exp $
 *
 * Common stuff for Umix.
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

#ifndef UMIX_COMMON_H
#define UMIX_COMMON_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/* common includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mixer.h"
#include "error.h"

/* internationalization */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  define _(str) gettext (str)
#else
#  define _(str) str
#endif

/* default settings */
#define DEF_CONFIG_PATH	DEF_CONFIG_DIR"/umixrc"
#define DEF_UI		"ncurses"
#define DEF_DRIVER	"OSS"
#define NAME_STR	"Umix"
#define UMIX_VERSION_STR NAME_STR " " VERSION

/* boundary checking macros */
#undef  MAX
#define MAX(a, b)	(((a) > (b)) ? (a) : (b))
#undef  MIN
#define MIN(a, b)	(((a) < (b)) ? (a) : (b))
#undef  CLAMP
#define CLAMP(x, low, high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))      
#undef	ABS
#define	ABS(a)		(((a) < 0) ? -(a) : (a))
#undef NELEMS
#define NELEMS(a) (sizeof(a)/sizeof(a[0]))

/* string lengths */
enum
{
	TINYSTRLEN	= 32,
	LINEWIDTH	= 80,
	STRLEN		= 128,
	BUFLEN		= 256
};

/* settings loading errors */
enum 
{
	UMIX_ESETERR		= -1, /* general settings error */
	UMIX_ESETINVALID	= 1,  /* invalid config file format */
	UMIX_ESETNOTFOUND	= 2,  /* mixer settings not found */
	UMIX_ESETBINARY		= 3   /* binary file detected */
};

/* channel parsing errors */
enum
{
	UMIX_EOPTERR		= -1, /* general settings error */
	UMIX_EOPTLIMIT		= 1,  /* limit error */
	UMIX_EOPTINVALID	= 2,  /* invalid value */
	UMIX_EOPTNOTREC		= 3   /* cannot change record value */
};

/* types of settings to load */
enum
{
	UMIX_SET_GLOBAL, /* global settings */
	UMIX_SET_MIXER   /* mixer settings */
};

/* the global config structure */
typedef struct umix_cfg umix_cfg;
struct umix_cfg
{
	char devpath[STRLEN];
	char cfgpath[STRLEN];
	char driver[TINYSTRLEN];
};

/* the global option structure, declared in umix.c */
extern umix_cfg globalopts;
	
/* common function prototypes */
int settings_load(const char *path, int mode);
int settings_save(const char *path);
int parsechanopt(const char *optarg);

#endif /* UMIX_COMMON_H */
