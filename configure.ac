AC_PREREQ([2.68])
AC_INIT([bline], [0.3.0], [dirleyrls@gmail.com])
AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE([1.11.1 foreign -Wall -Werror dist-bzip2])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB

AM_PATH_GLIB_2_0(2.33.14, :, :, gobject gthread)
PKG_CHECK_MODULES([GEGL], [gegl-0.2 >= 0.2.0])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/gegl-operations/Makefile
])

AC_OUTPUT
