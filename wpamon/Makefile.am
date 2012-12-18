ACLOCAL_AMFLAGS = -I m4 -Wall

SCAFFOLD=oml2-scaffold

appname = wpamon
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = wpamon-oml2
wpamon_oml2_SOURCES = wpamon.c
wpamon_oml2_LDADD = -loml2 -locomm -lpopt

pkgdata_DATA = $(appname).rb

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(appname).rb.in

BUILT_SOURCES = $(top_srcdir)/.version \
		$(appname).rb \
		$(appname)_oml.h \
		$(appname)_popt.h

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

CLEANFILES = $(appname)_oml.h \
	     $(appname)_popt.h \
	     $(appname).rb

$(appname)_popt.h: $(appname).rb
	$(SCAFFOLD) --opts $<

$(appname)_oml.h: $(appname).rb
	$(SCAFFOLD) --oml $<

$(appname).rb: $(appname).rb.in
	VER=`echo $(VERSION) | sed "s/-[0-9a-z]\+\(-dirty\)$$//;s/-.*//;s/\(pre\|rc\)\./-/;s/[^-0-9.]//g;s/\./, /g"`; \
		sed -e 's|@bindir[@]|$(bindir)|g' \
		-e "/a\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/a\.version.*/a.version($$VER)/" \
		$< > $@

uninstall-hook:
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
