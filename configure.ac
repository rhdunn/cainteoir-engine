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
dnl espeak tts engine checks.
dnl ================================================================

AC_ARG_WITH([espeak],
    [AS_HELP_STRING([--with-espeak[[=DIR]]], [support for the eSpeak TTS engine @<:@default=yes@:>@])],
    [ case "$withval" in
        n|no)
            ;;
        y|yes)
            ESPEAK_DIR=/usr
            CPPFLAGS="${CPPFLAGS} -I/usr/include"
            ;;
        *)
            ESPEAK_DIR=$withval
            CPPFLAGS="${CPPFLAGS} -I$withval/include"
            ;;
    esac ])

if test "$with_espeak" = "no"; then
    echo "Disabling eSpeak Text-to-Speech voice support"
    have_espeak=no
else
    AC_CHECK_HEADERS(espeak/speak_lib.h,
        [
            AC_DEFINE(HAVE_ESPEAK, [], [Do we have eSpeak])
            if test "x${ESPEAK_DIR}" != "x"; then
                ESPEAK_CFLAGS="-I${ESPEAK_DIR}/include"
                ESPEAK_LIBS="-L${ESPEAK_DIR}/lib -lespeak"
            else
                ESPEAK_LIBS="-lespeak"
            fi
            have_espeak=yes
        ],[
            have_espeak=no
        ])
fi

AC_SUBST(ESPEAK_CFLAGS)
AC_SUBST(ESPEAK_LIBS)

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

