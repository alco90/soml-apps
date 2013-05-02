AC_PREREQ([2.69])
AC_INIT([collectd-write-oml2],
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
AC_PROG_LN_S

gl_INIT

AM_PROG_AR
LT_INIT([disable-static])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([memset strcasecmp])
AC_FUNC_MALLOC

# Specific libraries and headers
AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])
AC_CHECK_HEADER([oml2/omlc.h],  [AC_DEFINE([HAVE_OMLC_H], [1], [Define if omlc.h is installed.])], [missing_headers+="omlc.h"])

AC_ARG_WITH([debug],[AS_HELP_STRING([--enable-debug],
	    [enable debugging code])],
			  [debug=yes],
			  [debug=no])
AS_IF([test x$debug=xyes], [configure_debug=--enable-debug])

AC_ARG_WITH([collectd-version],[AS_HELP_STRING([--with-collectd-version],
	    [version of the collectd source to download @<:@default=5.1.0@:>@])],
			  [collectdver=$withval],
			  [collectdver=5.1.0])
AC_ARG_WITH([collectd],[AS_HELP_STRING([--with-collectd],
	    [path to Collectd source directory @<:@default=download source@:>@])],
			  [collectd_srcdir=$withval],
			  [collectd_srcdir=${PWD}/collectd-${collectdver}])
AC_MSG_CHECKING([whether collectd source is available in ${collectd_srcdir} (change with --with-collectd)])
AS_IF([test -d ${collectd_srcdir}],
     [AC_MSG_RESULT([yes])],
     [AC_MSG_RESULT([no])
     collectd_src=collectd-${collectdver}
     collectd_srcdir=${PWD}/${collectd_src}
     collectd_archive=/tmp/${collectd_src}.tar.bz2
     AS_IF([test -e ${collectd_archive} ], [
	   AC_MSG_NOTICE([collectd source archive found in ${collectd_archive}])], [
	   AC_CHECK_PROGS([getter], [wget curl])
	   AS_CASE([$getter],
		   [wget], [getter="$getter -nv -O ${collectd_archive}"],
		   [curl], [getter="$getter -o ${collectd_archive}"],
		   AC_MSG_ERROR([no supported download tool found...]))
	   AC_MSG_NOTICE([downloading ${collectd_src} source to ${collectd_archive} (change version with --collectd-version)...])
	   $getter http://collectd.org/files/collectd-${collectdver}.tar.bz2
	   ])
     tar xjf ${collectd_archive}
     ])
AS_IF([test -f ${collectd_srcdir}/src/config.h],
     [],
     [AC_MSG_NOTICE([configuring collectd source in ${collectd_srcdir} (see collectd_config.log for details)...])
     OPWD=$PWD
     cd ${collectd_srcdir}
     ./configure ${configure_debug} > $OPWD/collectd_config.log 2>&1 
     collectdver=`sed -n 's/^#define PACKAGE_VERSION "\([0-9.]\+\)"/\1/p' src/config.h`
     AS_IF([test $? = 0],
	   [AC_MSG_NOTICE([source in ${collectd_srcdir} configured succesfully])],
	   [AC_MSG_ERROR([./configure in ${collectd_srcdir} failed])])
     cd $OPWD
     ])
AX_COMPARE_VERSION([${collectdver}],[lt],[5.0.0],
     AC_DEFINE([COLLECTD_VERSION_PRE_5], [1], [Define if collectd version is <= 4.])
)
AC_SUBST([collectd_version], [$collectdver])
AC_SUBST([collectd_srcdir], [$collectd_srcdir])
AC_SUBST([collectd_incdir], [$collectd_srcdir/src])

AC_ARG_WITH([collectd-lib],[AS_HELP_STRING([--with-collectd-lib],
	                  [path to Collectd plugin directory @<:@default=/usr/lib/collectd@:>@])],
			  [collectd_libdir=$withval],
			  [collectd_libdir=$libdir/collectd])
AC_SUBST([collectd_libdir], [$collectd_libdir])

# Let the user know we're missing required headers, and don't generate the build system.
AS_IF([test "x$missing_headers" != "x"],
   [
    for lib in $missing_headers; do
      AC_MSG_WARN([could not find required library $lib])
   done
   AC_MSG_ERROR([missing headers, could not continue. Please install all of the following headers and then re-run configure (you might need the development header packages (-dev) as well if you are using your system's package manager to install them): $missing_headers])
   ])
# Let the user know we're missing required libraries, and don't generate the build system.
AS_IF([test "x$missing_libs" != "x"],
   [
    for lib in $missing_libs; do
      AC_MSG_WARN([could not find required library $lib])
   done
   AC_MSG_ERROR([missing libraries, could not continue. Please install all of the following libraries and then re-run configure (you might need the development header packages (-dev) as well if you are using your system's package manager to install them): $missing_libs])
   ])

AC_CONFIG_FILES([Makefile
		 write_oml/Makefile])
AC_OUTPUT
