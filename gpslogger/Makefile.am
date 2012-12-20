ACLOCAL_AMFLAGS = -I m4 -Wall

SCAFFOLD=oml2-scaffold

appname = gpslogger
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = gpslogger-oml2
gpslogger_oml2_SOURCES = gpslogger.c
gpslogger_oml2_LDADD = -loml2 -lgps -lm

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
	VER=`echo $(VERSION) | sed "s/.*+oml//;s/\([0-9\+]\(\.[0-9]\+\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    sed -e 's|@bindir[@]|$(bindir)|g' \
		-e "/a\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/a\.version.*/a.version($$VER)/" \
		$< > $@

installed_link_names = \
		       $(DESTDIR)$(bindir)/`echo "$(appname)_oml2" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo "oml2-$(appname)" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo "omf_$(appname)" | sed '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_link_names); do \
		test -e $${OLDNAME} && mv $${OLDNAME} $${OLDNAME}.pre$(PACKAGE_VERSION); \
		$(LN_S) $(installed_name) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
