AC_PREREQ([2.69])
AC_INIT([unityfs], [2.0.1])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_SRCDIR([unityfs.h])

AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(fuse3, fuse3 >= 3.2)

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
AC_CHECK_FUNCS([ftruncate memset mkdir rmdir strdup])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
