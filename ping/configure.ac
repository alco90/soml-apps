AC_PREREQ([2.69])
AC_INIT([ping-oml2],
	m4_esyscmd([build-aux/git-version-gen .tarball-version]),
	[oml-user@lists.nicta.com.au])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
gl_EARLY
AC_PROG_INSTALL
AC_PROG_LN_S

gl_INIT

AC_MSG_NOTICE([You'll need Ruby and the OML4R gem to be able to run ping-oml2])

# Generate output files
AC_CONFIG_FILES([Makefile
		 ping-oml2])
AC_OUTPUT
