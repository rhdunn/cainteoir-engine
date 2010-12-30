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
dnl getopt checks.
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

dnl ================================================================
dnl libmagic checks.
dnl ================================================================

AC_CHECK_HEADERS(magic.h,
    [
        MAGIC_CFLAGS=""
        MAGIC_LIBS="-lmagic"
    ], [AC_MSG_ERROR([libmagic is required.])])

AC_SUBST(MAGIC_CFLAGS)
AC_SUBST(MAGIC_LIBS)

dnl ================================================================
dnl libxml2 checks.
dnl ================================================================

PKG_CHECK_MODULES(LIBXML2, [libxml-2.0 >= 2.0])
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

dnl ================================================================
dnl libzip checks.
dnl ================================================================

PKG_CHECK_MODULES(LIBZIP, [libzip >= 0.9])
AC_SUBST(LIBZIP_CFLAGS)
AC_SUBST(LIBZIP_LIBS)

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
dnl PulseAudio checks.
dnl ================================================================

PKG_CHECK_MODULES(PULSE, [libpulse-simple >= 0.9])
AC_SUBST(PULSE_CFLAGS)
AC_SUBST(PULSE_LIBS)

dnl ================================================================
dnl Ogg/Vorbis checks.
dnl ================================================================

AC_ARG_WITH([vorbisenc],
    [AS_HELP_STRING([--with-vorbisenc], [support for vorbisenc Ogg/Vorbis encoder @<:@default=yes@:>@])],
    [])

if test "$with_vorbisenc" = "no"; then
    echo "Disabling Ogg/Vorbis support via vorbisenc"
    have_vorbisenc=no
else
    PKG_CHECK_MODULES(VORBIS, [vorbisenc >= 1.2],
        [
            AC_DEFINE(HAVE_VORBISENC, [], [Do we have vorbisenc])
            have_vorbisenc=yes
        ],[
            have_vorbisenc=no
        ])
fi

AC_SUBST(VORBIS_CFLAGS)
AC_SUBST(VORBIS_LIBS)

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

