AC_PREREQ([2.68])
AC_INIT([bline], [0.2.1], [dirleyrls@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11.1 foreign -Wall -Werror dist-bzip2])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AM_PATH_GTK_2_0([2.24.0], , AC_MSG_ERROR([Gtk+ 2.24.0 or higher required.]))
AM_PATH_GLIB_2_0(, , , gmodule)

PKG_CHECK_MODULES([OPENCV], [opencv >= 2.3.1])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_OUTPUT
