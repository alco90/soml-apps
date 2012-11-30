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

built_apps="iperf ripwavemon wlanconfig"
AC_CONFIG_SUBDIRS([iperf ripwavemon wlanconfig])

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

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

AC_MSG_NOTICE([The following applications will be built: $built_apps])
AS_IF([test -n "$notbuilt_apps"],
      [AC_MSG_WARN([The following applications will *not* be built (missing dependencies, see WARNINGs above): $notbuilt_apps])])
