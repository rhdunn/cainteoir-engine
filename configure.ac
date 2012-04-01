AC_PREREQ([2.65])
AC_INIT([Cainteoir Text-to-Speech], [0.7], [msclrhd@gmail.com], [cainteoir-engine], [https://github.com/rhdunn/cainteoir-engine])
AM_INIT_AUTOMAKE()

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.17)

LT_INIT

dnl ================================================================
dnl Program checks.
dnl ================================================================

AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl C++11 language support
dnl ================================================================

AC_LANG_PUSH(C++)

AX_CHECK_COMPILE_FLAG(
	[-std=c++0x],
	[CXXFLAGS="$CXXFLAGS -std=c++0x"],
	[AC_MSG_ERROR([Cannot detect a compiler configuration that supports C++11.])])

AC_MSG_CHECKING([if $CXX supports C++11 nullptr])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[[]],
		[[const char *ptr = nullptr;]])],
	[AC_MSG_RESULT([yes])
	 AC_DEFINE([HAVE_CPP_NULLPTR],[1],[Define 1 if the nullptr keyword is not supported.])],
	[AC_MSG_RESULT([no])])

AC_LANG_POP(C++)

dnl ================================================================
dnl getopt checks.
dnl ================================================================

AC_CHECK_HEADERS([getopt.h])
AC_CHECK_FUNCS([getopt_long])

dnl ================================================================
dnl iconv checks.
dnl ================================================================

AC_CHECK_HEADERS([iconv.h])
AC_CHECK_FUNCS([iconv_open])

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
dnl zlib checks.
dnl ================================================================

PKG_CHECK_MODULES(ZLIB, [zlib >= 1.2.3], [], [
    # Fallback for distributions that don't install zlib.pc (e.g. Ubuntu Lucid) ...
    ZLIB_HOME=/usr/local
    if test ! -f "${ZLIB_HOME}/include/zlib.h"
    then
        ZLIB_HOME=/usr
    fi
    ZLIB_OLD_LDFLAGS=$LDFLAGS
    ZLIB_OLD_CPPFLAGS=$LDFLAGS
    LDFLAGS="$LDFLAGS -L${ZLIB_HOME}/lib"
    CPPFLAGS="$CPPFLAGS -I${ZLIB_HOME}/include"
    # Check for zlib ...
    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_LIB(z, inflateEnd, [zlib_cv_libz=yes], [zlib_cv_libz=no])
    AC_CHECK_HEADER(zlib.h, [zlib_cv_zlib_h=yes], [zlib_cv_zlib_h=no])
    AC_LANG_RESTORE
    # And determine if zlib is present and working ...
    AC_MSG_CHECKING(zlib in ${ZLIB_HOME})
    LDFLAGS="$ZLIB_OLD_LDFLAGS"
    CPPFLAGS="$ZLIB_OLD_CPPFLAGS"
    if test "$zlib_cv_libz" = "yes" -a "$zlib_cv_zlib_h" = "yes"
    then
        AC_MSG_RESULT(yes)
        ZLIB_CFLAGS="-L${ZLIB_HOME}/include"
        ZLIB_LIBS="-L${ZLIB_HOME}/lib"
    else
        AC_MSG_RESULT(no)
        AC_MSG_ERROR(could not locate the zlib package)
    fi
])

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
dnl MIME Type Integration.
dnl ================================================================

AC_ARG_WITH([xdgdatadir],
    [AS_HELP_STRING([--with-xdgdatadir=path], [Change where the theme icons and mime registrations are installed [DATADIR]])],
    [opt_xdgdatadir=$withval])

if test x$opt_xdgdatadir = x; then
        XDGDATADIR='${datadir}'
else
        XDGDATADIR="$opt_xdgdatadir"
fi
AC_SUBST(XDGDATADIR)

AC_ARG_ENABLE(update-mime-database,
    AC_HELP_STRING([--disable-update-mime-database],[do not update mime database after installation]),,
    enable_update_mime_database=yes)
AM_CONDITIONAL(ENABLE_UPDATE_MIME_DATABASE, test x$enable_update_mime_database = xyes)

# update-mime-info is required to run the tests, so always look for it ...
AC_PATH_PROG(UPDATE_MIME_DATABASE, [update-mime-database], no)
if test $UPDATE_MIME_DATABASE = no; then
    AC_MSG_ERROR([Cannot find update-mime-database, make sure it is installed and in your PATH])
fi

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([Makefile po/Makefile.in])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for Cainteoir Text-to-Speech engine complete.

        Source code location:          ${srcdir}
        XDG data location:             ${XDGDATADIR}

        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}

        eSpeak support:                ${have_espeak}
        Ogg/Vorbis support:            ${have_vorbisenc}
])
