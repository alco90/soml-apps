ACLOCAL_AMFLAGS = -Wall

appname = wlanconfig
installed_name = `echo -n '$(PACKAGE)' | sed '$(transform)'`

bin_PROGRAMS = wlanconfig-oml2
wlanconfig_oml2_SOURCES = wlanconfig.c
wlanconfig_oml2_LDADD = -loml2 -locomm

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
