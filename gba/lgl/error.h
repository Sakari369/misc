/* 
 * $Id: error.h,v 1.3 2002/12/22 15:18:57 sakari Exp $
 *
 * Error and wrapper function headers.
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

#ifndef MY_ERROR_H
#define MY_ERROR_H

int eioctl(int fd, int request, void *arg);
void *emalloc(size_t size);
void *erealloc(void *ptr, size_t size);
char *estrdup(const char *str);
void eclose(int fd);
int eopen(const char *pathname, int flags, mode_t mode);
	
void err_msg(const char *fmt, ...);
void err_str(const char *fmt, ...);
void err_quit(const char *fmt, ...);

/* the name program was called with */
extern const char *program_name;

#endif /* MY_ERROR_H */
