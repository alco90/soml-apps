ACLOCAL_AMFLAGS = -I m4 -Wall

SUBDIRS = gpslogger nmetrics iperf otg2 ripwavemon trace wlanconfig

EXTRA_DIST = autogen.sh \
	     autoclean.sh \
	     m4/gnulib-cache.m4 \
	     $(top_srcdir)/.version \
	     $(top_srcdir)/build-aux/gen-authors.sh

BUILT_SOURCES = $(top_srcdir)/.version
$(top_srcdir)/.version:
	echo $(VERSION) > $@-t && mv $@-t $@
dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
	if test -d .git; then							\
		$(top_srcdir)/build-aux/gen-authors.sh > $(distdir)/AUTHORS;	\
	fi

if ENABLE_PACKAGING

# LN_S is otherwise provided by libtool.m4
LN_S=ln -s
# So is sed
SED=sed

PKG_PACKAGES=
PKG_CLEANFILES=

if BUILD_ARCH
PKG_ARCH_BASE_NAME=$(PACKAGE)-$(OML_ALT_VER)
PKG_ARCH_BUILD_DIR=p-arch
PKG_ARCH_BUILD=$(PKG_ARCH_BUILD_DIR)/PKGBUILD

PKG_PACKAGES+=pkg-arch
PKG_CLEANFILES+=$(PKG_ARCH_BUILD_DIR)

MAKEPKG_FLAGS=-fc $(MAKEPKG_FLAGS_ADD)

$(PKG_ARCH_BUILD):
	rm -rf $(PKG_ARCH_BUILD_DIR)
	mkdir -p $(PKG_ARCH_BUILD_DIR)/tmp
	$(MAKE) distdir=$(PKG_ARCH_BUILD_DIR)/tmp/$(PKG_ARCH_BASE_NAME) distdir
	$(GIT) archive $(GITARCHREF) | $(am__untar) -C $(PKG_ARCH_BUILD_DIR)
# Releases should already know where to get the tarball
if !GITISTAG
	cd $(PKG_ARCH_BUILD_DIR)/tmp/; \
		tardir=$(PKG_ARCH_BASE_NAME) && $(am__tar) | \
		GZIP=$(GZIP_ENV) gzip -c >../$(PKG_ARCH_BASE_NAME).tar.gz
endif
	cat $(PKG_ARCH_BUILD).proto | \
		$(SED) -e "/<GIT/,/GIT>/d" | \
		$(SED) -e "/!GIT.*/d" -e "s/^pkgver.*=.*/pkgver=$(OML_ALT_VER)/" \
		> $(PKG_ARCH_BUILD)
	cd $(PKG_ARCH_BUILD_DIR); $(MAKEPKG) -p PKGBUILD -g >> PKGBUILD
	cat $(PKG_ARCH_BUILD).proto | \
		$(SED) -e "/<!GIT/,/!GIT>/d" -e "/GIT.*/d" \
		> $(PKG_ARCH_BUILD)-git
	cd $(PKG_ARCH_BUILD_DIR); $(MAKEPKG) -p PKGBUILD-git -g >> PKGBUILD-git

pkg-arch: $(PKG_ARCH_BUILD)
	cd $(PKG_ARCH_BUILD_DIR); $(MAKEPKG) $(MAKEPKG_FLAGS)
	cd $(PKG_ARCH_BUILD_DIR); $(MAKEPKG) $(MAKEPKG_FLAGS) --source
	cd $(PKG_ARCH_BUILD_DIR); $(MAKEPKG) $(MAKEPKG_FLAGS) -p PKGBUILD-git --source
endif #BUILD_ARCH

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

if BUILD_RPM


PKG_RPM_BASE_NAME=$(PACKAGE)-$(OML_ALT_VER)
PKG_RPM_SPEC=$(PKG_RPM_BUILD_DIR)/SPECS/$(PACKAGE).spec
PKG_RPM_BUILD_DIR=p-rpm

PKG_PACKAGES+=pkg-rpm
PKG_CLEANFILES+=$(PKG_RPM_BUILD_DIR)

RPMBUILD_FLAGS=-bb --clean $(RPMBUILD_FLAGS_ADD)

$(PKG_RPM_SPEC):
	rm -rf $(PKG_RPM_BUILD_DIR)
	mkdir -p $(PKG_RPM_BUILD_DIR)/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	$(GIT) archive $(GITRPMREF) | $(am__untar) -C $(PKG_RPM_BUILD_DIR)/SPECS/
if GITISTAG
# Releases should have a valid specfile and know where to get the tarball
#
# Extract all defines from a SPECFILE, and reformat them into VAR=VALUE, to be
# evaluated by the shell. Then, replace %{VAR} with ${VAR} in the Source entries
# of the SPECFILE, and let the shell substitute these variables with their newly
# assigned values.
	URL=$$(eval "`sed -n "s/%define\s\+\(\S\+\)\s\+\(\S\+\)$$/\1=\2/p;/Source[0-9]*:/{s/Source[0-9]*:\s*/echo /;s/%/\$$/g;p}" $(PKG_RPM_SPEC)`"); \
		wget -q $${URL} -O $(PKG_RPM_BUILD_DIR)/SOURCES/`basename $${URL}`
else
	$(MAKE) distdir=$(PKG_RPM_BUILD_DIR)/BUILD/$(PKG_RPM_BASE_NAME) distdir
	cd $(PKG_RPM_BUILD_DIR)/BUILD/; \
		tardir=$(PKG_RPM_BASE_NAME) && $(am__tar) | \
		GZIP=$(GZIP_ENV) gzip -c >../SOURCES/$(PKG_RPM_BASE_NAME).tar.gz
	mv $(PKG_RPM_SPEC) $(PKG_RPM_SPEC).orig
	cat $(PKG_RPM_SPEC).orig | \
		$(SED) -e "s/^%define version.*/%define version	$(OML_ALT_VER)/" \
		-e "s/^%define pkgver.*/%define pkgver $(VERSION)/" \
		-e "s/^%define liboml2cur.*/%define liboml2cur $(LIBOML2_LT_VER_CUR)/" \
		-e "s/^%define liboml2rev.*/%define liboml2rev $(LIBOML2_LT_VER_REV)/" \
		-e "s/^%define liboml2age.*/%define liboml2age $(LIBOML2_LT_VER_AGE)/" \
		-e "s/^%define libocommcur.*/%define libocommcur $(LIBOCOMM_LT_VER_CUR)/" \
		-e "s/^%define libocommrev.*/%define libocommrev $(LIBOCOMM_LT_VER_REV)/" \
		-e "s/^%define libocommage.*/%define libocommage $(LIBOCOMM_LT_VER_AGE)/" \
		> $(PKG_RPM_SPEC)
	rm $(PKG_RPM_SPEC).orig
endif

pkg-rpm: $(PKG_RPM_SPEC)
	$(RPMBUILD) $(RPMBUILD_FLAGS) --define "_topdir $(abs_top_builddir)/$(PKG_RPM_BUILD_DIR)" $(PKG_RPM_SPEC)
endif #BUILD_RPM

pkg-all: $(PKG_PACKAGES)

pkg-clean:
	rm -rf $(PKG_CLEANFILES)

distclean-local: pkg-clean

endif #ENABLE_PACKAGING
