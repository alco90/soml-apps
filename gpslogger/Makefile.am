ACLOCAL_AMFLAGS = -Wall

appname = gpslogger
progname = $(appname)-oml2
prefixedname = $(shell echo $(PACKAGE) | sed '$(transform)')
pkgdatadir = $(datadir)/$(prefixedname)

# Cannot use progname here
bin_PROGRAMS = gpslogger-oml2
gpslogger_oml2_SOURCES = gpslogger.c
gpslogger_oml2_LDADD = -loml2 -lgps -lm

installed_link_names = $(DESTDIR)$(bindir)/$(shell echo $(appname)_oml2 | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo oml2-$(appname) | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo omf_$(appname) | sed '$(transform)')$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_names)
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(prefixedname) $${OLDNAME}; \
	done
