AUTOMAKE_OPTIONS = subdir-objects

localedir = $(datadir)/locale

AM_LDFLAGS  = ${LTLIBINTL}
AM_CXXFLAGS = -Isrc/include ${LIBXML2_CFLAGS} \
	-std=c++0x -DDATADIR='"$(datadir)"' -DLOCALEDIR=\"$(localedir)\"

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
lib_LTLIBRARIES =
man1_MANS =
noinst_bin_PROGRAMS =
noinst_LIBRARIES =

noinst_bindir =

EXTRA_DIST = config.rpath m4/ChangeLog
CLEANFILES =
SUBDIRS = po

############################# libtool #########################################

EXTRA_DIST += config.guess config.sub ltconfig ltmain.sh

# Increment if the interface has changed and is backward compatible
CURRENT=0

# Increment  if source files have changed
# Reset to 0 if the interface has changed
REVISION=0

# Increment  if the interface is backward compatible (superset)
# Reset to 0 if the interface is not backward compatible
AGE=0

LIBCAINTEOIR_VERSION=$(CURRENT):$(REVISION):$(AGE)

############################# gettext #########################################

EXTRA_DIST += mkinstalldirs m4/Makefile.in src/gettext.h

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	@find $(srcdir)/src/ -name "*.cpp" -print | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo-%: check-gettext
	@language=`echo $@ | sed s/force-update-gmo-//` ; \
	if test ! -f po/$$language.po ; then echo "file po/$$language.po does not exist" ; exit 1 ; fi ; \
	touch po/$$language.po ; \
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

.PHONY: check-gettext update-po update-gmo force-update-gmo

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

libcainteoir_includedir = $(includedir)/cainteoir
libcainteoir_include_HEADERS = \
	src/include/cainteoir/platform.hpp \
	src/include/cainteoir/audio.hpp \
	src/include/cainteoir/buffer.hpp \
	src/include/cainteoir/xmlreader.hpp \
	src/include/cainteoir/document.hpp \
	src/include/cainteoir/engines.hpp \
	src/include/cainteoir/metadata.hpp

lib_LTLIBRARIES += src/libcainteoir/libcainteoir.la

src_libcainteoir_libcainteoir_la_LDFLAGS = -version-info $(LIBCAINTEOIR_VERSION) \
	${PULSE_LIBS} \
	${VORBIS_LIBS} \
	${MAGIC_LIBS} \
	${LIBXML2_LIBS} \
	${ZLIB_LIBS} \
	${ESPEAK_LIBS} \
	${LTLIBINTL}

src_libcainteoir_libcainteoir_la_CXXFLAGS = ${AM_CXXFLAGS} \
	${PULSE_CFLAGS} \
	${VORBIS_CFLAGS} \
	${MAGIC_CFLAGS} \
	${ZLIB_CFLAGS} \
	${ESPEAK_CFLAGS}

src_libcainteoir_libcainteoir_la_SOURCES = \
	src/libcainteoir/buffers/data_buffer.cpp \
	src/libcainteoir/buffers/mmap_buffer.cpp \
	src/libcainteoir/buffers/xmlstring_buffer.cpp \
	src/libcainteoir/rdf/namespaces.cpp \
	src/libcainteoir/rdf/formatter.cpp \
	src/libcainteoir/parsers/mimetypes.hpp \
	src/libcainteoir/parsers/parsers.hpp \
	src/libcainteoir/parsers/compressed.cpp \
	src/libcainteoir/parsers/xml.hpp \
	src/libcainteoir/parsers/zip.hpp \
	src/libcainteoir/parsers/zip.cpp \
	src/libcainteoir/parsers/xmlreader.cpp \
	src/libcainteoir/parsers/html-entities.h \
	src/libcainteoir/parsers/xml.cpp \
	src/libcainteoir/parsers/xhtml.cpp \
	src/libcainteoir/parsers/ocf.cpp \
	src/libcainteoir/parsers/opf.cpp \
	src/libcainteoir/parsers/epub.cpp \
	src/libcainteoir/parsers/rdfxml.cpp \
	src/libcainteoir/parsers/smil.cpp \
	src/libcainteoir/parsers/document.cpp \
	src/libcainteoir/audio/audio.cpp \
	src/libcainteoir/audio/pulse.cpp \
	src/libcainteoir/audio/wav.cpp \
	src/libcainteoir/audio/ogg.cpp \
	src/libcainteoir/tts/tts_engine.hpp \
	src/libcainteoir/tts/engines.cpp \
	src/libcainteoir/tts/espeak.cpp

############################# applications ####################################

bin_PROGRAMS += src/apps/cainteoir/cainteoir
man1_MANS += src/apps/cainteoir/cainteoir.man

src_apps_cainteoir_cainteoir_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_cainteoir_cainteoir_SOURCES = src/apps/cainteoir/main.cpp

bin_PROGRAMS += src/apps/metadata/metadata
man1_MANS += src/apps/metadata/metadata.man

src_apps_metadata_metadata_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_metadata_metadata_SOURCES = src/apps/metadata/main.cpp

############################# tests ###########################################

noinst_bin_PROGRAMS += tests/rdfmodel

tests_rdfmodel_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfmodel_SOURCES = tests/rdfmodel.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/ntriple_formatter

tests_ntriple_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_ntriple_formatter_SOURCES = tests/ntriple_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/turtle_formatter

tests_turtle_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_turtle_formatter_SOURCES = tests/turtle_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/events

tests_events_LDADD   = src/libcainteoir/libcainteoir.la
tests_events_SOURCES = tests/events.cpp

noinst_bin_PROGRAMS += tests/xmlreader

tests_xmlreader_LDADD   = src/libcainteoir/libcainteoir.la
tests_xmlreader_SOURCES = tests/xmlreader.cpp

noinst_bin_PROGRAMS += tests/voicesynth

tests_voicesynth_LDADD   = src/libcainteoir/libcainteoir.la
tests_voicesynth_SOURCES = tests/voicesynth.cpp

EXTRA_DIST += \
	tests/epub/metadata/* \
	tests/ncx/* \
	tests/ocf/* \
	tests/opf/dc-metadata/* \
	tests/opf/metadata/* \
	tests/opf/vorbis-comments/* \
	tests/rdfxml/schema/* \
	tests/rdfxml/syntax/* \
	tests/smil/metadata/* \
	tests/html/* \
	tests/xmlparser/* \
	tests/*.py

TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

TESTS = \
	tests/rdfmodel \
	tests/ntriple_formatter \
	tests/turtle_formatter \
	tests/metadata.py \
	tests/vorbis-comments.py \
	tests/events.py \
	tests/xmlreader.py

