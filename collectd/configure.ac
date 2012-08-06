AC_PREREQ([2.69])
AC_INIT([collectd-oml2], [2.9.pre0], [oml-user@list.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([write_oml/write_oml.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AM_PROG_AR
LT_INIT([disable-static])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strcasecmp])
AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])

AC_ARG_WITH([collectd],[AS_HELP_STRING([--with-collectd],
	                  [path to Collectd plugin directory @<:@default=/usr/lib/collectd@:>@])],
			  [collectd_libdir=$withval],
			  [collectd_libdir=/usr/lib/collectd])
AC_SUBST([collectd_libdir], [$collectd_libdir])

AC_CONFIG_FILES([Makefile
		 write_oml/Makefile])
AC_OUTPUT
