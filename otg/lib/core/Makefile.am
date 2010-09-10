ACLOCAL_AMFLAGS = -I ../../m4 -Wnone

AM_CPPFLAGS = \
	-I $(srcdir)/.. \
	-DWITH_OML

noinst_LTLIBRARIES = libotg2core.la

libotg2core_la_SOURCES = \
	component.cpp \
	address.cpp \
	flow.cpp \
	packet.cpp \
	stream.cpp \
	unixtime.cpp

libotg2core_la_LIBADD = -locomm
