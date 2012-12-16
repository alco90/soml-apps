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

AC_ARG_WITH([debug],[AS_HELP_STRING([--enable-debug],
	    [enable debugging code])],
			  [debug=yes],
			  [debug=no])
AS_IF([test x$debug=xyes], [configure_debug=--enable-debug])

AC_ARG_WITH([collectd-include],[AS_HELP_STRING([--with-collectd-include],
	    [path to Collectd include directory @<:@default=download source@:>@])],
			  [collectd_incdir=$withval],
			  [collectd_incdir=no])
AS_IF([test x$collectd_incdir=xno], [
       collectdver=5.1.0
       AC_MSG_CHECKING([whether collectd source is available (change with --with-collectd-include)])
       AS_IF([test -d collectd-${collectdver}],
	     [AC_MSG_RESULT([yes])],
	     [AC_MSG_RESULT([no])
	     AC_CHECK_PROGS([getter], [wget curl])
	     AS_CASE([$getter],
		     [wget], [getter="$getter -nv -O -"],
		     [curl], [getter="$getter -o -"],
		     AC_MSG_ERROR([no support download tool found...]))
	     AC_MSG_NOTICE([downloading collectd-${collectdver} source...])
	     $getter http://collectd.org/files/collectd-${collectdver}.tar.bz2 | tar xj
	     ])
       AS_IF([test -f collectd-${collectdver}/src/config.h],
             [],
	     [AC_MSG_NOTICE([configuring collectd-${collectdver} source (see collectd_config.log for details)...])
	     cd collectd-${collectdver}; ./configure ${configure_debug} > ../collectd_config.log
	     AS_IF([test $? = 0],
		   [AC_MSG_NOTICE([collectd-${collectdver} configured succesfully])],
		   [AC_MSG_ERROR([./configure in collectd-${collectdver}/ failed])])
	     cd ..
	     ])
       collectd_incdir=$PWD/collectd-${collectdver}/src
       ])
AC_SUBST([collectd_version], [$collectdver])
AC_SUBST([collectd_incdir], [$collectd_incdir])

AC_ARG_WITH([collectd-lib],[AS_HELP_STRING([--with-collectd-lib],
	                  [path to Collectd plugin directory @<:@default=/usr/lib/collectd@:>@])],
			  [collectd_libdir=$withval],
			  [collectd_libdir=$libdir/collectd])
AC_SUBST([collectd_libdir], [$collectd_libdir])

AC_CONFIG_FILES([Makefile
		 write_oml/Makefile])
AC_OUTPUT
