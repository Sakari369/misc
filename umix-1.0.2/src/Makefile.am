## $Id: Makefile.am,v 1.3 2003/02/09 09:36:22 sakari Exp $
## 
## Process this file with automake to generate Makefile.in
## this file is part of the Umix package, see README for more information
## Saato rules taas. 

bin_PROGRAMS = umix
umix_SOURCES = umix_common.c umix_common.h umix.c mixer.c mixer.h \
	error.c error.h ui.c ui.h
EXTRA_umix_SOURCES = driver_oss.c driver_oss.h driver_dummy.c driver_dummy.h \
	ui_ncurses.c ui_ncurses.h
umix_LDADD = @umix_CONDOBJS@
umix_DEPENDENCIES = @umix_CONDOBJS@
