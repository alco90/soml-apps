AC_PREREQ([2.69])
AC_INIT([otg2],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[oml-user@lists.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# automake 1.12 seems to require this, but automake 1.11 doesn't recognize it
m4_pattern_allow([AM_PROG_AR]) 
AM_PROG_AR

LT_INIT

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
gl_EARLY
AC_PROG_INSTALL
AC_PROG_MAKE_SET

gl_INIT
AC_PROG_LN_S

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h])

# Checks for library functions.
AC_CHECK_FUNCS([gethostbyname gettimeofday inet_ntoa memmove memset socket sqrt strerror])
AC_FUNC_FORK
AC_FUNC_MALLOC

AC_CHECK_LIB([pthread], [pthread_create], [AC_DEFINE([HAVE_LIBPTHREAD], [1], [Define if libpthread is installed.])], [missing_libs+=" libpthread"])
AC_CHECK_HEADER([pthread.h], [AC_DEFINE([HAVE_PTHREAD_H], [1], [Define if pthread.h is installed.])], [missing_headers+="pthread.h"])


# Specific libraries and headers
AC_CHECK_LIB([popt], [poptGetContext], [AC_DEFINE([HAVE_LIBPOPT], [1], [Define if libpopt is installed.])], [missing_libs+=" libpopt"])
AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])

AC_CHECK_HEADER([popt.h], [AC_DEFINE([HAVE_POPT_H], [1], [Define if popt.h is installed.])], [missing_headers+="popt.h"])
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

# Generate output files
AC_CONFIG_FILES([Makefile
		lib/core/Makefile
		lib/generator/Makefile
		lib/port/Makefile
		])
AC_OUTPUT
