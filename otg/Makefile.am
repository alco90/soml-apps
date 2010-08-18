ACLOCAL_AMFLAGS = -I ../m4 -Wnone

SUBDIRS = src/cpp/core src/cpp/port src/cpp/generator

AM_CPPFLAGS = \
	-I $(srcdir)/src/cpp \
	-DWITH_OML

otgrlibs = -lpthread \
	libotg2.la

bin_PROGRAMS = otg2 otr2

otg2_SOURCES = \
	src/cpp/version.h \
	src/cpp/otg2.cpp \
	src/cpp/otg2/address.h \
	src/cpp/otg2/application.h \
	src/cpp/otg2/component.h \
	src/cpp/otg2/generator.h \
	src/cpp/otg2/otg2_app.h \
	src/cpp/otg2/otr2_app.h \
	src/cpp/otg2/packet.h \
	src/cpp/otg2/port.h \
	src/cpp/otg2/sender.h \
	src/cpp/otg2/source.h \
	src/cpp/otg2/stream.h \
	src/cpp/otg2/unixtime.h


otr2_SOURCES = src/cpp/otr2.cpp

otg2_LDADD = $(otgrlibs)
otr2_LDADD = $(otgrlibs)

lib_LTLIBRARIES = libotg2.la 

libotg2_la_SOURCES = \
	src/cpp/app/application.cpp \
	src/cpp/app/otg2_app.cpp \
	src/cpp/app/otr2_app.cpp


libotg2_la_LIBADD = -locomm \
	$(builddir)/src/cpp/core/libotg2core.la \
	$(builddir)/src/cpp/port/libotg2port.la \
	$(builddir)/src/cpp/generator/libotg2generator.la \
    -lpopt -lpthread -loml2



