#!/bin/sh

ver=$1

if [ "x$ver" == "x" ]; then
	echo "Please set a version number"
fi

echo "Setting version number to ${ver} in..."

files=$(find . -name configure.ac)

for i in $files; do
	echo $i
	sed -i "s/\(AC_INIT(\[[^]]*\], *\)\[[0-9\.pre]*\]/\1[${ver}]/" $i
done

