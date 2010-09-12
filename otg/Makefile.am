ACLOCAL_AMFLAGS = -I m4 -Wnone

SUBDIRS = lib/core lib/port lib/generator

AM_CPPFLAGS = \
	-I $(srcdir) \
	-I $(srcdir)/lib \
	-DWITH_OML

otgrlibs = -lpthread libotg2.la

bin_PROGRAMS = oml2-otg2 oml2-otr2

oml2_otg2_SOURCES = \
	version.h \
	otg2.cpp

oml2_otr2_SOURCES = otr2.cpp

oml2_otg2_LDADD = $(otgrlibs)
oml2_otr2_LDADD = $(otgrlibs)

pkglib_LTLIBRARIES = libotg2.la 

libotg2_la_SOURCES = \
	lib/app/application.cpp \
	lib/app/otg2_app.cpp \
	lib/app/otr2_app.cpp \
	lib/otg2/address.h \
	lib/otg2/application.h \
	lib/otg2/component.h \
	lib/otg2/generator.h \
	lib/otg2/otg2_app.h \
	lib/otg2/otr2_app.h \
	lib/otg2/packet.h \
	lib/otg2/port.h \
	lib/otg2/sender.h \
	lib/otg2/source.h \
	lib/otg2/stream.h \
	lib/otg2/unixtime.h

libotg2_la_LIBADD = -locomm \
	$(builddir)/lib/core/libotg2core.la \
	$(builddir)/lib/port/libotg2port.la \
	$(builddir)/lib/generator/libotg2generator.la \
    -lpopt -lpthread -loml2

target_otg = oml2-otg2$(EXEEXT)
target_otr = oml2-otr2$(EXEEXT)
installed_link_otg = $(DESTDIR)$(bindir)/otg2$(EXEEXT)
installed_link_otr = $(DESTDIR)$(bindir)/otr2$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_otg) $(installed_link_otr)
	$(LN_S) $(target_otg) $(installed_link_otg)
	$(LN_S) $(target_otr) $(installed_link_otr)
