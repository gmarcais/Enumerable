AC_INIT([enumerable], [0.0.1], [gmarcais@umd.edu])
AC_CANONICAL_HOST
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects foreign parallel-tests color-tests])
AM_SILENT_RULES([yes])
AC_CONFIG_SRCDIR([.])
AC_CONFIG_HEADERS([config.h])

# Change default compilation flags
AC_SUBST([ALL_CXXFLAGS], [-std=c++1y])
CXXFLAGS="-std=c++1y $CXXFLAGS"
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
