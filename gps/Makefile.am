ACLOCAL_AMFLAGS = -I m4 -Wnone

progname = gps

bin_PROGRAMS = gps_oml2

gps_oml2_SOURCES = gps.c

gps_oml2_LDADD = -loml2 -ltrace -lpopt

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