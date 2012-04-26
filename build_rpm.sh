#!/bin/bash

# make sure you install the build dependencies from the .spec file
# before using this script

# If this is a Git repo, git-archive the right tag, and do some sanity check
if [ -d .git ]; then
	./fetch-source-tree.rb
fi

autoreconf --install
./configure
make dist

package=oml2-apps
version=`grep AC_INIT configure.ac |  sed -n -e 's/[^[]*\[\([^]]*\)][^[]*\[\([^]]*\)].*/\2/p'`
tmp=/tmp/$package-rpmbuild

if [ -d $tmp ]; then rm -rf $tmp; fi

mkdir -p $tmp
cd $tmp
mkdir -p BUILD RPMS SOURCES SPECS SRPMS
cd -
ln -s `pwd`/$package-$version.tar.gz $tmp/SOURCES/
rpmbuild -v -bb --clean $package.spec --define "_topdir $tmp" --nodeps
find $tmp -type f -name "*.rpm" -exec cp -v {} . \;
rm -rf $tmp

