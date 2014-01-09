AC_PREREQ([2.65])
AC_INIT([Cainteoir Text-to-Speech], [0.11], [https://github.com/rhdunn/cainteoir-engine/issues], [cainteoir-engine], [https://github.com/rhdunn/cainteoir-engine])
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

AC_PROG_CPP
AC_PROG_CXX
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

dnl ================================================================
dnl Python checks.
dnl ================================================================

AC_DEFUN([PYTHON2_CHECK],
    [
        AC_MSG_CHECKING([for $1])
        AS_IF([which $1 > /dev/null],
            [
                VERSION=`$1 --version 2>&1 | sed -e 's/Python //g'`
                MAJOR_VERSION=`echo ${VERSION} | sed -e 's/\..*//g'`
                AC_MSG_RESULT([${VERSION}])
                AC_MSG_CHECKING([if $1 is a Python 2 interpreter])
                AS_IF([test ${MAJOR_VERSION} -eq 2],
                    [
                        AC_MSG_RESULT([yes])
                        PYTHON=$1
                    ],[
                        AC_MSG_RESULT([no])
                        PYTHON=no
                    ])
            ],[
                AC_MSG_RESULT([no])
                PYTHON=no
            ])
    ])

AC_ARG_VAR([PYTHON], [the python interpreter to use @<:@default=auto@:>@])
if test "x${PYTHON}" == "x" ; then
    # autodetect Python version ...
    PYTHON2_CHECK(python)
    if test "x${PYTHON}" == "xno" ; then
        PYTHON2_CHECK(python2)
    fi
else
    PYTHON2_CHECK(${PYTHON})
fi

if test "x${PYTHON}" == "xno" ; then
    AC_MSG_ERROR([unable to find a Python 2 interpreter (use ./configure PYTHON=... to point to the correct Python program)])
fi

AC_SUBST(PYTHON)

dnl ================================================================
dnl C++11 support
dnl ================================================================

CXX11_CHECK_CXX11_SUPPORT

CXX11_NULLPTR
CXX11_DECLTYPE
CXX11_AUTO
CXX11_RANGE_BASED_FOR
CXX11_BRACE_INITIALIZATION
CXX11_SCOPED_ENUMS
CXX11_CONSTEXPR
CXX11_LAMBDA
CXX11_LAMBDA_CAPTURE

CXX11_REQUIRE_STD_INITIALIZER_LIST
CXX11_REQUIRE_STD_SHARED_PTR

CXX11_REQUIRE_STD_BEGIN
CXX11_REQUIRE_STD_END

dnl ================================================================
dnl stopwatch checks.
dnl ================================================================

AC_CHECK_HEADERS([sys/time.h])
AC_CHECK_FUNCS([gettimeofday])

AC_CHECK_HEADERS([time.h])
AC_CHECK_FUNCS([time])

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
dnl stdlib checks.
dnl ================================================================

AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_FUNCS([setenv])
AC_CHECK_FUNCS([mkstemp])

dnl ================================================================
dnl stdio checks.
dnl ================================================================

AC_CHECK_HEADERS([stdio.h])
AC_CHECK_FUNCS([open_memstream])
AC_CHECK_FUNCS([tmpfile])

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
    ZLIB_HOME=${SYSROOT}/usr/local
    if test ! -f "${ZLIB_HOME}/include/zlib.h"
    then
        ZLIB_HOME=${SYSROOT}/usr
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

            # espeak_TextToPhonemes is implemented in eSpeak 1.47.07b and
            # declared in speak_lib.h since 1.47.08 for revision 8 of the
            # eSpeak API, but ESPEAK_API_REVISION was not updated. In
            # 1.47.11c, the method signature changed and ESPEAK_API_REVISION
            # was updated, but the declaration in speak_lib.h was not
            # updated.
            AC_MSG_CHECKING([for espeak_TextToPhonemes])
            AC_LANG_PUSH(C++)
            AC_COMPILE_IFELSE(
                [AC_LANG_PROGRAM(
                    [[#include "espeak/speak_lib.h"]],
                    [[const char * (*ptr)(const void **textptr, int textmode, int phonememode);]
                     [ptr = espeak_TextToPhonemes;]])
                ],[
                    AC_MSG_RESULT([yes])
                    AC_DEFINE(HAVE_ESPEAK_TEXTTOPHONEMES, [], [Does eSpeak have espeak_TextToPhonemes])
                ],[
                    AC_MSG_RESULT([no])
                ])
            AC_LANG_POP(C++)
        ],[
            have_espeak=no
        ])
fi

AC_SUBST(ESPEAK_CFLAGS)
AC_SUBST(ESPEAK_LIBS)

dnl ================================================================
dnl pico tts engine checks.
dnl ================================================================

AC_ARG_WITH([pico],
    [AS_HELP_STRING([--with-pico], [support for pico text-to-speech @<:@default=yes@:>@])],
    [])

AC_MSG_CHECKING([location of Pico voices])
if test "$with_pico" = "no"; then
    echo "Disabling pico support"
    have_pico=no
    PICO_LANG_DIR=
    AC_MSG_RESULT([disabled])
else
    AC_CHECK_HEADERS(picoapi.h,
        [
            PICO_CFLAGS="-DHAVE_PICO"
            PICO_LIBS="-lttspico"
            have_pico=yes
            if test "$with_pico" = "yes" -o "$with_pico" = "" ; then
                PICO_LANG_DIR=/usr/share/pico/lang
                AC_MSG_RESULT([${PICO_LANG_DIR} (default location)])
            else
                PICO_LANG_DIR=$with_pico
                AC_MSG_RESULT([${PICO_LANG_DIR}])
           fi
        ],[
            have_pico=no
            PICO_LANG_DIR=
            AC_MSG_RESULT([disabled])
        ])
fi

AC_SUBST(PICO_CFLAGS)
AC_SUBST(PICO_LIBS)
AC_SUBST(PICO_LANG_DIR)

dnl ================================================================
dnl MBROLA tts synthesizer checks.
dnl ================================================================

AC_ARG_WITH([mbrola],
    [AS_HELP_STRING([--with-mbrola], [support MBROLA text-to-speech voices where applicable @<:@default=yes@:>@])],
    [])

AC_MSG_CHECKING([location of MBROLA voices])
if test "$with_mbrola" = "no"; then
    echo "Disabling MBROLA support"
    have_mbrola=no
    MBROLA_DIR=
    AC_MSG_RESULT([disabled])
else
    AC_DEFINE([HAVE_MBROLA], [1], [Define if support for the MBROLA speech synthesizer and voices is enabled.])
    have_mbrola=yes
    if test "$with_mbrola" = "yes" -o "$with_mbrola" = "" ; then
        MBROLA_DIR=/usr/share/mbrola
        AC_MSG_RESULT([${MBROLA_DIR} (default location)])
    else
        MBROLA_DIR=$with_mbrola
        AC_MSG_RESULT([${MBROLA_DIR}])
    fi
fi

AC_SUBST(MBROLA_DIR)

dnl ================================================================
dnl PulseAudio checks.
dnl ================================================================

AC_ARG_WITH([pulseaudio],
    [AS_HELP_STRING([--with-pulseaudio], [support for PulseAudio output @<:@default=yes@:>@])],
    [])

if test "$with_pulseaudio" = "no"; then
    echo "Disabling PulseAudio output support"
    have_pulseaudio=no
else
    PKG_CHECK_MODULES(PULSE, [libpulse-simple >= 0.9],
        [
            AC_DEFINE(HAVE_PULSEAUDIO, [], [Do we have pulseaudio])
            have_pulseaudio=yes
        ],[
            have_pulseaudio=no
        ])
fi

AC_SUBST(PULSE_CFLAGS)
AC_SUBST(PULSE_LIBS)

dnl ================================================================
dnl ALSA checks.
dnl ================================================================

AC_ARG_WITH([alsa],
    [AS_HELP_STRING([--with-alsa], [support for ALSA audio output @<:@default=yes@:>@])],
    [])

if test "$with_alsa" = "no"; then
    echo "Disabling ALSA audio output support"
    have_alsa=no
else
    PKG_CHECK_MODULES(ALSA, [alsa],
        [
            AC_DEFINE(HAVE_ALSA, [], [Do we have ALSA])
            have_alsa=yes
        ],[
            have_alsa=no
        ])
fi

AC_SUBST(ALSA_CFLAGS)
AC_SUBST(ALSA_LIBS)

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
dnl Poppler checks.
dnl ================================================================

AC_ARG_WITH([poppler],
    [AS_HELP_STRING([--with-poppler], [support for PDF files using Poppler @<:@default=yes@:>@])],
    [])

if test "$with_poppler" = "no"; then
    echo "Disabling PDF support via poppler"
    have_poppler=no
else
    PKG_CHECK_MODULES(POPPLER, [poppler-glib >= 0.16],
        [
            AC_DEFINE(HAVE_POPPLER, [], [Do we have poppler glib bindings])
            have_poppler=yes
        ],[
            have_poppler=no
        ])
fi

AC_SUBST(POPPLER_CFLAGS)
AC_SUBST(POPPLER_LIBS)

dnl ================================================================
dnl FFmpeg/libav checks.
dnl ================================================================

AC_ARG_WITH([ffmpeg],
    [AS_HELP_STRING([--with-ffmpeg], [support ePub Media Overlay audio using FFmpeg/libav @<:@default=yes@:>@])],
    [])

if test "$with_ffmpeg" = "no"; then
    echo "Disabling ePub Media Overlay support via the FFmpeg/libav library"
    have_ffmpeg=no
else
    PKG_CHECK_MODULES(FFMPEG, [libavformat >= 0.9 libavcodec >= 0.9 libavutil >= 0.9],
        [
            AC_DEFINE(HAVE_FFMPEG, [], [Do we have the FFmpeg libraries])
            have_ffmpeg=yes

            # API compatibility checks ...

            CFLAGS="$CFLAGS $FFMPEG_CFLAGS"
            LIBS="$LIBS $FFMPEG_LIBS"

            AC_CHECK_FUNCS([av_frame_alloc])
            AC_CHECK_FUNCS([avcodec_alloc_frame])
        ],[
            have_ffmpeg=no
        ])

    PKG_CHECK_MODULES(AVRESAMPLE, [libavresample],
        [
            AC_DEFINE(HAVE_LIBAVRESAMPLE, [], [Do we have the libavresample library])
            have_avresample=yes
        ],[
            have_avresample=no
        ])
fi

AC_SUBST(FFMPEG_CFLAGS)
AC_SUBST(FFMPEG_LIBS)

AC_SUBST(AVRESAMPLE_CFLAGS)
AC_SUBST(AVRESAMPLE_LIBS)

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
dnl Doxygen documentation
dnl ================================================================

AC_CHECK_PROG(HAVE_DOT,dot,yes,no)
AC_SUBST(HAVE_DOT)

dnl ================================================================
dnl Compiler/linker hardening and security
dnl ================================================================

AC_ARG_ENABLE([all-warnings],
    AS_HELP_STRING([--enable-all-warnings], [Turn on all compiler warnings]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wconversion"
     fi])

AC_ARG_ENABLE([format-security],
    AS_HELP_STRING([--format-security], [Turn on printf format security checks]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -Wformat -Wformat-security -Werror=format-security"
     fi])

AC_ARG_ENABLE([stack-protection],
    AS_HELP_STRING([--enable-stack-protection], [Use stack protection to protect against stack smashing]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -fstack-protector-all -Wstack-protector --param ssp-buffer-size=4"
     fi])

AC_ARG_ENABLE([pie],
    AS_HELP_STRING([--enable-pie], [Generate a Position Independent Executable (PIE)]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -pie -fPIE"
     fi])

AC_ARG_ENABLE([signed-overflow-checks],
    AS_HELP_STRING([--enable-signed-overflow-checks], [Generate traps for signed overflows]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -ftrapv"
     fi])

AC_ARG_ENABLE([buffer-checks],
    AS_HELP_STRING([--enable-buffer-checks], [Generate checks for buffer overflows]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -D_FORTIFY_SOURCE=2 -O2"
     fi])

AC_ARG_ENABLE([relro],
    AS_HELP_STRING([--enable-relro], [Prevent certain ELF sections being written to by the application]),
    [if test x$enableval == xyes ; then
         CXXFLAGS="$CXXFLAGS -Wl,-z,relro,-z,now"
     fi])

dnl ================================================================
dnl Generate output.
dnl ================================================================

AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
	docs/cainteoir-engine.doxyconf])
AC_OUTPUT

AC_MSG_NOTICE([

    Configuration for Cainteoir Text-to-Speech engine complete.

        Source code location:          ${srcdir}
        XDG data location:             ${XDGDATADIR}

        Compiler:                      ${CXX}
        Compiler flags:                ${CXXFLAGS}
        Python interpreter:            ${PYTHON}

        eSpeak support:                ${have_espeak}
        MBROLA support:                ${have_mbrola}	(voice directory: ${MBROLA_DIR})
        Pico support:                  ${have_pico}	(voice directory: ${PICO_LANG_DIR})
        PulseAudio support:            ${have_pulseaudio}
        ALSA support:                  ${have_alsa}
        Ogg/Vorbis support:            ${have_vorbisenc}
        PDF support:                   ${have_poppler}
        FFmpeg/libav support:          ${have_ffmpeg}
        libavresample support:         ${have_avresample}
])
