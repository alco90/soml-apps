ACLOCAL_AMFLAGS = -I ../m4 -Wnone

SUBDIRS = core port generator

AM_CPPFLAGS = \
	-I $(srcdir) \
	-DWITH_OML

otgrlibs = -lpopt -lpthread -loml2 -locomm \
	libotg2.la

bin_PROGRAMS = otg2 otr2

otg2_SOURCES = \
	version.h \
	otg2.cpp \
	otg2/address.h \
	otg2/application.h \
	otg2/component.h \
	otg2/generator.h \
	otg2/otg2_app.h \
	otg2/otr2_app.h \
	otg2/packet.h \
	otg2/port.h \
	otg2/sender.h \
	otg2/source.h \
	otg2/stream.h \
	otg2/unixtime.h


otr2_SOURCES = otr2.cpp

otg2_LDADD = $(otgrlibs)
otr2_LDADD = $(otgrlibs)

lib_LTLIBRARIES = libotg2.la 

libotg2_la_SOURCES = \
	app/application.cpp \
	app/otg2_app.cpp \
	app/otr2_app.cpp


libotg2_la_LIBADD = -locomm \
	$(builddir)/core/libotg2core.la \
	$(builddir)/port/libotg2port.la \
	$(builddir)/generator/libotg2generator.la



