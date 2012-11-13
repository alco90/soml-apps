#!/bin/sh

if [ ! -f iperf/autogen.sh ]; then
	git submodule update --init
fi

APPS=`sed -n "s/SUBDIRS * =//p" Makefile.am`
for i in $APPS; do
	cd $i
	./autogen.sh --quick # Don't run autoreconf just yet
	cd -
done

rm -rf autom4te.cache/
gnulib-tool --update --quiet --quiet
chmod a+x build-aux/git-version-gen
autoreconf -i $*
