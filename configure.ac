AC_INIT([index],[2.0])
AC_PROG_CC
AC_CONFIG_FILES([Makefile
src/Makefile])
CFLAGS="$CFLAGS -I/opt/local/include -I/usr/include"
LDFLAGS="$LDFLAGS -L/opt/local/lib -L/usr/lib"
AC_CHECK_LIB([icui18n], [ucol_open_49],[],[AC_MSG_ERROR([I couldn't find the ICU icui8n library. Misses correct Library path.])])
AC_CHECK_LIB([icuuc], [u_errorName_49],[],[AC_MSG_ERROR([I couldn't find the ICU iuuc library. Misses correct Library path.])])
AC_CHECK_LIB([icuio], [u_printf_49],[],[AC_MSG_ERROR([I couldn't find the ICU icuio library. Misses correct Library path.])])
AC_CHECK_LIB([icudata], [icudt49_dat],[],[AC_MSG_ERROR([I couldn't find the ICU icudata library. Misses correct Library path.])])
AC_CHECK_LIB([ncursesw], [refresh],[],[AC_MSG_ERROR([I couuldn't find the  ncursesw library. Misses correct Library path.])])
AC_OUTPUT
