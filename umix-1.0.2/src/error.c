/* 
 * $Id: error.c,v 1.5 2003/08/23 12:37:42 sakari Exp $
 *
 * Error and wrapper functions.
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
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "error.h"

/* internal error handler */
static void error_handler(int strflag, const char *fmt, va_list ap)
{
	int n;
	int errno_save;
	char buf[256];
	
	errno_save = errno;

	fprintf(stderr, "%s: ", program_name);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	n = strlen(buf);
	/* only print errno if it is specified and not 0 */
	if (strflag == 1 && errno_save != 0)
		snprintf(buf+n, sizeof(buf) - n, ": %s", strerror(errno_save));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(stderr);
}

int eioctl(int fd, int request, void *arg)
{
	int n;

	if ((n = ioctl(fd, request, arg)) == -1)
		err_msg("ioctl request %d failed: %s", request, 
			strerror(errno));
		
	return n;
}

void *emalloc(size_t size)
{
	void *ptr;

	if ((ptr = malloc(size)) == NULL)
		err_quit("malloc of %d bytes failed", size);

	return ptr;
}

void *erealloc(void *ptr, size_t size)
{
	void *reptr;

	if ((reptr = realloc(ptr, size)) == NULL)
		err_quit("realloc of %d bytes failed", size);

	return reptr;
}

char *estrdup(const char *str)
{
	char *ptr;

	if ((ptr = strdup(str)) == NULL)
		err_quit("strdup \"%s\" failed");

	return ptr;
}

void eclose(int fd)
{
	if (close(fd) == -1)
		err_quit("close of fd %d failed", fd);
}

int eopen(const char *path, int flags, mode_t mode)
{
	int fd;

	if ((fd = open(path, flags, mode)) == -1)
		fd = -1;

	return fd;
}

/* nonfatal, print message and return */
void err_msg(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	error_handler(0, fmt, ap);
	va_end(ap);
}

/* nonfatal, print message with strerror(errno) and return */
void err_str(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	error_handler(1, fmt, ap);
	va_end(ap);
}

/* fatal, print message and exit */
void err_quit(const char *fmt, ...)
{
	va_list ap;
	
	va_start(ap, fmt);
	error_handler(0, fmt, ap);
	va_end(ap);

	exit(EXIT_FAILURE);
}
