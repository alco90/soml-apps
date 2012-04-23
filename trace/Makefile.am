ACLOCAL_AMFLAGS = -Wall

appname = trace
progname = $(appname)-oml2
prefixedname = $(shell echo $(PACKAGE) | sed '$(transform)')
pkgdatadir = $(datadir)/$(prefixedname)

# Cannot use progname here
bin_PROGRAMS = trace-oml2
trace_oml2_SOURCES = trace.c
trace_oml2_LDADD = -loml2 -ltrace -lpopt

pkgdata_DATA = $(appname).rb

BUILT_SOURCES = \
	$(appname).rb \
	$(appname)_popt.h \
	$(appname)_oml.h

CLEANFILES = \
	$(pkgdata_DATA)
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST=$(appname).rb.in

$(appname)_popt.h: $(appname).rb Makefile
	oml2-scaffold --opts $<

$(appname)_oml.h: $(appname).rb Makefile
	oml2-scaffold --oml $<

$(appname).rb: $(appname).rb.in
	sed -e 's|@bindir[@]|$(bindir)|g' -e '/a\.path/s/$(PACKAGE)/$(prefixedname)/' $< > $@

installed_link_names = $(DESTDIR)$(bindir)/$(shell echo $(appname)_oml2 | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo oml2-$(appname) | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo omf_$(appname) | sed '$(transform)')$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_names)
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(prefixedname) $${OLDNAME}; \
	done
