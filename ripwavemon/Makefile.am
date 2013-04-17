ACLOCAL_AMFLAGS = -I m4 -Wall

SCAFFOLD=oml2-scaffold

appname = ripwavemon
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = ripwavemon-oml2
ripwavemon_oml2_SOURCES = ripwavemon.c
ripwavemon_oml2_LDADD = -loml2 -locomm -lpopt

pkgdata_DATA = $(appname).rb

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(appname).rb.in \
	     navini-protocol.txt

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
	VER=`echo $(VERSION) | sed "s/.*+oml//;s/\([0-9\+]\(\.[0-9]\+\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    sed -e 's|@bindir[@]|$(bindir)|g' \
		-e "/a\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/a\.version.*/a.version($$VER)/" \
		$< > $@

installed_link_names = $(DESTDIR)$(bindir)/`echo "ripwave_monitor" | sed '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_link_names); do \
		test -e $${OLDNAME} && mv $${OLDNAME} $${OLDNAME}.pre$(PACKAGE_VERSION); \
		$(LN_S) $(installed_name) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
