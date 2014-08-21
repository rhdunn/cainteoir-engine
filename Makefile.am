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
	-DPICO_LANG_DIR=\"$(PICO_LANG_DIR)\" \
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

install-data-hook: \
	install-data-hook-mimetypes \
	install-data-hook-locale \
	install-data-hook-accents \
	install-data-hook-phonemeset

uninstall-hook: \
	uninstall-hook-mimetypes

uninstall-local: \
	uninstall-local-locale \
	uninstall-local-accents \
	uninstall-local-phonemeset

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
	data/locale/* \
	data/phonemeset/* \
	src/schema/*.rdf

data/languages.dat:
	wget -O $@ http://www.iana.org/assignments/language-subtag-registry

data/languages.stat: data/languages.dat data/private-use.dat data/languages/*.dat
	touch $@

languages: data/languages.py data/languages.stat
	cd data && ${PYTHON} languages.py

data/languages.rdf.gz: data/languages.rdf
	gzip -n -c $< > $@

pkgdata_DATA += \
	data/languages.rdf.gz \
	src/schema/text-to-speech.rdf

cssdir = $(pkgdatadir)/css
css_DATA = data/css/*.css
EXTRA_DIST += $(css_DATA)

install-data-hook-locale:
	cd data && (find -L locale -type f | grep -vF "/." | \
		tar -cO -T - --no-recursion | \
		tar -C $(DESTDIR)$(pkgdatadir) -xvf - > $(DESTDIR)$(pkgdatadir)/locale.installed)

uninstall-local-locale:
	cd $(DESTDIR)$(pkgdatadir); \
	cat locale.installed | sed -e 's/ /\\ /g' | xargs rm -f; \
	rm -f locale.installed

install-data-hook-phonemeset:
	cd data && (find -L phonemeset -type f | grep -vF "/." | \
		tar -cO -T - --no-recursion | \
		tar -C $(DESTDIR)$(pkgdatadir) -xvf - > $(DESTDIR)$(pkgdatadir)/phonemeset.installed)

uninstall-local-phonemeset:
	cd $(DESTDIR)$(pkgdatadir); \
	cat phonemeset.installed | sed -e 's/ /\\ /g' | xargs rm -f; \
	rm -f phonemeset.installed

install-data-hook-accents:
	cd data && (find -L accents -type f | grep -vF "/." | \
		tar -cO -T - --no-recursion | \
		tar -C $(DESTDIR)$(pkgdatadir) -xvf - > $(DESTDIR)$(pkgdatadir)/accents.installed)

uninstall-local-accents:
	cd $(DESTDIR)$(pkgdatadir); \
	cat accents.installed | sed -e 's/ /\\ /g' | xargs rm -f; \
	rm -f acccents.installed

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

data/mime/mime.cache: data/mime/packages/cainteoir-data.xml
	$(UPDATE_MIME_DATABASE) data/mime/

install-data-hook-mimetypes:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif
 
uninstall-hook-mimetypes:
if ENABLE_UPDATE_MIME_DATABASE
	$(UPDATE_MIME_DATABASE) "$(DESTDIR)$(mimedir)"
endif

############################# tools / extensions ##############################

%.html: %.phon src/apps/phoneme-converter
	CAINTEOIR_DATA_DIR=`pwd`/data \
	    src/apps/phoneme-converter --chart \
	        `echo $< | sed -e 's,data/phonemeset/,,' -e 's,\.phon,,'` \
	        > /tmp/phonemechart && \
	    mv /tmp/phonemechart $@

%.fsm.h: %.fsm tools/fsm
	${PYTHON} tools/fsm header $< > $@

%.dot: %.fsm tools/fsm
	${PYTHON} tools/fsm dot $< > $@

.dot.svg: $<
	dot -Tsvg $< | sed -n '4,9999p' > $@

.md.html:
	kramdown $< > $@

############################# libcainteoir ####################################

src/libcainteoir/tts/text_reader.cpp: src/libcainteoir/tts/text_reader.fsm.h

libcainteoir_includedir = $(includedir)/cainteoir
libcainteoir_include_HEADERS = \
	src/include/cainteoir/archive.hpp \
	src/include/cainteoir/audio.hpp \
	src/include/cainteoir/buffer.hpp \
	src/include/cainteoir/content.hpp \
	src/include/cainteoir/dictionary.hpp \
	src/include/cainteoir/document.hpp \
	src/include/cainteoir/encoding.hpp \
	src/include/cainteoir/engines.hpp \
	src/include/cainteoir/languages.hpp \
	src/include/cainteoir/metadata.hpp \
	src/include/cainteoir/mimetype.hpp \
	src/include/cainteoir/path.hpp \
	src/include/cainteoir/phoneme.hpp \
	src/include/cainteoir/range.hpp \
	src/include/cainteoir/stopwatch.hpp \
	src/include/cainteoir/synthesizer.hpp \
	src/include/cainteoir/text.hpp \
	src/include/cainteoir/trie.hpp \
	src/include/cainteoir/unicode.hpp \
	src/include/cainteoir/xmlreader.hpp

lib_LTLIBRARIES += src/libcainteoir/libcainteoir.la

src_libcainteoir_libcainteoir_la_LDFLAGS = -version-info $(LIBCAINTEOIR_VERSION) \
	${FFMPEG_LIBS} \
	${AVRESAMPLE_LIBS} \
	${ALSA_LIBS} \
	${PULSE_LIBS} \
	${VORBIS_LIBS} \
	${ZLIB_LIBS} \
	${POPPLER_LIBS} \
	${ESPEAK_LIBS} \
	${PICO_LIBS} \
	${LTLIBINTL}

src_libcainteoir_libcainteoir_la_CXXFLAGS = ${AM_CXXFLAGS} \
	${FFMPEG_CFLAGS} \
	${AVRESAMPLE_CFLAGS} \
	${ALSA_CFLAGS} \
	${PULSE_CFLAGS} \
	${VORBIS_CFLAGS} \
	${ZLIB_CFLAGS} \
	${POPPLER_CFLAGS} \
	${ESPEAK_CFLAGS} \
	${PICO_CFLAGS}

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
	src/libcainteoir/cainteoir_file_reader.hpp \
	src/libcainteoir/cainteoir_file_reader.cpp \
	\
	src/libcainteoir/audio/alsa.cpp \
	src/libcainteoir/audio/audio.cpp \
	src/libcainteoir/audio/ffmpeg_player.cpp \
	src/libcainteoir/audio/ogg.cpp \
	src/libcainteoir/audio/pulse.cpp \
	src/libcainteoir/audio/wav.cpp \
	\
	src/libcainteoir/buffers/buffer.cpp \
	src/libcainteoir/buffers/data_buffer.cpp \
	src/libcainteoir/buffers/memory_file.cpp \
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
	src/libcainteoir/engines/tts_engine.hpp \
	src/libcainteoir/engines/engines.cpp \
	src/libcainteoir/engines/espeak.cpp \
	src/libcainteoir/engines/pico.cpp \
	\
	src/libcainteoir/phoneme/arpabet_phonemeset.cpp \
	src/libcainteoir/phoneme/espeak_phonemeset.cpp \
	src/libcainteoir/phoneme/explicit_feature_phonemeset.cpp \
	src/libcainteoir/phoneme/ipa_phonemeset.cpp \
	src/libcainteoir/phoneme/kirshenbaum_phonemeset.cpp \
	src/libcainteoir/phoneme/phoneme.cpp \
	src/libcainteoir/phoneme/phonemeset.cpp \
	src/libcainteoir/phoneme/phoneme_file_reader.cpp \
	src/libcainteoir/phoneme/phoneme_stress.cpp \
	src/libcainteoir/phoneme/phoneme_to_phoneme.cpp \
	\
	src/libcainteoir/dictionary/cainteoir.cpp \
	src/libcainteoir/dictionary/dictionary.cpp \
	src/libcainteoir/dictionary/espeak.cpp \
	src/libcainteoir/tts/letter_to_phoneme.cpp \
	\
	src/libcainteoir/tts/adjust_stress.cpp \
	src/libcainteoir/tts/context_analysis.cpp \
	src/libcainteoir/tts/next_clause.cpp \
	src/libcainteoir/tts/phoneme_stream.cpp \
	src/libcainteoir/tts/text_reader.cpp \
	src/libcainteoir/tts/word_stream.cpp \
	\
	src/libcainteoir/synthesizer/fixed_duration.cpp \
	src/libcainteoir/synthesizer/duration.cpp \
	\
	src/libcainteoir/synthesizer/pho_file.cpp \
	src/libcainteoir/synthesizer/diphone.cpp \
	src/libcainteoir/synthesizer/prosody.cpp \
	\
	src/libcainteoir/synthesizer/synth.hpp \
	src/libcainteoir/synthesizer/synthesizer.cpp \
	src/libcainteoir/synthesizer/mbrola.cpp \
	\
	src/libcainteoir/unicode/utf8.cpp

############################# documentation ###################################

DOC_SOURCE_FILES = \
	$(libcainteoir_include_HEADERS:src/include/cainteoir/%.hpp=docs/api/src/%.md) \
	docs/api/src/namespaces.md

docs/api/xml/index.xml: docs/api/cainteoir-engine.doxyconf ${libcainteoir_include_HEADERS}
	rm -rf docs/api/xml
	doxygen docs/api/cainteoir-engine.doxyconf

docs/api/html.stamp: docs/api/xml/index.xml ${DOC_SOURCE_FILES}
	rm -rf docs/api/html
	${PYTHON} ${DOCGEN}/doxygen.py docs/api/xml/*.xml ${DOC_SOURCE_FILES}
	touch $@

data/languages.dot: data/languages.py data/languages.stat
	cd data && ${PYTHON} languages.py

fsm:	src/libcainteoir/tts/text_reader.svg

api:	docs/api/html.stamp

html:	README.html \
	docs/voicedb-format.html \
	docs/ReleaseNotes.html

doc:	html api

phonemes:	data/phonemeset/ascii-ipa.html \
		data/phonemeset/ipa.html \
		data/phonemeset/x-sampa.html \
		data/phonemeset/cxs.html

############################# applications ####################################

bin_PROGRAMS += src/apps/cainteoir
man1_MANS += src/apps/cainteoir.man

src_apps_cainteoir_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_cainteoir_SOURCES = src/apps/cainteoir.cpp

bin_PROGRAMS += src/apps/metadata
man1_MANS += src/apps/metadata.man

src_apps_metadata_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_metadata_SOURCES = src/apps/metadata.cpp

bin_PROGRAMS += src/apps/phoneme-converter
man1_MANS += src/apps/phoneme-converter.man

src_apps_phoneme_converter_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_phoneme_converter_SOURCES = src/apps/phoneme-converter.cpp

bin_PROGRAMS += src/apps/dictionary
man1_MANS += src/apps/dictionary.man

src_apps_dictionary_LDADD   = src/libcainteoir/libcainteoir.la
src_apps_dictionary_SOURCES = src/apps/dictionary.cpp

EXTRA_DIST += ${man1_MANS}

############################# examples ########################################

noinst_bin_PROGRAMS += src/examples/doc2doc

src_examples_doc2doc_LDADD   = src/libcainteoir/libcainteoir.la
src_examples_doc2doc_SOURCES = src/examples/doc2doc.cpp

noinst_bin_PROGRAMS += src/examples/tagcloud

src_examples_tagcloud_LDADD   = src/libcainteoir/libcainteoir.la
src_examples_tagcloud_SOURCES = src/examples/tagcloud.cpp

noinst_bin_PROGRAMS += src/examples/musicplayer

src_examples_musicplayer_LDADD   = src/libcainteoir/libcainteoir.la
src_examples_musicplayer_SOURCES = src/examples/musicplayer.cpp

noinst_bin_PROGRAMS += src/examples/sigproc

src_examples_sigproc_LDADD   = src/libcainteoir/libcainteoir.la
src_examples_sigproc_SOURCES = src/examples/sigproc.cpp

noinst_bin_PROGRAMS += src/examples/voice-synthesizer

src_examples_voice_synthesizer_LDADD   = src/libcainteoir/libcainteoir.la
src_examples_voice_synthesizer_SOURCES = src/examples/voice-synthesizer.cpp

############################# experimental ####################################

noinst_bin_PROGRAMS += src/experimental/synthesizer

src_experimental_synthesizer_LDADD   = src/libcainteoir/libcainteoir.la
src_experimental_synthesizer_SOURCES = src/experimental/synthesizer.cpp

############################# tests ###########################################

CSS_TEST_FILES = $(shell find tests/css/*)

DICTIONARY_TEST_FILES = $(shell find tests/dict/*)

PARSETEXT_TEST_FILES = $(shell find tests/parse-text/*)

XMLPARSER_TEST_FILES = $(shell find tests/xmlparser/*)

HTMLTREE_TEST_FILES = $(shell find tests/html/tree-construction/*)

PHONEMESET_TEST_FILES = $(shell find tests/phonemeset/*)

ACCENT_TEST_FILES = $(shell find tests/accent/*)

DIPHONE_TEST_FILES = $(shell find tests/diphone/*)

PROSODY_TEST_FILES = $(shell find tests/prosody/*)

DOCUMENT_TEST_FILES = \
	$(shell find tests/epub2/*) \
	$(shell find tests/epub3/*) \
	$(shell find tests/ncx/*) \
	$(shell find tests/ocf/*) \
	$(shell find tests/opf/*) \
	$(shell find tests/rdfxml/*) \
	$(shell find tests/rtf/*) \
	$(shell find tests/ssml/*) \
	$(shell find tests/smil/*) \
	$(shell find tests/html/*) \
	$(shell find tests/http/*) \
	$(shell find tests/mime/*) \
	$(shell find tests/zip/*)

%.check : %.py tests/harness.py data/mime/mime.cache
	rm -f $@
	(${PYTHON} $< && touch $@) | tee `echo $< | sed -e 's/.py/.results/'`
	test -f $@ || false

%.check: %.test data/mime/mime.cache
	rm -f $@
	(XDG_DATA_DIRS=`pwd`/data/:/usr/local/share/:/usr/share/ CAINTEOIR_DATA_DIR=`pwd`/data $< && touch $@) | tee `echo $< | sed -e 's/.test/.results/'`
	test -f $@ || false

EXTRA_DIST += \
	$(CSS_TEST_FILES) \
	$(DICTIONARY_TEST_FILES) \
	$(PARSETEXT_TEST_FILES) \
	$(XMLPARSER_TEST_FILES) \
	$(DOCUMENT_TEST_FILES) \
	$(PHONEMESET_TEST_FILES) \
	$(ACCENT_TEST_FILES) \
	$(DIPHONE_TEST_FILES) \
	$(PROSODY_TEST_FILES) \
	tests/*.py

noinst_bin_PROGRAMS += tests/range.test

tests_range_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_range_test_SOURCES = tests/range.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/buffer.test

tests_buffer_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_buffer_test_SOURCES = tests/buffer.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/path.test

tests_path_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_path_test_SOURCES = tests/path.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/multiword_entry.test

tests_multiword_entry_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_multiword_entry_test_SOURCES = tests/multiword_entry.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/rdfmodel.test

tests_rdfmodel_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfmodel_test_SOURCES = tests/rdfmodel.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/rdfquery.test

tests_rdfquery_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_rdfquery_test_SOURCES = tests/rdfquery.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/ntriple_formatter.test

tests_ntriple_formatter_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_ntriple_formatter_test_SOURCES = tests/ntriple_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/turtle_formatter.test

tests_turtle_formatter_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_turtle_formatter_test_SOURCES = tests/turtle_formatter.cpp tests/tester.hpp

noinst_bin_PROGRAMS += tests/events

tests_events_LDADD   = src/libcainteoir/libcainteoir.la
tests_events_SOURCES = tests/events.cpp

tests/events.check: tests/events data/css ${DOCUMENT_TEST_FILES}

noinst_bin_PROGRAMS += tests/encoding.test

tests_encoding_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_encoding_test_SOURCES = tests/encoding.cpp

noinst_bin_PROGRAMS += tests/xmlreader

tests_xmlreader_LDADD   = src/libcainteoir/libcainteoir.la
tests_xmlreader_SOURCES = tests/xmlreader.cpp

tests/xmlreader.check: tests/xmlreader ${XMLPARSER_TEST_FILES}

tests/htmltree.check: tests/xmlreader ${HTMLTREE_TEST_FILES}

noinst_bin_PROGRAMS += tests/ttslanguages.test

tests_ttslanguages_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_ttslanguages_test_SOURCES = tests/ttslanguages.cpp

noinst_bin_PROGRAMS += tests/languages.test

tests_languages_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_languages_test_SOURCES = tests/languages.cpp

noinst_bin_PROGRAMS += tests/utf8.test

tests_utf8_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_utf8_test_SOURCES = tests/utf8.cpp

noinst_bin_PROGRAMS += tests/parsetext

tests_parsetext_LDADD   = src/libcainteoir/libcainteoir.la
tests_parsetext_SOURCES = tests/parsetext.cpp

tests/parsetext.check: tests/parsetext ${PARSETEXT_TEST_FILES}

noinst_bin_PROGRAMS += tests/mimetype

tests_mimetype_LDADD   = src/libcainteoir/libcainteoir.la
tests_mimetype_SOURCES = tests/mimetype.cpp

noinst_bin_PROGRAMS += tests/content.test

tests_content_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_content_test_SOURCES = tests/content.cpp

noinst_bin_PROGRAMS += tests/phoneme.test

tests_phoneme_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_phoneme_test_SOURCES = tests/phoneme.cpp

noinst_bin_PROGRAMS += tests/phoneme_stress.test

tests_phoneme_stress_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_phoneme_stress_test_SOURCES = tests/phoneme_stress.cpp

noinst_bin_PROGRAMS += tests/trie.test

tests_trie_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_trie_test_SOURCES = tests/trie.cpp

noinst_bin_PROGRAMS += tests/styles

tests_styles_LDADD   = src/libcainteoir/libcainteoir.la
tests_styles_SOURCES = tests/styles.cpp

noinst_bin_PROGRAMS += tests/duration.test

tests_duration_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_duration_test_SOURCES = tests/duration.cpp

noinst_bin_PROGRAMS += tests/pho_file.test

tests_pho_file_test_LDADD   = src/libcainteoir/libcainteoir.la
tests_pho_file_test_SOURCES = tests/pho_file.cpp

tests/styles.check: tests/styles ${CSS_TEST_FILES}

tests/dictionary.check: src/apps/dictionary ${DICTIONARY_TEST_FILES}

tests/phonemeset.check: src/apps/phoneme-converter ${PHONEMESET_TEST_FILES}

tests/accent.check: src/apps/phoneme-converter ${ACCENT_TEST_FILES}

tests/metadata.check: src/apps/metadata ${DOCUMENT_TEST_FILES}

tests/vorbis-comments.check: src/apps/metadata ${DOCUMENT_TEST_FILES}

tests/diphones.check: src/examples/voice-synthesizer ${DIPHONE_TEST_FILES}

tests/prosody.check: src/examples/voice-synthesizer ${PROSODY_TEST_FILES}

check: \
	tests/range.check \
	tests/buffer.check \
	tests/path.check \
	tests/multiword_entry.check \
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
	tests/htmltree.check \
	tests/events.check \
	tests/phoneme.check \
	tests/trie.check \
	tests/phonemeset.check \
	tests/accent.check \
	tests/phoneme_stress.check \
	tests/dictionary.check \
	tests/parsetext.check \
	tests/pho_file.check \
	tests/diphones.check \
	tests/duration.check \
	tests/prosody.check
