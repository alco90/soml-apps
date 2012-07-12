#!/bin/bash

ver=$1

if [ "x$ver" == "x" ]; then
	echo "usage: $0 MAJOR.MINOR.REV" >&2
	exit 1
fi

echo -n "Setting version number to ${ver} in:"

conffiles=$(find . -name configure.ac)

for i in $conffiles; do
	echo -n " $i"
	sed -i "s/\(AC_INIT(\[[^]]*\], *\)\[\([0-9.]\++oml\)\?[0-9\.pre]*\]/\1[\2${ver}]/" $i
done

appdeffiles=$(find . -name *.rb.in)
for i in $appdeffiles; do
	echo -n " $i"
	sed -i "s/a\.version([^)]*)/a.version(${ver//./, })/" $i
done

echo .
