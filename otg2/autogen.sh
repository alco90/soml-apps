#!/bin/sh
rm -rf autom4te.cache/
gnulib-tool --update --quiet --quiet
chmod a+x build-aux/git-version-gen
if [ x$1 != x--quick ]; then
	autoreconf -i $*
fi
