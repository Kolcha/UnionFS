AC_PREREQ([2.69])
AC_INIT([unityfs], [2.3.0])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([unityfs.h])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC
AC_PROG_SED

# Checks for libraries.
PKG_CHECK_MODULES(fuse, fuse >= 2.6)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h sys/statvfs.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([fchdir ftruncate getcwd memset mkdir rmdir strcasecmp strchr strdup])

AC_ARG_ENABLE([systemd],
    [AS_HELP_STRING([--enable-systemd], [install the systemd service file])], [], [enable_systemd=no])
AM_CONDITIONAL([install_systemd_unit], [test "x$enable_systemd" != "xno"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
