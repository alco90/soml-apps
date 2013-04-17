ACLOCAL_AMFLAGS = -I ../m4 -Wnone

AM_CPPFLAGS = \
	-I $(srcdir)/.. \
	-I $(top_srcdir) \
	-I $(top_builddir)

noinst_LTLIBRARIES = libotg2port.la

libotg2port_la_SOURCES = \
	socket.cpp \
	factory.cpp \
	udp_outport.cpp \
	udp_inport.cpp \
	null_outport.cpp \
	socket.h \
	udp_outport.h \
	udp_inport.h \
	null_outport.h

libotg2port_la_LIBADD = -locomm

