AC_PREREQ([2.65])
AC_INIT([Cainteoir Text-to-Speech], [0.2], [msclrhd@gmail.com], [cainteoir-engine], [https://github.com/rhdunn/cainteoir-engine])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.18)

LT_INIT

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl getopt checks.
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

dnl ================================================================
dnl pthread checks.
dnl ================================================================

AC_CHECK_HEADERS([pthread.h])
AC_CHECK_LIB( pthread, pthread_create, [],
    [LDFLAGS="-pthread $LDFLAGS"
     AC_TRY_LINK([char pthread_create();],
         pthread_create();,
         [], [AC_MSG_ERROR([Missing pthread])])
    ])

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
dnl zlib checks.
dnl ================================================================

PKG_CHECK_MODULES(ZLIB, [zlib >= 1.2.3])
AC_SUBST(ZLIB_CFLAGS)
AC_SUBST(ZLIB_LIBS)

dnl ================================================================
dnl espeak tts engine checks.
dnl ================================================================

AC_ARG_WITH([espeak],
    [AS_HELP_STRING([--with-espeak], [support for espeak text-to-speech @<:@default=yes@:>@])],
    [])

if test "$with_espeak" = "no"; then
    echo "Disabling espeak support"
    have_espeak=no
else
    AC_CHECK_HEADERS(espeak/speak_lib.h,
        [
            ESPEAK_CFLAGS="-DHAVE_ESPEAK"
            ESPEAK_LIBS="-lespeak"
            have_espeak=yes
        ],[
            have_espeak=no
        ])
fi

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

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for Cainteoir Text-to-Speech engine complete.

        Source code location:          ${srcdir}
        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}

        eSpeak support:                ${have_espeak}
        Ogg/Vorbis support:            ${have_vorbisenc}
])
