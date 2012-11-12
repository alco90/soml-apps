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

AC_CONFIG_SUBDIRS([gpslogger iperf nmetrics otg2 ripwavemon trace wlanconfig])

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
