ACLOCAL_AMFLAGS = -I m4

progname = gpslogger

bin_PROGRAMS = oml2-gpslogger

oml2_gpslogger_SOURCES = gpslogger.c

oml2_gpslogger_LDADD = -loml2 -lgps

installed_name = oml2-$(progname)$(EXEEXT)
installed_link_name = $(DESTDIR)$(bindir)/$(progname)_oml2$(EXEEXT)
install-exec-hook:
	rm -f $(installed_link_name)
	$(LN_S) $(installed_name) $(installed_link_name)
