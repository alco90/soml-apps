ACLOCAL_AMFLAGS = -I m4 -Wall

appotg2 = otg2
progotg2 = $(appotg2)-oml2
prefixedotg2 = $(shell echo $(progotg2) | sed '$(transform)')

appotr2 = otr2
progotr2 = $(appotr2)-oml2
prefixedotr2 = $(shell echo $(progotr2) | sed '$(transform)')

SUBDIRS = lib/core lib/port lib/generator

AM_CPPFLAGS = \
	-I $(srcdir) \
	-I $(srcdir)/lib \
	-DWITH_OML

otgrlibs = -lpthread libotg2.la

bin_PROGRAMS = otg2-oml2 otr2-oml2

otg2_oml2_SOURCES = \
	version.h \
	otg2.cpp
otg2_oml2_LDADD = $(otgrlibs)

otr2_oml2_SOURCES = otr2.cpp
otr2_oml2_LDADD = $(otgrlibs)

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

target_otg2 = $(prefixedotg2)$(EXEEXT)
target_otr2 = $(prefixedotr2)$(EXEEXT)
installed_links_otg2 = $(DESTDIR)$(bindir)/$(shell echo $(appotg2) | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo oml2-$(appotg2) | sed '$(transform)')$(EXEEXT)
installed_links_otr2 = $(DESTDIR)$(bindir)/$(shell echo $(appotr2) | sed '$(transform)')$(EXEEXT) $(DESTDIR)$(bindir)/$(shell echo oml2-$(appotr2) | sed '$(transform)')$(EXEEXT)

install-exec-hook:
	rm -f $(installed_link_otg2)
	for OLDNAME in $(installed_links_otg2); do \
		$(LN_S) $(target_otg2) $${OLDNAME}; \
	done
	rm -f $(installed_link_otr2)
	for OLDNAME in $(installed_links_otr2); do \
		$(LN_S) $(target_otr2) $${OLDNAME}; \
	done
