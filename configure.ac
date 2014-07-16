AC_PREREQ([2.69])
AC_INIT([oml2-apps],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[oml-user@lists.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_PROG_CXX
AC_PROG_CC
gl_EARLY
AC_PROG_INSTALL
AC_PROG_MAKE_SET

gl_INIT

AC_SUBST([OML_PKG_VER], [`echo "$PACKAGE_VERSION" | sed 's/^\([[0-9]]\+\(\.[[0-9]]\+\)\{1,2\}\)\([[-.a-zA-Z0-9]]\+\)\?.*$/\1~\3/;s/-/./g'`])

built_apps="collectd iperf ripwavemon wattsup wpamon"
AC_CONFIG_SUBDIRS([collectd iperf ripwavemon wattsup wpamon])

# gpslogger
AC_CHECK_HEADER([gps.h], [
		 AC_CONFIG_SUBDIRS([gpslogger])
		 built_apps="$built_apps gpslogger"
		 ], [
		 AC_MSG_WARN([gpslogger will not be built])
		 notbuilt_apps="$notbuilt_apps gpslogger"
		 ])

# httperf
AC_CHECK_LIB([event],
		[event_init], [
		 AC_CONFIG_SUBDIRS([httperf])
		 built_apps="$built_apps httperf"
		 ], [
		 AC_MSG_WARN([httperf will not be built])
		 notbuilt_apps="$notbuilt_apps httperf"
		 ])

# nmetrics
AC_CHECK_HEADER([sigar.h])
AC_CHECK_HEADER([libsigar/sigar.h])
AS_IF([test "x$ac_cv_header_libsigar_sigar_h" != "xyes" -a "x$ac_cv_header_sigar_h" != "xyes"],
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
AS_IF([test "x$ac_cv_header_pcap_h" != "xyes" -o "x$ac_cv_header_libtrace_h" != "xyes" ],
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
				    gitlasttag=$gittag
				    gitistag=yes
				   ],
				   [
				    gitlasttag=`${GIT} describe --tags --abbrev=0 --match v\* HEAD`
				    AC_MSG_RESULT([none, latest tag is $gittag])
				    gitistag=no
				   ])

			     AX_WITH_PROG([MAKEPKG],[makepkg])
			     AS_IF([test "x$MAKEPKG" != "x"],[
				    OML_DISTRIBUTION_IS([ArchLinux],
							[/etc/arch-release],
							[isarch],
							[],
							[AC_SUBST([MAKEPKG_FLAGS_ADD],[--nodeps])])
				    OML_GIT_SEARCH_TREEISH([ArchLinux],
							  [gitarchref], [$gitistag],
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
							  [gitdebref], [$gitistag],
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
							  [gitrpmref], [$gitistag],
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
AC_SUBST([GITLASTTAG],[$gitlasttag])
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

AC_MSG_NOTICE([The following applications will be built: $built_apps])
AC_SUBST([BUILT_APPS],[$built_apps])
AS_IF([test -n "$notbuilt_apps"],
      [AC_MSG_WARN([The following applications will *not* be built (missing dependencies, see WARNINGs above): $notbuilt_apps])])

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
