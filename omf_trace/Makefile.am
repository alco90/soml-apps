ACLOCAL_AMFLAGS = -I ../../m4 -Wnone

progname = omf_trace

bin_PROGRAMS = trace_oml2

trace_oml2_SOURCES = omf_trace.c

trace_oml2_LDADD = -loml2	-ltrace -lpopt

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
