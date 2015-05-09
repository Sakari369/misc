#!/bin/sh
#
# $Id: autogen.sh,v 1.4 2002/11/16 15:39:34 sakari Exp $
#
# Generate build information.
#
# Copyright (C) 2002 Sakari Lehtonen <sakari@ionstream.fi>

echo "Generating build information, this may take a while .."

echo "Running libtoolize, ignore non-fatal messages .."
libtoolize --copy --force || exit

# Regenerate configuration files
echo -n "Running aclocal, autoheader, automake and autoconf "
aclocal || exit
echo -n "TOO .."
autoheader || exit
echo -n "DAMN .."
automake --add-missing --copy 
echo -n "SLOW .."
autoconf || exit
echo "... done"

# Run configure for this platform
#./configure $*
echo "Now you are ready to run ./configure"
