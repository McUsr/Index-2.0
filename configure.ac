AC_PREREQ([2.69])
AC_INIT([index],[2.0],[https://github.com/McUsr/Index-2.0/issues])
AC_CONFIG_SRCDIR([src/common.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# preliminiary convenience
CFLAGS="$CFLAGS -I/opt/local/include -I/usr/include"
LDFLAGS="$LDFLAGS -L/opt/local/lib -L/usr/lib"

# Checks for libraries.
AC_CHECK_LIB([icui18n], [ucol_open_49],[],[AC_MSG_ERROR([I couldnt find the ICU icui8n library. Misses correct Library path.])])

AC_CHECK_LIB([icuuc], [u_errorName_49],[],[AC_MSG_ERROR([I couldnt find the ICU iuuc library. Misses correct Library path.])])

AC_CHECK_LIB([icuio], [u_printf_49],[],[AC_MSG_ERROR([I couldnt find the ICU icuio library. Misses correct Library path.])])

AC_CHECK_LIB([icudata], [icudt49_dat],[],[AC_MSG_ERROR([I couldnt find the ICU icudata library. Misses correct Library path.])])

AC_CHECK_LIB([ncursesw], [refresh],[],[AC_MSG_ERROR([I couldnt find the  ncursesw library. Misses correct Library path.])])


# Checks for header files.
AC_CHECK_HEADERS([locale.h stddef.h stdlib.h string.h sys/file.h sys/param.h unistd.h wchar.h wctype.h])


# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_MODE_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([getcwd memset realpath setlocale strchr strerror strrchr strstr])
AC_CONFIG_FILES([Makefile
src/Makefile])

AC_OUTPUT
