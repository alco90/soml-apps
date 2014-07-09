ACLOCAL_AMFLAGS = -I m4 -Wall

AUTOMAKE_OPTIONS = subdir-objects

# SED is otherwise provided by libtool.m4
SED=sed

AM_CPPFLAGS = -I $(srcdir) \
	      -I $(srcdir)/lib

SCAFFOLD=oml2-scaffold

appotg2 = otg2
progotg2 = $(appotg2)-oml2
installed_otg2 = `echo '$(progotg2)' | $(SED) '$(transform)'`

appotr2 = otr2
progotr2 = $(appotr2)-oml2
installed_otr2 = `echo '$(progotr2)' | $(SED) '$(transform)'`

pkgdatadir = $(datadir)/$(installed_otg2)

SUBDIRS = lib/core lib/port lib/generator

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(appotg2).rb.in \
	     $(appotr2).rb.in

BUILT_SOURCES = $(top_srcdir)/.version \
		$(appotg2).rb \
		$(appotr2).rb \
		otg2_oml.h \
		otr2_oml.h

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

CLEANFILES =  otg2_oml.h \
	      otr2_oml.h \
	      $(appotg2).rb \
	      $(appotr2).rb

omllibs= -loml2 -locomm -lpthread
otgrlibs = $(omllibs) libotg2.la

bin_PROGRAMS = otg2-oml2 otr2-oml2

otg2_oml2_SOURCES = version.h \
		    otg2.cpp \
		    otg2_oml.h
otg2_oml2_LDADD = $(otgrlibs)

otr2_oml2_SOURCES = version.h \
		    otr2.cpp \
		    otr2_oml.h
otr2_oml2_LDADD = $(otgrlibs)

pkglib_LTLIBRARIES = libotg2.la
pkgdata_DATA = $(appotg2).rb $(appotr2).rb


$(appotg2).rb: $(appotg2).rb.in
	VER=`echo $(VERSION) | $(SED) "s/.*+oml//;s/\([0-9]\{1,\}\(\.[0-9]\{1,\}\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    $(SED) -e 's|@bindir[@]|$(bindir)|g' \
		-e "/app\.path/s/$(appotg2)/$(installed_otg2)/" \
		-e "s/app\.version.*/app.version($$VER)/" \
		$< > $@
$(appotr2).rb: $(appotr2).rb.in
	VER=`echo $(VERSION) | $(SED) "s/.*+oml//;s/\([0-9]\{1,\}\(\.[0-9]\{1,\}\)\{1,2\}\).*/\1/;s/\./, /g"`; \
	    $(SED) -e 's|@bindir[@]|$(bindir)|g' \
		-e "/app\.path/s/$(appotr2)/$(installed_otr2)/" \
		-e "s/app\.version.*/app.version($$VER)/" \
		$< > $@

$(appotg2)_oml.h: $(appotg2).rb
	$(SCAFFOLD) --oml $<
$(appotr2)_oml.h: $(appotr2).rb
	$(SCAFFOLD) --oml $<

libotg2_la_SOURCES = \
	lib/app/application.cpp \
	lib/app/otg2_app.cpp \
	lib/app/otr2_app.cpp \
	lib/otg2/address.h \
	lib/otg2/application.h \
	lib/otg2/compat.h \
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

libotg2_la_LIBADD = \
	$(omllibs) -lpopt \
	$(builddir)/lib/core/libotg2core.la \
	$(builddir)/lib/port/libotg2port.la \
	$(builddir)/lib/generator/libotg2generator.la

target_otg2 = $(installed_otg2)$(EXEEXT)
target_otr2 = $(installed_otr2)$(EXEEXT)
installed_links_otg2 = $(DESTDIR)$(bindir)/`echo $(appotg2) | $(SED) '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo oml2-$(appotg2) | $(SED) '$(transform)'`$(EXEEXT)
installed_links_otr2 = $(DESTDIR)$(bindir)/`echo $(appotr2) | $(SED) '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo oml2-$(appotr2) | $(SED) '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_links_otg2); do  \
		test -e $${OLDNAME} && mv $${OLDNAME} $${OLDNAME}.pre$(PACKAGE_VERSION); \
		$(LN_S) $(target_otg2) $${OLDNAME}; \
	done
	for OLDNAME in $(installed_links_otr2); do  \
		test -e $${OLDNAME} && mv $${OLDNAME} $${OLDNAME}.pre$(PACKAGE_VERSION); \
		$(LN_S) $(target_otr2) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appotg2).rb
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appotr2).rb
