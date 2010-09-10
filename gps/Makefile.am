ACLOCAL_AMFLAGS = -I m4 -Wnone

progname = gps

bin_PROGRAMS = oml2-gps

oml2_gps_SOURCES = gps.c

oml2_gps_LDADD = -loml2 -ltrace -lpopt

BUILT_SOURCES = \
	$(progname)_popt.h \
	$(progname)_oml.h


CLEANFILES = \
	$(progname)_popt.h \
	$(progname)_oml.h

EXTRA_DIST=$(progname).app

OML2_SCAFFOLD = oml2_scaffold

$(progname)_popt.h:	$(progname).app Makefile
	$(OML2_SCAFFOLD) --opts $<

$(progname)_oml.h:	$(progname).app Makefile
	$(OML2_SCAFFOLD) --oml $<

installed_name = $(DESTDIR)$(bindir)/oml2-$(progname)$(EXEEXT)
installed_link_name = $(DESTDIR)$(bindir)/$(progname)_oml2$(EXEEXT)
install-exec-hook:
	rm -f $(installed_link_name)
	$(LN_S) $(installed_name) $(installed_link_name)
