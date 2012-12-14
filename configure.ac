AC_PREREQ([2.69])
AC_INIT([oml2-apps],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[oml-user@lists.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_CC
gl_EARLY
AC_PROG_INSTALL
AC_PROG_MAKE_SET

gl_INIT

AC_SUBST([OML_ALT_VER], [`echo "$PACKAGE_VERSION" | sed 's/-/./g'`])

built_apps="iperf ping ripwavemon wpamon wlanconfig"
AC_CONFIG_SUBDIRS([iperf ping ripwavemon wpamon wlanconfig])

# gpslogger
AC_CHECK_HEADER([gps.h], [
		 AC_CONFIG_SUBDIRS([gpslogger])
		 built_apps="$built_apps gpslogger"
		 ], [
		 AC_MSG_WARN([gpslogger will not be built])
		 notbuilt_apps="$notbuilt_apps gpslogger"
		 ])

# nmetrics
AC_CHECK_HEADER([sigar.h])
AC_CHECK_HEADER([libsigar/sigar.h])
AS_IF([test "x$ac_cv_header_libsigar_sigar_h" = "xno" -a "x$ac_cv_header_sigar_h" = "no"],
      [AC_MSG_WARN([nmetrics will not be built])
      notbuilt_apps="$notbuilt_apps nmetrics"],
      [AC_CONFIG_SUBDIRS([nmetrics])
      built_apps="$built_apps nmetrics"]
)

# otg2
AC_CHECK_HEADER([pthread.h], [
		 AC_CONFIG_SUBDIRS([otg2])
		 built_apps="$built_apps otg2"
		 ], [
		 AC_MSG_WARN([otg2 will not be built])
		 notbuilt_apps="$notbuilt_apps otg2"
		 ])

# trace
AC_CHECK_HEADER([pcap.h])
AC_CHECK_HEADER([libtrace.h])
AS_IF([test "x$ac_cv_header_pcap_h" = "xno" -o "x$ac_cv_libtrace_h" = "xno" ],
      [AC_MSG_WARN([trace will not be built])
      notbuilt_apps="$notbuilt_apps trace"],
      [AC_CONFIG_SUBDIRS([trace])
      built_apps="$built_apps trace"]
)

AC_ARG_ENABLE([packaging],
	      [AS_HELP_STRING([--enable-packaging],
			      [enable targets to create distribution-specific packages (Git clone needed)])],
	      [
	       AX_WITH_PROG([GIT],[git])
	       AS_IF([test "x$enable_packaging" != "xno" && test -n "$GIT"],
		     [
		      AC_MSG_CHECKING([whether \$srcdir is a Git repo])
		      AS_IF([test "x${srcdir}" = "x." && $GIT status >/dev/null 2>&1],
			    [
			     AC_MSG_RESULT([yes])
			     AC_MSG_CHECKING([current Git branch])
			     gitbranch=`${GIT} describe --contains --all HEAD`
			     AC_MSG_RESULT([$gitbranch])

			     AC_MSG_CHECKING([current Git tag])
			     gittag=`${GIT} describe --exact-match --match v\* 2>/dev/null`
			     AS_IF([ test "x" != "x$gittag"],
				   [
				    AC_MSG_RESULT([$gittag])
				    gitistag=yes
				   ],
				   [
				    AC_MSG_RESULT([none])
				    gitistag=no

				    AC_MSG_CHECKING([closest Git tag])
				    gittag=`${GIT} describe --abbrev=0 2>/dev/null`
				    AC_MSG_RESULT([$gittag])
				   ])

			     AX_WITH_PROG([MAKEPKG],[makepkg])
			     AS_IF([test "x$MAKEPKG" != "x"],[
				    OML_DISTRIBUTION_IS([ArchLinux],
							[/etc/arch-release],
							[isarch],
							[],
							[AC_SUBST([MAKEPKG_FLAGS_ADD],[--nodeps])])
				    OML_GIT_SEARCH_TREEISH([ArchLinux],
							  [gitarchref],
							  [archlinux/$gittag,
							   origin/archlinux/$gittag,
							   archlinux/$gitbranch,
							   origin/archlinux/$gitbranch,
							   archlinux/master],
							   [origin/archlinux/master])
				   ])
			     AX_WITH_PROG([DPKG_BUILDPACKAGE],[dpkg-buildpackage])
			     AX_WITH_PROG([DCH],[dch])
			     AS_IF([test "x$DPKG_BUILDPACKAGE" != "x" -a "x$DCH" != "x"],[
				    OML_DISTRIBUTION_IS([Debian-based],
							[/etc/debian_version],
							[isdebian],
							[],
							[AC_SUBST([DPKG_BUILDPACKAGE_FLAGS_ADD],[-d])])
				    OML_GIT_SEARCH_TREEISH([Debian],
							  [gitdebref],
							  [debian/$gittag,
							   origin/debian/$gittag,
							   debian/$gitbranch,
							   origin/debian/$gitbranch,
							   debian/master],
							   [origin/debian/master])
				   ])
			     AX_WITH_PROG([RPMBUILD],[rpmbuild])
			     AS_IF([test "x$RPMBUILD" != "x"], [
				    OML_DISTRIBUTION_IS([RedHat-based],
							[/etc/redhat-release],
							[isredhat],
							[],
							[AC_SUBST([RPMBUILD_FLAGS_ADD],[--nodeps])])
				    OML_GIT_SEARCH_TREEISH([RPM],
							  [gitrpmref],
							  [rpm/$gittag,
							   origin/rpm/$gittag,
							   rpm/$gitbranch,
							   origin/rpm/$gitbranch,
							   rpm/master],
							   [origin/rpm/master])
				   ])
			    ],
			    [
			     AC_MSG_RESULT([no])
			     AC_MSG_WARN([--enable-packaging needs git and a Git repo for binary distributions])
			     enable_packaging=partial
			    ])
		      ])],
	      [enable_packaging=no])
AC_SUBST([GITBRANCH],[$gitbranch])
AC_SUBST([GITTAG],[$gittag])
AC_SUBST([GITHEAD],[$githead])
AC_SUBST([GITARCHREF],[$gitarchref])
AC_SUBST([GITDEBREF],[$gitdebref])
AC_SUBST([GITRPMREF],[$gitrpmref])
AM_CONDITIONAL([ENABLE_PACKAGING], [test "x$enable_packaging" != "xno" && test "x$enable_packaging" != "xpartial"])
AM_CONDITIONAL([GITISTAG], [test "x$gitistag" != "xno"])
AM_CONDITIONAL([BUILD_ARCH], [test "x$MAKEPKG" != "x"])
AM_CONDITIONAL([BUILD_DEBIAN], [test "x$DPKG_BUILDPACKAGE" != "x" -a "x$DCH" != "x"])
AM_CONDITIONAL([BUILD_RPM], [test "x$RPMBUILD" != "x"])
AM_CONDITIONAL([IS_ARCH], [test "x$isarch" != "x"])
AM_CONDITIONAL([IS_DEBIAN], [test "x$isdebian" != "x"])
AM_CONDITIONAL([IS_REDHAT], [test "x$isredhat" != "x"])

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([The following applications will be built: $built_apps])
AS_IF([test -n "$notbuilt_apps"],
      [AC_MSG_WARN([The following applications will *not* be built (missing dependencies, see WARNINGs above): $notbuilt_apps])])
