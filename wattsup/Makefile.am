ACLOCAL_AMFLAGS = -I m4 -Wall

# SED is otherwise provided by libtool.m4
SED=sed

SCAFFOLD=oml2-scaffold

appname = wattsup
installed_name = `echo '$(PACKAGE)' | $(SED) '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = wattsup-oml2
wattsup_oml2_SOURCES = wattsup.c \
		       $(appname)_oml.h
wattsup_oml2_LDADD = -loml2 -locomm

pkgdata_DATA = $(appname).rb

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(appname).rb.in

BUILT_SOURCES = $(top_srcdir)/.version \
		$(appname).rb \
		$(appname)_oml.h

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

CLEANFILES = $(appname)_oml.h \
	     $(appname).rb

$(appname)_oml.h: $(appname).rb
	$(SCAFFOLD) --oml $<

$(appname).rb: $(appname).rb.in
	VER=`echo $(VERSION) | $(SED) "s/.*+oml//;s/\([0-9]\{1,\}\(\.[0-9]\{1,\}\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    $(SED) -e 's|@bindir[@]|$(bindir)|g' \
		-e "/app\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/app\.version.*/app.version($$VER)/" \
		$< > $@

uninstall-hook:
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
