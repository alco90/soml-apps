ACLOCAL_AMFLAGS = -I m4 -Wall

appname = wlanconfig
installed_name = `echo '$(PACKAGE)' | sed '$(transform)'`
pkgdatadir = $(datadir)/$(installed_name)

bin_PROGRAMS = wlanconfig-oml2
wlanconfig_oml2_SOURCES = wlanconfig.c
wlanconfig_oml2_LDADD = -loml2 -locomm

pkgdata_DATA = $(appname).rb

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(appname).rb.in \
	     $(appname).rb

MAINTAINERCLEANFILES = $(appname).rb

$(appname).rb: $(appname).rb.in
	VER=`echo $(VERSION) | sed "s/-[0-9a-z]\+\(-dirty\)$$//;s/-.*//;s/\(pre\|rc\)\./-/;s/[^-0-9.]//g;s/\./, /g"`; \
		sed -e 's|@bindir[@]|$(bindir)|g' \
		-e "/a\.path/s/$(PACKAGE)/$(installed_name)/" \
		-e "s/a\.version.*/a.version($$VER)/" \
		$< > $@

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
