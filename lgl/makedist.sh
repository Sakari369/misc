#!/usr/bin/env bash

dirname="lgl-dist"
builddir="/tmp"
mkdir $builddir/$dirname
cp -R * $builddir/$dirname
cd $builddir/$dirname
make clean
rm -rf *~
cd $builddir
date=`date +%d%m%y`
filename="lgl-$date.tar.gz"
tar zcvf $filename $dirname
echo "Dist package is completed at $builddir/$filename"
