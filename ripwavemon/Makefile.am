CLOCAL_AMFLAGS = -Wall

appname = ripwavemon
progname = $(appname)-oml2
prefixedname = $(shell echo $(PACKAGE) | sed '$(transform)')
pkgdatadir = $(datadir)/$(prefixedname)

# Cannot use progname here
bin_PROGRAMS = ripwavemon-oml2
ripwavemon_oml2_SOURCES = ripwavemon.c
ripwavemon_oml2_LDADD = -loml2 -lpopt

pkgdata_DATA = $(appname).rb

BUILT_SOURCES = \
	$(appname).rb \
	$(appname)_oml.h

CLEANFILES = \
	$(pkgdata_DATA)
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST=$(appname).rb.in \
	   navini-protocol.txt

$(appname)_oml.h: $(appname).rb Makefile
	oml2-scaffold --oml $<

$(appname).rb: $(appname).rb.in
	sed -e 's|@bindir[@]|$(bindir)|g' -e '/a\.path/s/$(PACKAGE)/$(prefixedname)/' $< > $@

installed_link_names = $(DESTDIR)$(bindir)/$(shell echo ripwave_monitor | sed '$(transform)')$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_names)
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(prefixedname) $${OLDNAME}; \
	done
