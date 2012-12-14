ACLOCAL_AMFLAGS = -I m4 -Wall

appname = ping
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_SCRIPTS = ping-oml2

pkgdata_DATA = $(appname).rb

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(appname).rb.in \
	     $(appname).rb \
	     $(appname)-oml2.in \
	     $(appname)-oml2

BUILT_SOURCES = $(top_srcdir)/.version \
		$(appname).rb

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

CLEANFILES = $(appname)-oml2

MAINTAINERCLEANFILES = $(appname).rb

$(appname).rb: $(appname).rb.in
	VER=`echo $(VERSION) | sed "s/-[0-9a-z]\+\(-dirty\)$$//;s/-.*//;s/\(pre\|rc\)\./-/;s/[^-0-9.]//g;s/\./, /g"`; \
		sed -e 's|@bindir[@]|$(bindir)|g' \
		-e "/a\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/a\.version.*/a.version($$VER)/" \
		$< > $@

uninstall-hook:
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
