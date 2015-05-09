/* 
 * $Id: driver_oss.h,v 1.1 2003/02/09 09:36:22 sakari Exp $
 *
 * The OSS mixer driver for Umix.
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

#ifndef UMIX_DRIVER_OSS_H
#define UMIX_DRIVER_OSS_H

umixer_func *oss_get_mixer_func();

#endif /* UMIX_DRIVER_OSS_H */
