ACLOCAL_AMFLAGS = -I ../m4 -Wnone

bin_PROGRAMS = nmetrics_oml2

nmetrics_oml2_SOURCES = \
	src/c/nmetrics.c \
	src/c/nmetrics_oml.h \
	src/c/nmetrics_popt.h \
	src/c/o_log.h

nmetrics_oml2_LDADD = -loml2 -locomm -lsigar -lpopt
