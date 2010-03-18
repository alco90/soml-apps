ACLOCAL_AMFLAGS = -I ../m4 -Wnone

progname = omf_nmetrics

bin_PROGRAMS = nmetrics_oml2
nmetrics_oml2_SOURCES = omf_nmetrics.c
nmetrics_oml2_LDADD = -loml2 -locomm -lsigar -lpopt
nmetrics_oml2_CPPFLAGS = $(LIBSIGAR_CFLAGS)

BUILT_SOURCES = \
	$(progname)_popt.h \
	$(progname)_oml.h

CLEANFILES = \
	$(progname)_popt.h \
	$(progname)_oml.h

EXTRA_DIST=$(progname).app

$(progname)_popt.h:	$(progname).app Makefile
	oml2_scaffold --opts $<

$(progname)_oml.h:	$(progname).app Makefile
	oml2_scaffold --oml $<
