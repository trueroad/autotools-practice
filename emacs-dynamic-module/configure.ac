#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hello-emacs-module], [0.0.0], [test@example.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/hello-module.c])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIRS([m4])

# Checks for programs.
AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = xno],
  AC_MSG_ERROR([C99 compiler is not found.]))
AM_PROG_AR
AM_PATH_LISPDIR
AC_ARG_WITH([pkglispdir],
  AS_HELP_STRING([--with-pkglispdir=DIR],
    [where to install emacs dynamic modules (default lispdir/PACKAGE)]),
  [pkglispdir=${withval}], [pkglispdir='${lispdir}/${PACKAGE}'])
AC_SUBST([pkglispdir])
AC_MSG_NOTICE([where to install emacs dynamic modules is ${pkglispdir}])

# Checks for libraries.
LT_INIT([disable-static shared win32-dll])
SHREXT=${shrext_cmds}
AC_SUBST([SHREXT])
AC_SUBST([objdir])

# Checks for header files.
AC_CHECK_HEADERS([emacs-module.h],,
  AC_MSG_ERROR([emacs-module.h is not found.]))
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
