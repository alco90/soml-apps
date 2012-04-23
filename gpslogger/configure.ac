AC_PREREQ([2.6])
AC_INIT([gpslogger-oml2], [2.7.0], [oml-user@lists.nicta.com.au])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h])
AC_CHECK_HEADER([oml2/omlc.h],  [AC_DEFINE([HAVE_OMLC_H], [1], [Define if omlc.h is installed.])], [missing_headers+="omlc.h"])
AC_CHECK_HEADER([gps.h], [AC_DEFINE([HAVE_GPS_H], [1], [Define if gps.h is installed.])], [missing_headers+="gps.h"])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostbyname gettimeofday inet_ntoa memmove memset socket sqrt strerror])

#Checks for libraries.
missing_libs=""

AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])
AC_CHECK_LIB([gps], [gps_open], [AC_DEFINE([HAVE_LIBGPS], [1], [Define if libgps is installed.])], [missing_libs+=" libgps"])

# Let the user know we're missing required libraries, and don't generate the build system.
if test "x$missing_libs" != "x"; then
   for lib in $missing_libs; do
      AC_MSG_WARN([could not find required library $lib])
   done
   AC_MSG_ERROR([missing libraries, could not continue.
Please install all of the following libraries and then re-run configure
(you might need the development header packages (-dev) as well if you are
using your system's package manager to install them):
$missing_libs])
fi

# Let the user know we're missing required header files, and don't generate the build system.
if test "x$missing_headers" != "x"; then
   for header in $missing_headers; do
      AC_MSG_WARN([could not find required header $header])
   done
   AC_MSG_ERROR([missing headers, could not continue.
Please install all of the following headers (and their corresponding
libraries) and then re-run configure.  Usually the headers for a library
a contained in the development header package for the library if your
system uses a package manager (for instance, libsigar --> libsigar-dev):
$missing_headers])
fi

# Generate output files
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
