dnl Process this file with -*- autoconf -*- to produce a configure script.

AC_PREREQ(2.50)

AC_INIT(libConfuse, 2.7, confuse-devel@nongnu.org, confuse)
AC_CONFIG_AUX_DIR(support)
AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR(src/confuse.c)

# Checks for programs.
# disable unnecessary compiler checks
AC_DEFUN([AC_PROG_F77], [:])
AC_DEFUN([AC_PROG_FC], [:])
AC_DEFUN([AC_PROG_CXX], [:])
AC_DEFUN([AC_PROG_CXXCPP], [:])
AC_DEFUN([AC_PROG_OBJC], [:])
AC_DEFUN([AC_PROG_OBJCCPP], [:])
AC_DEFUN([AC_LIBTOOL_CXX], [:])
AC_DEFUN([AC_LIBTOOL_F77], [:])
AC_PROG_CC
AM_PROG_LEX
AC_DISABLE_SHARED
AC_PROG_LIBTOOL

# optional building of examples:
AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--disable-examples], [don't build examples in examples])],
	[], [enable_examples=yes])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.16.1])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([strcasecmp strdup strndup])

dnl Check for the library containing inet_aton/inet_ntoa (for tests)
AC_SEARCH_LIBS([inet_ntoa], [socket nsl])

AC_CONFIG_FILES([Makefile \
		 src/Makefile \
		 examples/Makefile \
                 po/Makefile.in \
		 m4/Makefile \
		 tests/Makefile \
		 doc/Makefile \
                 doc/Doxyfile \
		 libconfuse.pc \
		 libconfuse.spec])
AC_OUTPUT

