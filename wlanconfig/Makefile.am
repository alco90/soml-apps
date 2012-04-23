ACLOCAL_AMFLAGS = -Wall

appname = wlanconfig
progname = $(appname)-oml2
prefixedname = $(shell echo $(PACKAGE) | sed '$(transform)')
pkgdatadir = $(datadir)/$(prefixedname)

# Cannot use progname here
bin_PROGRAMS = wlanconfig-oml2
wlanconfig_oml2_SOURCES = wlanconfig.c
wlanconfig_oml2_LDADD = -loml2 -locomm

installed_link_names = $(DESTDIR)$(bindir)/$(shell echo $(appname)_oml2 | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo oml2-$(appname) | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo omf_$(appname) | sed '$(transform)')$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_names)
	for OLDNAME in $(installed_link_names); do \
		$(LN_S) $(prefixedname) $${OLDNAME}; \
	done
