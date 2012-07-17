ACLOCAL_AMFLAGS = -Wall

appname = gpslogger
installed_name = `echo -n '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = gpslogger-oml2
gpslogger_oml2_SOURCES = gpslogger.c
gpslogger_oml2_LDADD = -loml2 -lgps -lm

pkgdata_DATA = $(appname).rb

EXTRA_DIST = $(appname).rb.in
CLEANFILES = $(appname).rb

$(appname).rb: $(appname).rb.in
	sed -e 's|@bindir[@]|$(bindir)|g' -e "/a\.path/s/$(PACKAGE)/$(installed_name)/" $< > $@

installed_link_names = \
		       $(DESTDIR)$(bindir)/`echo -n "$(appname)_oml2" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo -n "oml2-$(appname)" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo -n "omf_$(appname)" | sed '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(installed_name) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
