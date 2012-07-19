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
verruby=$ver
if echo $verruby | grep -q "[^0-9.]"; then
	# Strip any character, replace them by a minus sign
	verruby=`echo $verruby | sed "s/[^.0-9]\+/-/g"`
fi
for i in $appdeffiles; do
	echo -n " $i"
	sed -i "s/a\.version([^)]*)/a.version(${verruby//./, })/" $i
done

echo .

echo Do not forget to commit changes in iperf too:
echo " (cd iperf; git commit -a -s -m 'Bump to oml2-apps-${ver}'); git add iperf"
