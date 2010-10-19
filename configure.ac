AC_PREREQ([2.65])
AC_INIT([Cainteoir Text-to-Speech], [0.1], [msclrhd@gmail.com], [cainteoir-engine], [https://github.com/rhdunn/cainteoir-engine])
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl ================================================================
dnl getopt
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

dnl ================================================================
dnl espeak tts engine checks.
dnl ================================================================

AC_CHECK_HEADERS(espeak/speak_lib.h,
    [
        ESPEAK_CFLAGS="-DHAVE_ESPEAK"
        ESPEAK_LIBS="-lespeak"
    ])

AC_SUBST(ESPEAK_CFLAGS)
AC_SUBST(ESPEAK_LIBS)

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

