ACLOCAL_AMFLAGS = -I m4 -Wall

# SED is otherwise provided by libtool.m4
SED=sed

SCAFFOLD=oml2-scaffold

appname = trace
installed_name = `echo '$(PACKAGE)' | $(SED) '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = trace-oml2
trace_oml2_SOURCES = trace.c
trace_oml2_LDADD = -loml2 -locomm -ltrace -lpopt

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
	$(SCAFFOLD) --opts $< --ontology ./etsi-ontology/

$(appname)_oml.h: $(appname).rb
	$(SCAFFOLD) --oml $< --ontology ./etsi-ontology/

$(appname).rb: $(appname).rb.in
	VER=`echo $(VERSION) | $(SED) "s/.*+oml//;s/\([0-9]\{1,\}\(\.[0-9]\{1,\}\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    $(SED) -e 's|@bindir[@]|$(bindir)|g' \
		-e "/app\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/app\.version.*/app.version($$VER)/" \
		$< > $@

uninstall-hook:
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
