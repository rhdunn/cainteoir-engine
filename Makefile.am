AUTOMAKE_OPTIONS = subdir-objects

localedir  = $(datadir)/locale
xdgdatadir = @XDGDATADIR@
mimedir    = $(xdgdatadir)/mime
xmldir     = $(mimedir)/packages
vimdir     = $(prefix)/share/vim

AM_LDFLAGS  = ${LTLIBINTL}
AM_CXXFLAGS = \
	-Isrc/include -Isrc -Iucd-tools/src/include \
	-DMBROLA_DIR=\"$(MBROLA_DIR)\" \
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
SUFFIXES = .html .phon .md
SUBDIRS = po

install-data-hook: install-data-hook-mimetypes install-data-hook-locale

uninstall-hook: uninstall-hook-mimetypes

uninstall-local: uninstall-local-locale

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

############################# phonemes ########################################

.phon.html:
	data/phonemes/mkchart.py $< > $@

phonemes:	data/phonemes/ascii-ipa.html \
		data/phonemes/ipa.html \
		data/phonemes/x-sampa.html \
		data/phonemes/arpabet.html \
		data/phonemes/sampa/de.html \
		data/phonemes/sampa/en.html

############################# data ############################################

EXTRA_DIST += \
	data/languages.rdf \
	data/locale/* \
	src/schema/*.rdf

data/languages.dat:
	wget -O $@ http://www.iana.org/assignments/language-subtag-registry

data/languages.stat: data/languages.dat data/private-use.dat data/languages/*.dat
	touch $@

languages: data/languages.py data/languages.stat
	cd data && ./languages.py

data/languages.rdf.gz: data/languages.rdf
	gzip -n -c $< > $@

pkgdata_DATA += \
	data/languages.rdf.gz \
	src/schema/text-to-speech.rdf

cssdir = $(pkgdatadir)/css
css_DATA = data/css/*.css
EXTRA_DIST += $(css_DATA)

install-data-hook-locale:
	cd data && (find locale -type f | grep -vF "/." | \
		tar -cO -T - --no-recursion | \
		tar -C $(DESTDIR)$(pkgdatadir) -xvf - > $(DESTDIR)$(pkgdatadir)/locale.installed)

uninstall-local-locale:
	cd $(DESTDIR)$(pkgdatadir); \
	cat locale.installed | sed -e 's/ /\\ /g' | xargs rm -f; \
	rm -f locale.installed

vim_registrydir = $(vimdir)/registry
vim_registry_DATA = data/vim/registry/*.yaml
EXTRA_DIST += $(vim_registry_DATA)

vim_addons_ftdetectdir = $(vimdir)/addons/ftdetect
vim_addons_ftdetect_DATA = data/vim/ftdetect/*.vim
EXTRA_DIST += $(vim_addons_ftdetect_DATA)

vim_addons_syntaxdir = $(vimdir)/addons/syntax
vim_addons_syntax_DATA = data/vim/syntax/*.vim
EXTRA_DIST += $(vim_addons_syntax_DATA)

############################# mimetypes #######################################

xml_DATA = data/mime/packages/cainteoir-data.xml
EXTRA_DIST += $(xml_DATA)

mimetypes:
	$(UPDATE_MIME_DATABASE) data/mime/

install-data-hook-mimetypes:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
 
uninstall-hook-mimetypes:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
 
############################# documentation ###################################

.md.html:
	pandoc --from markdown --to html $< --output $@

data/languages.dot: data/languages.py data/languages.stat
	cd data && ./languages.py

.dot.svg: $<
	dot -Tsvg -o $@ $<

fsm:	data/parsetext-fsm.svg

api:
	doxygen docs/cainteoir-engine.doxyconf 2>&1 | ( ! grep . )

doc:	README.html \
	docs/ReleaseNotes.html \
	api

############################# libcainteoir ####################################

libcainteoir_includedir = $(includedir)/cainteoir
libcainteoir_include_HEADERS = \
	src/include/cainteoir/archive.hpp \
	src/include/cainteoir/audio.hpp \
	src/include/cainteoir/buffer.hpp \
	src/include/cainteoir/content.hpp \
	src/include/cainteoir/document.hpp \
	src/include/cainteoir/encoding.hpp \
	src/include/cainteoir/engines.hpp \
	src/include/cainteoir/languages.hpp \
	src/include/cainteoir/metadata.hpp \
	src/include/cainteoir/mimetype.hpp \
	src/include/cainteoir/path.hpp \
	src/include/cainteoir/range.hpp \
	src/include/cainteoir/stopwatch.hpp \
	src/include/cainteoir/text.hpp \
	src/include/cainteoir/unicode.hpp \
	src/include/cainteoir/xmlreader.hpp

lib_LTLIBRARIES += src/libcainteoir/libcainteoir.la

src_libcainteoir_libcainteoir_la_LDFLAGS = -version-info $(LIBCAINTEOIR_VERSION) \
	${ALSA_LIBS} \
	${PULSE_LIBS} \
	${VORBIS_LIBS} \
	${ZLIB_LIBS} \
	${POPPLER_LIBS} \
	${ESPEAK_LIBS} \
	${LTLIBINTL}

src_libcainteoir_libcainteoir_la_CXXFLAGS = ${AM_CXXFLAGS} \
	${ALSA_CFLAGS} \
	${PULSE_CFLAGS} \
	${VORBIS_CFLAGS} \
	${ZLIB_CFLAGS} \
	${POPPLER_CFLAGS} \
	${ESPEAK_CFLAGS}

src_libcainteoir_libcainteoir_la_SOURCES = \
	ucd-tools/src/include/ucd/ucd.h \
	ucd-tools/src/case.cpp \
	ucd-tools/src/categories.cpp \
	ucd-tools/src/ctype.cpp \
	ucd-tools/src/scripts.cpp \
	ucd-tools/src/tostring.cpp \
	\
	src/i18n.h \
	src/compatibility.hpp \
	\
	src/libcainteoir/document.cpp \
	src/libcainteoir/encoding.cpp \
	src/libcainteoir/languages.cpp \
	src/libcainteoir/mimetype_database.hpp \
	src/libcainteoir/mimetype.cpp \
	src/libcainteoir/path.cpp \
	\
	src/libcainteoir/audio/alsa.cpp \
	src/libcainteoir/audio/audio.cpp \
	src/libcainteoir/audio/ogg.cpp \
	src/libcainteoir/audio/pulse.cpp \
	src/libcainteoir/audio/wav.cpp \
	\
	src/libcainteoir/buffers/buffer.cpp \
	src/libcainteoir/buffers/data_buffer.cpp \
	src/libcainteoir/buffers/mmap_buffer.cpp \
	src/libcainteoir/buffers/normalized_text_buffer.cpp \
	src/libcainteoir/buffers/rope.cpp \
	\
	src/libcainteoir/compression/base64.cpp \
	src/libcainteoir/compression/gzip.cpp \
	src/libcainteoir/compression/none.cpp \
	src/libcainteoir/compression/quoted_printable.cpp \
	\
	src/libcainteoir/css/counterstyles.cpp \
	src/libcainteoir/css/reader.cpp \
	src/libcainteoir/css/style_manager.cpp \
	src/libcainteoir/css/units.cpp \
	\
	src/libcainteoir/parsers/html-entities.h \
	src/libcainteoir/parsers/xml-entities.h \
	\
	src/libcainteoir/parsers/parsers.hpp \
	src/libcainteoir/parsers/document.cpp \
	src/libcainteoir/parsers/epub.cpp \
	src/libcainteoir/parsers/mime.cpp \
	src/libcainteoir/parsers/ncx.cpp \
	src/libcainteoir/parsers/ocf.cpp \
	src/libcainteoir/parsers/opf.cpp \
	src/libcainteoir/parsers/pdf.cpp \
	src/libcainteoir/parsers/plaintext.cpp \
	src/libcainteoir/parsers/rdfxml.cpp \
	src/libcainteoir/parsers/rtf.cpp \
	src/libcainteoir/parsers/smil.cpp \
	src/libcainteoir/parsers/ssml.cpp \
	src/libcainteoir/parsers/xhtml.cpp \
	src/libcainteoir/parsers/xmlreader.cpp \
	src/libcainteoir/parsers/zip.cpp \
	src/libcainteoir/parsers/zipdoc.cpp \
	\
	src/libcainteoir/rdf/formatter.cpp \
	src/libcainteoir/rdf/metadata.cpp \
	\
	src/libcainteoir/tts/context_analysis.cpp \
	src/libcainteoir/tts/dictionary.cpp \
	src/libcainteoir/tts/engines.cpp \
	src/libcainteoir/tts/espeak.cpp \
	src/libcainteoir/tts/explicit_feature_phonemeset.cpp \
	src/libcainteoir/tts/letter_to_phoneme.cpp \
	src/libcainteoir/tts/phoneme.cpp \
	src/libcainteoir/tts/phoneme_stream.cpp \
	src/libcainteoir/tts/text_reader.cpp \
	src/libcainteoir/tts/tts_engine.hpp \
	src/libcainteoir/tts/word_stream.cpp \
	\
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

bin_PROGRAMS += src/apps/doc2doc/doc2doc

src_apps_doc2doc_doc2doc_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_doc2doc_doc2doc_SOURCES = src/apps/doc2doc/main.cpp

EXTRA_DIST += ${man1_MANS}

############################# tests ###########################################

TEST_FILES = \
	tests/css/* \
	tests/dict/* \
	tests/epub/* \
	tests/ncx/* \
	tests/ocf/* \
	tests/opf/* \
	tests/parse-text/* \
	tests/rdfxml/* \
	tests/rtf/* \
	tests/ssml/* \
	tests/smil/* \
	tests/html/* \
	tests/http/* \
	tests/mime/* \
	tests/xmlparser/* \
	tests/zip/*

EXTRA_DIST += $(TEST_FILES) tests/*.py

noinst_bin_PROGRAMS += tests/buffer

tests_buffer_LDADD   = src/libcainteoir/libcainteoir.la
tests_buffer_SOURCES = tests/buffer.cpp tests/tester.hpp

tests/buffer.check: tests/buffer
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/rdfmodel

tests_rdfmodel_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfmodel_SOURCES = tests/rdfmodel.cpp tests/tester.hpp

tests/rdfmodel.check: tests/rdfmodel
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/rdfquery

tests_rdfquery_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfquery_SOURCES = tests/rdfquery.cpp tests/tester.hpp

tests/rdfquery.check: tests/rdfquery
	rm -f $@
	(XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ $< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/ntriple_formatter

tests_ntriple_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_ntriple_formatter_SOURCES = tests/ntriple_formatter.cpp tests/tester.hpp

tests/ntriple_formatter.check: tests/ntriple_formatter
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/turtle_formatter

tests_turtle_formatter_LDADD   = src/libcainteoir/libcainteoir.la
tests_turtle_formatter_SOURCES = tests/turtle_formatter.cpp tests/tester.hpp

tests/turtle_formatter.check: tests/turtle_formatter
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/events

tests_events_LDADD   = src/libcainteoir/libcainteoir.la
tests_events_SOURCES = tests/events.cpp

tests/events.check: tests/events tests/events.py tests/harness.py data/css ${TEST_FILES}
	rm -f $@
	(python tests/events.py && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/encoding

tests_encoding_LDADD   = src/libcainteoir/libcainteoir.la
tests_encoding_SOURCES = tests/encoding.cpp

tests/encoding.check: tests/encoding
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/xmlreader

tests_xmlreader_LDADD   = src/libcainteoir/libcainteoir.la
tests_xmlreader_SOURCES = tests/xmlreader.cpp

tests/xmlreader.check: tests/xmlreader tests/xmlreader.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/xmlreader.py && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/ttslanguages

tests_ttslanguages_LDADD   = src/libcainteoir/libcainteoir.la
tests_ttslanguages_SOURCES = tests/ttslanguages.cpp

tests/ttslanguages.check: tests/ttslanguages
	rm -f $@
	(XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=`pwd`/data $< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/languages

tests_languages_LDADD   = src/libcainteoir/libcainteoir.la
tests_languages_SOURCES = tests/languages.cpp

tests/languages.check: tests/languages
	rm -f $@
	(XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=`pwd`/data $< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/utf8

tests_utf8_LDADD   = src/libcainteoir/libcainteoir.la
tests_utf8_SOURCES = tests/utf8.cpp

tests/utf8.check: tests/utf8
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/parsetext

tests_parsetext_LDADD   = src/libcainteoir/libcainteoir.la
tests_parsetext_SOURCES = tests/parsetext.cpp

tests/parsetext.check: tests/parsetext tests/parsetext.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/parsetext.py && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/mimetype

tests_mimetype_LDADD   = src/libcainteoir/libcainteoir.la
tests_mimetype_SOURCES = tests/mimetype.cpp

noinst_bin_PROGRAMS += tests/content

tests_content_LDADD   = src/libcainteoir/libcainteoir.la
tests_content_SOURCES = tests/content.cpp

tests/content.check: tests/content
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/phoneme

tests_phoneme_LDADD   = src/libcainteoir/libcainteoir.la
tests_phoneme_SOURCES = tests/phoneme.cpp

tests/phoneme.check: tests/phoneme
	rm -f $@
	($< && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/dictionary

tests_dictionary_LDADD   = src/libcainteoir/libcainteoir.la
tests_dictionary_SOURCES = tests/dictionary.cpp

tests/dictionary.check: tests/dictionary tests/dictionary.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/dictionary.py && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/xmlcssparser

tests_xmlcssparser_LDADD   = src/libcainteoir/libcainteoir.la
tests_xmlcssparser_SOURCES = tests/xmlcssparser.cpp

noinst_bin_PROGRAMS += tests/styles

tests_styles_LDADD   = src/libcainteoir/libcainteoir.la
tests_styles_SOURCES = tests/styles.cpp

tests/styles.check: tests/styles tests/styles.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/styles.py && touch $@) | tee $<.results
	test -f $@ || false

tests/metadata.check: src/apps/metadata/metadata tests/metadata.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/metadata.py && touch $@) | tee $<.results
	test -f $@ || false

tests/vorbis-comments.check: src/apps/metadata/metadata tests/vorbis-comments.py tests/harness.py ${TEST_FILES}
	rm -f $@
	(python tests/vorbis-comments.py && touch $@) | tee $<.results
	test -f $@ || false

noinst_bin_PROGRAMS += tests/synthesizer

tests_synthesizer_LDADD   = src/libcainteoir/libcainteoir.la
tests_synthesizer_SOURCES = tests/synthesizer.cpp

check: \
	tests/buffer.check \
	tests/utf8.check \
	tests/encoding.check \
	tests/rdfmodel.check \
	tests/ntriple_formatter.check \
	tests/turtle_formatter.check \
	tests/rdfquery.check \
	tests/languages.check \
	tests/ttslanguages.check \
	tests/xmlreader.check \
	tests/metadata.check \
	tests/vorbis-comments.check \
	tests/content.check \
	tests/styles.check \
	tests/events.check \
	tests/phoneme.check \
	tests/dictionary.check \
	tests/parsetext.check
