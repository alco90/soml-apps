ACLOCAL_AMFLAGS = -I m4 -Wall

SUBDIRS = gpslogger nmetrics iperf otg2 ripwavemon trace wlanconfig

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version

if ENABLE_PACKAGING

# LN_S is otherwise provided by libtool.m4
LN_S=ln -s
# So is sed
SED=sed

PKG_PACKAGES=
PKG_CLEANFILES=

if BUILD_DEBIAN
PKG_DEB_BASE_NAME=$(PACKAGE)_$(VERSION)
PKG_DEB_BUILD_DIR=p-debian
PKG_DEB_ORIG_SRC=$(PKG_DEB_BUILD_DIR)/$(PKG_DEB_BASE_NAME).orig.tar.gz
PKG_DEB_RULES=$(PKG_DEB_BUILD_DIR)/$(distdir)/debian/rules

PKG_PACKAGES+=pkg-deb
PKG_CLEANFILES+= $(PKG_DEB_BUILD_DIR)

DPKG_BUILDPACKAGE_FLAGS=-us -uc $(DPKG_BUILDPACKAGE_FLAGS_ADD)

$(PKG_DEB_RULES): dist-gzip
	rm -rf $(PKG_DEB_BUILD_DIR)
	mkdir -p $(PKG_DEB_BUILD_DIR)
	test -e $(PKG_DEB_ORIG_SRC) || $(LN_S) ../$(distdir).tar.gz $(PKG_DEB_ORIG_SRC)
	$(MAKE) distdir=$(PKG_DEB_BUILD_DIR)/$(distdir)/ distdir
	$(GIT) archive $(GITDEBREF) | $(am__untar) -C $(PKG_DEB_BUILD_DIR)/$(distdir)/
if !IS_DEBIAN
	mv $(PKG_DEB_RULES) $(PKG_DEB_RULES).orig
	cat $(PKG_DEB_RULES).orig | \
		$(SED) -e "s/dh_shlibdeps/#&/" \
		> $(PKG_DEB_RULES)
	rm $(PKG_DEB_RULES).orig
endif
if !GITISTAG
	cd $(PKG_DEB_BUILD_DIR)/$(distdir)/; $(DCH) -v $(VERSION)-1 "Development build (`$(GIT) describe` on branch `$(GIT) describe --contains --all HEAD`), not for general distribution; changes since $(GITTAG) follow"
	cd $(PKG_DEB_BUILD_DIR)/$(distdir)/; $(GIT) log --pretty='%s' $(GITTAG)..HEAD | while read LINE; do $(DCH) -a "$${LINE}"; done
endif

pkg-deb: $(PKG_DEB_BUILD_DIR)/$(distdir)/debian/rules
	cd $(PKG_DEB_BUILD_DIR)/$(distdir)/; $(DPKG_BUILDPACKAGE) $(DPKG_BUILDPACKAGE_FLAGS)
endif #BUILD_DEBIAN

pkg-all: $(PKG_PACKAGES)

pkg-clean:
	rm -rf $(PKG_CLEANFILES)

distclean-local: pkg-clean

endif #ENABLE_PACKAGING
