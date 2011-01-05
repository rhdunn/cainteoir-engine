AUTOMAKE_OPTIONS = subdir-objects

AM_LDFLAGS  = ${PULSE_LIBS}   ${VORBIS_LIBS}   ${MAGIC_LIBS}   ${LIBXML2_LIBS}   ${LIBZIP_LIBS}   ${ESPEAK_LIBS}
AM_CXXFLAGS = ${PULSE_CFLAGS} ${VORBIS_CFLAGS} ${MAGIC_CFLAGS} ${LIBXML2_CFLAGS} ${LIBZIP_CFLAGS} ${ESPEAK_CFLAGS} -Isrc/include

AM_CXXFLAGS += -std=c++0x

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

EXTRA_DIST += data/magic

pkgdata_DATA = data/magic.mgc

data/magic.mgc: data/magic
	cd data && file -C -m magic

noinst_LIBRARIES += src/libcainteoir/libcainteoir.a

src_libcainteoir_libcainteoir_a_SOURCES = \
	src/libcainteoir/buffers/data_buffer.cpp \
	src/libcainteoir/buffers/mmap_buffer.cpp \
	src/libcainteoir/buffers/xmlstring_buffer.cpp \
	src/libcainteoir/rdf/namespaces.cpp \
	src/libcainteoir/rdf/formatter.cpp \
	src/libcainteoir/parsers/xml.cpp \
	src/libcainteoir/parsers/xhtml.cpp \
	src/libcainteoir/parsers/ocf.cpp \
	src/libcainteoir/parsers/opf.cpp \
	src/libcainteoir/parsers/epub.cpp \
	src/libcainteoir/parsers/rdfxml.cpp \
	src/libcainteoir/parsers/smil.cpp \
	src/libcainteoir/parsers/document.cpp \
	src/libcainteoir/audio/pulse.cpp \
	src/libcainteoir/audio/wav.cpp \
	src/libcainteoir/audio/ogg.cpp \
	src/libcainteoir/tts/engines.cpp \
	src/libcainteoir/tts/espeak.cpp

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

# RDF API tests:
TESTS = \
	tests/rdfmodel \
	tests/ntriple_formatter \
	tests/turtle_formatter

# RDF metadata extraction tests:
TESTS += \
	tests/opf-metadata.py \
	tests/epub-metadata.py \
	tests/rdfxml-metadata.py \
	tests/smil-metadata.py \
	tests/xhtml-metadata.py

# RDF metadata to audio specific metadata conversion tests:
TESTS += \
	tests/vorbis-comments.py

