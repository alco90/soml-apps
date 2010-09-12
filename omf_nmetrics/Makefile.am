ACLOCAL_AMFLAGS = -I m4 -Wnone

progname = nmetrics
appname = omf_nmetrics

bin_PROGRAMS = oml2-nmetrics
oml2_nmetrics_SOURCES = omf_nmetrics.c
oml2_nmetrics_LDADD = -loml2 -locomm -lsigar -lpopt
oml2_nmetrics_CPPFLAGS = $(LIBSIGAR_CFLAGS)

BUILT_SOURCES = \
	$(appname)_popt.h \
	$(appname)_oml.h

CLEANFILES = \
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST=$(appname).app

$(appname)_popt.h:	$(appname).app Makefile
	oml2_scaffold --opts $<

$(appname)_oml.h:	$(appname).app Makefile
	oml2_scaffold --oml $<

installed_name = oml2-$(progname)$(EXEEXT)
installed_link_name = $(DESTDIR)$(bindir)/$(progname)_oml2$(EXEEXT)
install-exec-hook:
	rm -f $(installed_link_name)
	$(LN_S) $(installed_name) $(installed_link_name)
