CLOCAL_AMFLAGS = -Wall

appname = ripwavemon
installed_name = `echo -n '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = ripwavemon-oml2
ripwavemon_oml2_SOURCES = ripwavemon.c
ripwavemon_oml2_LDADD = -loml2 -lpopt

pkgdata_DATA = $(appname).rb

BUILT_SOURCES = \
	$(appname).rb \
	$(appname)_popt.h \
	$(appname)_oml.h

CLEANFILES = \
	$(pkgdata_DATA) \
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST=$(appname).rb.in \
	   navini-protocol.txt

$(appname)_popt.h: $(appname).rb Makefile
	oml2-scaffold --opts $<

$(appname)_oml.h: $(appname).rb Makefile
	oml2-scaffold --oml $<

$(appname).rb: $(appname).rb.in
	sed -e 's|@bindir[@]|$(bindir)|g' -e "/a\.path/s/$(PACKAGE)/$(installed_name)/" $< > $@

installed_link_names = $(DESTDIR)$(bindir)/`echo -n "ripwave_monitor" | sed '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(installed_name) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
