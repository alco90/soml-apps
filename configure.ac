AC_PREREQ([2.6])
AC_INIT([oml2-apps], [2.5.0], [http://oml.mytestbed.net/wiki/OML/FAQ_and_Support])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_PROG_LIBTOOL([1.4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CONFIG_FILES([Makefile
		simple/Makefile
		])

AC_CONFIG_SUBDIRS([omf_nmetrics omf_trace otg ripwave_monitor wlanconfig])

# Generate output files
AC_OUTPUT
