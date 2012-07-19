AC_PREREQ([2.69])
AC_INIT([ripwavemon-oml2], [2.9.pre0], [oml-user@lists.nicta.com.au])
AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LN_S

#Checks for libraries.
missing_libs=""

AC_CHECK_LIB([oml2], [omlc_init], [AC_DEFINE([HAVE_LIBOML2], [1], [Define if liboml2 is installed.])], [missing_libs+=" liboml2"])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h stdint.h string.h sys/socket.h sys/types.h netinet/ip.h])
AC_CHECK_HEADER([oml2/omlc.h],  [AC_DEFINE([HAVE_OMLC_H], [1], [Define if omlc.h is installed.])], [missing_headers+="omlc.h"])

AC_CHECK_FUNCS([memset socket ntohs nothl])

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
