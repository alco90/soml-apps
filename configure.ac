AC_PREREQ([2.69])
AC_INIT([oml2-apps], [2.9.pre0], [oml-user@list.nicta.com.au])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

AC_CONFIG_SUBDIRS([gpslogger iperf nmetrics otg2 ripwavemon trace wlanconfig])

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
