ACLOCAL_AMFLAGS = -I m4 -Wall

appname = nmetrics
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = nmetrics-oml2
nmetrics_oml2_SOURCES = nmetrics.c
nmetrics_oml2_LDADD = -loml2 -lsigar -lpopt
nmetrics_oml2_CPPFLAGS = $(LIBSIGAR_CFLAGS)

pkgdata_DATA = $(appname).rb

BUILT_SOURCES = \
	$(appname).rb \
	$(appname)_popt.h \
	$(appname)_oml.h

CLEANFILES = \
	$(pkgdata_DATA) \
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST = m4/gnulib-cache.m4 \
	     $(appname).rb.in


$(appname)_popt.h: $(appname).rb Makefile
	oml2-scaffold --opts $<

$(appname)_oml.h: $(appname).rb Makefile
	oml2-scaffold --oml $<

$(appname).rb: $(appname).rb.in
	sed -e 's|@bindir[@]|$(bindir)|g' -e "/a\.path/s/$(PACKAGE)/$(installed_name)/" $< > $@

installed_link_names = \
		       $(DESTDIR)$(bindir)/`echo "$(appname)_oml2" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo "oml2-$(appname)" | sed '$(transform)'`$(EXEEXT) \
		       $(DESTDIR)$(bindir)/`echo "omf_$(appname)" | sed '$(transform)'`$(EXEEXT)

install-exec-hook:
	for OLDNAME in $(installed_link_names); do \
		test -e $${OLDNAME} && mv $${OLDNAME} $${OLDNAME}.pre$(PACKAGE_VERSION); \
		$(LN_S) $(installed_name) $${OLDNAME}; \
	done

uninstall-hook:
	rm -f $(installed_link_names)
	rm -f $(DESTDIR)/$(pkgdatadir)/$(appname).rb
