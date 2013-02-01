AC_INIT([WOrspace Change Display], [1], [temp.email@gmail.com])
AC_CONFIG_SRCDIR([src/workspace-display-change.c])
AM_INIT_AUTOMAKE([-Wall foreign])
#AM_SILENT_RULES([yes])


AC_PROG_CC
PKG_PROG_PKG_CONFIG


PKG_CHECK_MODULES([WOCHDISP], [
    gtk+-2.0
    libwnck-1.0
])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_OUTPUT



