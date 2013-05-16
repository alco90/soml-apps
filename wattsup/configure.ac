AC_PREREQ([2.69])
AC_INIT([wattsup-oml2],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[oml-user@lists.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC
gl_EARLY

gl_INIT

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_STRTOD
AC_CHECK_FUNCS([fchdir memset select strcasecmp strchr strdup strpbrk strtol strtoul])

# Specific libraries and headers
AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])
AC_CHECK_HEADER([ocomm/o_log.h],  [AC_DEFINE([HAVE_O_LOG_H], [1], [Define if o_log.h is installed.])], [missing_headers+="o_log.h (libocomm)"])
AC_CHECK_HEADER([oml2/omlc.h],  [AC_DEFINE([HAVE_OMLC_H], [1], [Define if omlc.h is installed.])], [missing_headers+="omlc.h"])

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

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
