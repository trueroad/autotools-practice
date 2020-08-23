#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libhello-world], [0.0.0], [test@example.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/hello-world.cc])
AC_CONFIG_HEADERS([src/config.h])

PACKAGE_DESCRIPTION="Autotools practice for a static library with pkg-config"
AC_SUBST([PACKAGE_DESCRIPTION])

AC_ARG_WITH([pkgconfigdir],
  AC_HELP_STRING([--with-pkgconfigdir=DIR],
    [where to install pkgconfig file (default libdir/pkgconfig)]),
  [pkgconfigdir=${withval}], [pkgconfigdir='${libdir}/pkgconfig'])
AC_SUBST([pkgconfigdir])
AC_MSG_NOTICE([where to install pkgconfig file is ${pkgconfigdir}])

# Checks for programs.
AC_PROG_CXX
AC_PROG_RANLIB
AM_PROG_AR

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/libhello-world.pc])
AC_OUTPUT
