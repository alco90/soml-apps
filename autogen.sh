#!/bin/sh

if [ ! -f iperf/autogen.sh -o ! -f httperf/autogen.sh ]; then
	git submodule update --init
fi

APPS=`sed -n "s/DIST_SUBDIRS * =//p" Makefile.am`
for i in $APPS; do
	echo $i
	cd $i
	./autogen.sh --quick # Don't run autoreconf just yet
	cd - >/dev/null
done

rm -rf autom4te.cache/
# Let the user know what we do, tersely
echo "gnulib-tool: updating portability library files" >&2
gnulib-tool --update --quiet --quiet >/dev/null
chmod a+x build-aux/git-version-gen
autoreconf -i $*
