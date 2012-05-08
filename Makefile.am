AUTOMAKE_OPTIONS = subdir-objects

localedir  = $(datadir)/locale
xdgdatadir = @XDGDATADIR@
mimedir    = $(xdgdatadir)/mime
xmldir     = $(mimedir)/packages

AM_LDFLAGS  = ${LTLIBINTL}
AM_CXXFLAGS = \
	-Isrc/include -Isrc \
	-DDATADIR='"$(datadir)"' -DLOCALEDIR=\"$(localedir)\"

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS =
lib_LTLIBRARIES =
man1_MANS =
pkgdata_DATA =

noinst_bin_PROGRAMS =
noinst_LIBRARIES =

noinst_bindir =

EXTRA_DIST = config.rpath ChangeLog
CLEANFILES =
SUBDIRS = po

############################# libtool #########################################

EXTRA_DIST += config.guess config.sub ltmain.sh

# Increment if the interface has changed and is not backward compatible
CURRENT=0

# Increment  if source files have changed
# Reset to 0 if the interface has changed
REVISION=0

# Increment  if the interface is backward compatible (superset)
# Reset to 0 if the interface is not backward compatible
AGE=0

LIBCAINTEOIR_VERSION=$(CURRENT):$(REVISION):$(AGE)

############################# gettext #########################################

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

############################# data ############################################

EXTRA_DIST += \
	data/languages.rdf \
	src/schema/*.rdf

data/languages.rdf.gz: data/languages.rdf
	gzip -n -c $< > $@

pkgdata_DATA += \
	data/languages.rdf.gz \
	src/schema/text-to-speech.rdf

############################# mimetypes #######################################

EXTRA_DIST += data/mime/packages/cainteoir-data.xml

xml_DATA = data/mime/packages/cainteoir-data.xml

mimetypes:
	$(UPDATE_MIME_DATABASE) data/mime/

install-data-hook:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
 
uninstall-hook:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
 
############################# libcainteoir ####################################

libcainteoir_includedir = $(includedir)/cainteoir
libcainteoir_include_HEADERS = \
	src/include/cainteoir/languages.hpp \
	src/include/cainteoir/archive.hpp \
	src/include/cainteoir/audio.hpp \
	src/include/cainteoir/buffer.hpp \
	src/include/cainteoir/encoding.hpp \
	src/include/cainteoir/unicode.hpp \
	src/include/cainteoir/xmlreader.hpp \
	src/include/cainteoir/document.hpp \
	src/include/cainteoir/engines.hpp \
	src/include/cainteoir/mimetype.hpp \
	src/include/cainteoir/metadata.hpp

lib_LTLIBRARIES += src/libcainteoir/libcainteoir.la

src_libcainteoir_libcainteoir_la_LDFLAGS = -version-info $(LIBCAINTEOIR_VERSION) \
	${PULSE_LIBS} \
	${VORBIS_LIBS} \
	${ZLIB_LIBS} \
	${ESPEAK_LIBS} \
	${LTLIBINTL}

src_libcainteoir_libcainteoir_la_CXXFLAGS = ${AM_CXXFLAGS} \
	${PULSE_CFLAGS} \
	${VORBIS_CFLAGS} \
	${ZLIB_CFLAGS} \
	${ESPEAK_CFLAGS}

src_libcainteoir_libcainteoir_la_SOURCES = \
	src/i18n.h \
	src/compatibility.h \
	src/libcainteoir/languages.cpp \
	src/libcainteoir/mimetype.cpp \
	src/libcainteoir/encoding.cpp \
	src/libcainteoir/buffers/buffer.cpp \
	src/libcainteoir/buffers/data_buffer.cpp \
	src/libcainteoir/buffers/mmap_buffer.cpp \
	src/libcainteoir/buffers/normalized_text_buffer.cpp \
	src/libcainteoir/buffers/rope.cpp \
	src/libcainteoir/compression/none.cpp \
	src/libcainteoir/compression/gzip.cpp \
	src/libcainteoir/compression/base64.cpp \
	src/libcainteoir/compression/quoted_printable.cpp \
	src/libcainteoir/rdf/metadata.cpp \
	src/libcainteoir/rdf/formatter.cpp \
	src/libcainteoir/parsers/parsers.hpp \
	src/libcainteoir/parsers/zip.cpp \
	src/libcainteoir/parsers/xmlreader.cpp \
	src/libcainteoir/parsers/plaintext.cpp \
	src/libcainteoir/parsers/html-entities.h \
	src/libcainteoir/parsers/xhtml.cpp \
	src/libcainteoir/parsers/ncx.cpp \
	src/libcainteoir/parsers/mime.cpp \
	src/libcainteoir/parsers/ocf.cpp \
	src/libcainteoir/parsers/opf.cpp \
	src/libcainteoir/parsers/epub.cpp \
	src/libcainteoir/parsers/rdfxml.cpp \
	src/libcainteoir/parsers/ssml.cpp \
	src/libcainteoir/parsers/smil.cpp \
	src/libcainteoir/parsers/rtf.cpp \
	src/libcainteoir/parsers/document.cpp \
	src/libcainteoir/audio/audio.cpp \
	src/libcainteoir/audio/pulse.cpp \
	src/libcainteoir/audio/wav.cpp \
	src/libcainteoir/audio/ogg.cpp \
	src/libcainteoir/tts/tts_engine.hpp \
	src/libcainteoir/tts/engines.cpp \
	src/libcainteoir/tts/espeak.cpp \
	src/libcainteoir/unicode/utf8.cpp

############################# applications ####################################

bin_PROGRAMS += src/apps/cainteoir/cainteoir
man1_MANS += src/apps/cainteoir/cainteoir.man

src_apps_cainteoir_cainteoir_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_cainteoir_cainteoir_SOURCES = src/apps/cainteoir/main.cpp

bin_PROGRAMS += src/apps/metadata/metadata
man1_MANS += src/apps/metadata/metadata.man

src_apps_metadata_metadata_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_metadata_metadata_SOURCES = src/apps/metadata/main.cpp

bin_PROGRAMS += src/apps/tagcloud/tagcloud

src_apps_tagcloud_tagcloud_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_tagcloud_tagcloud_SOURCES = src/apps/tagcloud/main.cpp

EXTRA_DIST += ${man1_MANS}

############################# tests ###########################################

noinst_bin_PROGRAMS += tests/rdfmodel

tests_rdfmodel_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfmodel_SOURCES = tests/rdfmodel.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/rdfquery

tests_rdfquery_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfquery_SOURCES = tests/rdfquery.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/ntriple_formatter

tests_ntriple_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_ntriple_formatter_SOURCES = tests/ntriple_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/turtle_formatter

tests_turtle_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_turtle_formatter_SOURCES = tests/turtle_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/events

tests_events_LDADD   = src/libcainteoir/libcainteoir.la
tests_events_SOURCES = tests/events.cpp

noinst_bin_PROGRAMS += tests/encoding

tests_encoding_LDADD   = src/libcainteoir/libcainteoir.la
tests_encoding_SOURCES = tests/encoding.cpp

noinst_bin_PROGRAMS += tests/xmlreader

tests_xmlreader_LDADD   = src/libcainteoir/libcainteoir.la
tests_xmlreader_SOURCES = tests/xmlreader.cpp

noinst_bin_PROGRAMS += tests/languages

tests_languages_LDADD   = src/libcainteoir/libcainteoir.la
tests_languages_SOURCES = tests/languages.cpp

noinst_bin_PROGRAMS += tests/utf8

tests_utf8_LDADD   = src/libcainteoir/libcainteoir.la
tests_utf8_SOURCES = tests/utf8.cpp

TEST_FILES = \
	tests/epub/* \
	tests/ncx/* \
	tests/ocf/* \
	tests/opf/* \
	tests/rdfxml/* \
	tests/rtf/* \
	tests/ssml/* \
	tests/smil/* \
	tests/html/* \
	tests/http/* \
	tests/mime/* \
	tests/xmlparser/* \
	tests/*.py

EXTRA_DIST += $(TEST_FILES)

check: mimetypes tests/rdfmodel tests/rdfquery tests/ntriple_formatter tests/turtle_formatter src/apps/metadata/metadata tests/events tests/xmlreader $(TEST_FILES)
	tests/rdfmodel
	tests/ntriple_formatter
	tests/turtle_formatter
	XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ tests/rdfquery
	XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=`pwd`/data tests/languages
	tests/utf8
	tests/encoding
	python tests/xmlreader.py
	python tests/metadata.py
	python tests/vorbis-comments.py
	python tests/events.py
