ACLOCAL_AMFLAGS = -I m4

SUBDIRS = write_oml

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     m4/ax_compare_version.m4 \
	     $(top_srcdir)/.version

BUILT_SOURCES = $(top_srcdir)/.version

$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

clean-local:
	-test -d collectd-@collectd_version@/ && $(MAKE) -C collectd-@collectd_version@/ clean

distclean-local:
	rm -rf collectd-@collectd_version@/ collectd_config.log
