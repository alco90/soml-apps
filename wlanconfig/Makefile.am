ACLOCAL_AMFLAGS = -I m4

progname = wlanconfig

bin_PROGRAMS = oml2-wlanconfig

oml2_wlanconfig_SOURCES = wlanconfig_oml.c

oml2_wlanconfig_LDADD = -loml2 -locomm

installed_name = oml2-$(progname)$(EXEEXT)
installed_link_name = $(DESTDIR)$(bindir)/$(progname)_oml2$(EXEEXT)
install-exec-hook:
	rm -f $(installed_link_name)
	$(LN_S) $(installed_name) $(installed_link_name)
