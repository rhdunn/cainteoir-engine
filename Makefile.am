AUTOMAKE_OPTIONS = subdir-objects

AM_LDFLAGS  = ${PULSE_LIBS}   ${VORBIS_LIBS}   ${ESPEAK_LIBS}
AM_CXXFLAGS = ${PULSE_CFLAGS} ${VORBIS_CFLAGS} ${ESPEAK_CFLAGS} -Isrc/include

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
	src/libcainteoir/pulse_audio.cpp \
	src/libcainteoir/wav_audio.cpp \
	src/libcainteoir/ogg_audio.cpp \
	src/libcainteoir/espeak_engine.cpp \
	src/libcainteoir/mmap_buffer.cpp

############################# cainteoir #######################################

bin_PROGRAMS += src/apps/cainteoir/cainteoir

src_apps_cainteoir_cainteoir_LDADD = \
	src/libcainteoir/libcainteoir.a

src_apps_cainteoir_cainteoir_SOURCES = \
	src/apps/cainteoir/main.cpp

############################# metadata test ###################################

bin_PROGRAMS += tests/rdfmodel

tests_rdfmodel_LDADD = \
	src/libcainteoir/libcainteoir.a

tests_rdfmodel_SOURCES = \
	tests/rdfmodel.cpp

############################# tests ###########################################

TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

TESTS = \
	tests/metadata-rdfresource.py \
	tests/metadata-rdfliteral.py \
	tests/metadata-rdftriple.py \
	tests/opf-metadata.py \
	tests/rdfmodel

