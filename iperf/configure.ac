AC_PREREQ([2.6])
AC_INIT([oml2-iperf], [2.4.0], [http://omf.mytestbed.net/wiki/OML/FAQ_and_Support])
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
AC_PROG_LN_S

# Make a shell variable for checking Mac OS X
if test x`uname` = xDarwin; then
   DARWIN=yes
fi

#Checks for libraries.
missing_libs=""

# Create a Makefile variable for checking Mac OS X
AM_CONDITIONAL([DARWIN], [test x`uname` = xDarwin])
AC_CHECK_LIB([popt], [poptGetContext], [AC_DEFINE([HAVE_LIBPOPT], [1], [Define if libpopt is installed.])], [missing_libs+=" libpopt"])
AC_CHECK_LIB([pthread], [pthread_create], [AC_DEFINE([HAVE_LIBPTHREAD], [1], [Define if libpthread is installed.])], [missing_libs+=" libpthread"])
AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])
AC_CHECK_LIB([ocomm], [o_log], [AC_DEFINE([HAVE_LIBOCOMM], [1], [Define if libocomm is installed.])], [missing_libs+=" libocomm"])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netdb.h netinet/in.h stdlib.h string.h strings.h sys/ioctl.h sys/socket.h sys/time.h sys/timeb.h unistd.h])
AC_CHECK_HEADER([oml2/omlc.h],  [AC_DEFINE([HAVE_OMLC_H], [1], [Define if omlc.h is installed.])], [missing_headers+="omlc.h"])
AC_CHECK_HEADER([ocomm/o_log.h],  [AC_DEFINE([HAVE_O_LOG_H], [1], [Define if o_log.h is installed.])], [missing_headers+="o_log.h"])
AC_CHECK_HEADER([pthread.h], [AC_DEFINE([HAVE_PTHREAD_H], [1], [Define if pthread.h is installed.])], [missing_headers+="pthread.h"])
AC_CHECK_HEADER([popt.h], [AC_DEFINE([HAVE_POPT_H], [1], [Define if popt.h is installed.])], [missing_headers+="popt.h"])

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

AC_C_BIGENDIAN

AC_CONFIG_FILES([Makefile])



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
AC_OUTPUT
