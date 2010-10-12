AUTOMAKE_OPTIONS = subdir-objects

AM_LDFLAGS  = ${ESPEAK_LIBS}
AM_CXXFLAGS = ${ESPEAK_CFLAGS} -Isrc/include

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
noinst_LIBRARIES =

EXTRA_DIST = config.rpath
CLEANFILES =

############################# ChangeLog #######################################

ChangeLog:
	git log > ChangeLog

dist-hook: ChangeLog

.PHONY: ChangeLog

EXTRA_DIST += ChangeLog

############################# libcainteoir ####################################

noinst_LIBRARIES += src/libcainteoir/libcainteoir.a

src_libcainteoir_libcainteoir_a_SOURCES = \
	src/libcainteoir/mmap_buffer.cpp

############################# cainteoir #######################################

bin_PROGRAMS += src/apps/cainteoir/cainteoir

src_apps_cainteoir_cainteoir_LDADD = \
	src/libcainteoir/libcainteoir.a

src_apps_cainteoir_cainteoir_SOURCES = \
	src/apps/cainteoir/main.cpp

