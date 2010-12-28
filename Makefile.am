AUTOMAKE_OPTIONS = subdir-objects

AM_LDFLAGS  = ${PULSE_LIBS}   ${VORBIS_LIBS}   ${MAGIC_LIBS}   ${LIBXML2_LIBS}   ${ESPEAK_LIBS}
AM_CXXFLAGS = ${PULSE_CFLAGS} ${VORBIS_CFLAGS} ${MAGIC_CFLAGS} ${LIBXML2_CFLAGS} ${ESPEAK_CFLAGS} -Isrc/include

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
	src/libcainteoir/rdf/formatter.cpp \
	src/libcainteoir/pulse_audio.cpp \
	src/libcainteoir/wav_audio.cpp \
	src/libcainteoir/ogg_audio.cpp \
	src/libcainteoir/espeak_engine.cpp \
	src/libcainteoir/mmap_buffer.cpp

############################# applications ####################################

bin_PROGRAMS += src/apps/cainteoir/cainteoir

src_apps_cainteoir_cainteoir_LDADD   = src/libcainteoir/libcainteoir.a
src_apps_cainteoir_cainteoir_SOURCES = src/apps/cainteoir/main.cpp

bin_PROGRAMS += src/apps/metadata/metadata

src_apps_metadata_metadata_LDADD   = src/libcainteoir/libcainteoir.a
src_apps_metadata_metadata_SOURCES = src/apps/metadata/main.cpp

############################# metadata test ###################################

bin_PROGRAMS += tests/rdfmodel

tests_rdfmodel_LDADD   = src/libcainteoir/libcainteoir.a
tests_rdfmodel_SOURCES = tests/rdfmodel.cpp

bin_PROGRAMS += tests/ntriple_formatter

tests_ntriple_formatter_LDADD   = src/libcainteoir/libcainteoir.a
tests_ntriple_formatter_SOURCES = tests/ntriple_formatter.cpp

bin_PROGRAMS += tests/turtle_formatter

tests_turtle_formatter_LDADD   = src/libcainteoir/libcainteoir.a
tests_turtle_formatter_SOURCES = tests/turtle_formatter.cpp

############################# tests ###########################################

TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

TESTS = \
	tests/metadata-rdfresource.py \
	tests/metadata-rdfliteral.py \
	tests/metadata-rdftriple.py \
	tests/opf-metadata.py \
	tests/rdfmodel \
	tests/ntriple_formatter \
	tests/turtle_formatter

