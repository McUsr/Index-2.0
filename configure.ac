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

# Checks for libraries. We'll generate ALL error messages at the end, if we couldn't find our stuff.
lib_search_failed=no
have_icui8n=yes
AC_CHECK_LIB([icui18n], [ucol_open_49],[],[have_icui8n=no])

if test "x${have_icui8n}" = xno; then
	lib_search_failed=yes
fi

have_icuuc=yes
AC_CHECK_LIB([icuuc], [u_errorName_49],[],[have_icuuc=no])
if test "x${have_icuuc}" = xno; then
	lib_search_failed=yes
fi

have_icuio=yes
AC_CHECK_LIB([icuio], [u_printf_49],[],[have_icuio=no])
if test "x${have_icuio}" = xno; then
	lib_search_failed=yes
fi
have_icudata=yes
AC_CHECK_LIB([icudata], [icudt49_dat],[],[have_icudata=no])
if test "x${have_icudata}" = xno; then
	lib_search_failed=yes
fi

have_ncursesw=yes
AC_CHECK_LIB([ncursesw], [refresh],[],[have_ncursesw=no])
if test "x${have_ncursesw}" = xno; then
	lib_search_failed=yes
fi

# post processes any error messages, as we'll have to abort if we didn't find what we were looking for.

if test "x${lib_search_failed}" = xyes; then
	echo "----------------------------------------------"
fi
if test "x${have_icui8n}" = xno; then
	echo "unable to find libicui8n on this system."
fi

if test "x${have_icuuc}" = xno; then
	echo "unable to find libicuuc on this system."
fi
if test "x${have_icuio}" = xno; then
	echo "unable to find libicuio on this system."
fi

if test "x${have_icudata}" = xno; then
	echo "unable to find libicudata on this system."
fi

if test "x${have_ncursesw}" = xno; then
	echo "unable to find libncursesw on this system."
fi
if test "x${lib_search_failed}" = xyes; then
	AC_MSG_ERROR([You must either Install the missing libraries,
or specify correct path to your libraries with LDFLAGS, see configure --help
----------------------------------------------])
fi
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

echo \
"-------------------------------------------------

${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'.
Compiler: '${CC}
C-flags:
${CFLAGS}
Cpp-flags:
${CPPFLAGS}
Ld-flags:
${LDFLAGS}'

Now type 'make @<:@<target>@:>@'
where the optional <target> is:
all                - build all binaries
install            - install everything

--------------------------------------------------"
