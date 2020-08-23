#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([hello-elisp], [0.0.0], [test@example.org])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AM_PATH_LISPDIR
AC_ARG_WITH([pkglispdir],
  AS_HELP_STRING([--with-pkglispdir=DIR],
    [where to install lisp files (default lispdir/PACKAGE)]),
  [pkglispdir=${withval}], [pkglispdir='${lispdir}/${PACKAGE}'])
AC_SUBST([pkglispdir])
AC_MSG_NOTICE([where to install lisp files is ${pkglispdir}])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 lisp/Makefile])
AC_OUTPUT
