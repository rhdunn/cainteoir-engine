#!/usr/bin/python

# Copyright (C) 2010-2011 Reece H. Dunn
#
# This file is part of cainteoir-engine.
#
# cainteoir-engine is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cainteoir-engine is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cainteoir-engine.  If not, see <http://www.gnu.org/licenses/>.

import harness
import sys

if __name__ == '__main__':
	test = harness.TestSuite('events', sys.argv)
	test.run({'name': 'HTML', 'groups': [
		{'name': 'semantics', 'type': 'events', 'tests': [
			{'test': 'html/semantics/simple.html', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple.xhtml', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple-with-doctype.xhtml', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple-no-namespace.xhtml', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple-with-xml-magic.html', 'result': 'html/semantics/simple.events'},
			{'test': 'html/semantics/simple-rec-html40-ns.html', 'result': 'html/semantics/simple.events'}, # MS Word
			{'test': 'html/semantics/with-style.html', 'result': 'html/semantics/with-style.events'},
			{'test': 'html/semantics/with-style.xhtml', 'result': 'html/semantics/with-style.events'},
			{'test': 'html/semantics/with-style-type.html', 'result': 'html/semantics/with-style-type.events'},
			{'test': 'html/semantics/with-style-type.xhtml', 'result': 'html/semantics/with-style-type.events'},
			{'test': 'html/semantics/meta-content-type.html', 'result': 'html/semantics/meta-content-type.events'},
			{'test': 'html/semantics/meta-content-type.xhtml', 'result': 'html/semantics/meta-content-type.events'},
			{'test': 'html/semantics/link-rel-stylesheet.html', 'result': 'html/semantics/link-rel-stylesheet.events'},
			{'test': 'html/semantics/link-rel-stylesheet.xhtml', 'result': 'html/semantics/link-rel-stylesheet.events'},
		]},
		{'name': 'scripting', 'type': 'events', 'tests': [
			{'test': 'html/scripting/with-script.html', 'result': 'html/scripting/with-script.events'},
			{'test': 'html/scripting/with-script.xhtml', 'result': 'html/scripting/with-script.events'},
			{'test': 'html/scripting/with-script-type.html', 'result': 'html/scripting/with-script-type.events'},
			{'test': 'html/scripting/with-script-type.xhtml', 'result': 'html/scripting/with-script-type.events'},
		]},
		{'name': 'sections', 'type': 'events', 'tests': [
			{'test': 'html/sections/dir.html', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/dir.xhtml', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/div.html', 'result': 'html/sections/div.events'},
			{'test': 'html/sections/div.xhtml', 'result': 'html/sections/div.events'},
			{'test': 'html/sections/headings.html', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/headings.xhtml', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/headings-with-id.html', 'result': 'html/sections/headings-with-id.events'},
			{'test': 'html/sections/headings-with-id.xhtml', 'result': 'html/sections/headings-with-id.events'},
			{'test': 'html/sections/heading-with-matching-title.html', 'result': 'html/sections/heading-with-matching-title.events'},
			{'test': 'html/sections/heading-with-matching-title.xhtml', 'result': 'html/sections/heading-with-matching-title.events'},
			{'test': 'html/sections/heading-repeated-with-matching-title.html', 'result': 'html/sections/heading-repeated-with-matching-title.events'},
			{'test': 'html/sections/heading-repeated-with-matching-title.xhtml', 'result': 'html/sections/heading-repeated-with-matching-title.events'},
			{'test': 'html/sections/li-only.html', 'result': 'html/sections/li-only.events'},
			{'test': 'html/sections/li-only.xhtml', 'result': 'html/sections/li-only.events'},
			{'test': 'html/sections/menu.html', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/menu.xhtml', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/no-body.html', 'result': 'html/sections/no-body.events'},
			{'test': 'html/sections/no-body.xhtml', 'result': 'html/sections/no-body.events'},
			{'test': 'html/sections/ol.html', 'result': 'html/sections/ol.events'},
			{'test': 'html/sections/ol.xhtml', 'result': 'html/sections/ol.events'},
			{'test': 'html/sections/ol-cdata.xhtml', 'result': 'html/sections/ol.events'},
			{'test': 'html/sections/paragraphs.html', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/paragraphs.xhtml', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/paragraphs-cdata.xhtml', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/pre.html', 'result': 'html/sections/pre.events'},
			{'test': 'html/sections/pre.xhtml', 'result': 'html/sections/pre.events'},
			{'test': 'html/sections/pre-only-element.html', 'result': 'html/sections/pre-only-element.events'},
			{'test': 'html/sections/ul.html', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/ul.xhtml', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/ul-cdata.xhtml', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/unclosed-head.html', 'result': 'html/sections/unclosed-head.events'},
			{'test': 'html/sections/html-only.html', 'result': 'html/sections/html-only.events'},
		]},
		{'name': 'text-level semantics', 'type': 'events', 'tests': [
			{'test': 'html/text-level-semantics/address.html', 'result': 'html/text-level-semantics/address.events'},
			{'test': 'html/text-level-semantics/address.xhtml', 'result': 'html/text-level-semantics/address.events'},
			{'test': 'html/text-level-semantics/b.html', 'result': 'html/text-level-semantics/b.events'},
			{'test': 'html/text-level-semantics/b.xhtml', 'result': 'html/text-level-semantics/b.events'},
			{'test': 'html/text-level-semantics/cite.html', 'result': 'html/text-level-semantics/cite.events'},
			{'test': 'html/text-level-semantics/cite.xhtml', 'result': 'html/text-level-semantics/cite.events'},
			{'test': 'html/text-level-semantics/code.html', 'result': 'html/text-level-semantics/code.events'},
			{'test': 'html/text-level-semantics/code.xhtml', 'result': 'html/text-level-semantics/code.events'},
			{'test': 'html/text-level-semantics/dfn.html', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/dfn.xhtml', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/em.html', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/em.xhtml', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/i.html', 'result': 'html/text-level-semantics/i.events'},
			{'test': 'html/text-level-semantics/i.xhtml', 'result': 'html/text-level-semantics/i.events'},
			{'test': 'html/text-level-semantics/span.html', 'result': 'html/text-level-semantics/span.events'},
			{'test': 'html/text-level-semantics/span.xhtml', 'result': 'html/text-level-semantics/span.events'},
			{'test': 'html/text-level-semantics/strong.html', 'result': 'html/text-level-semantics/strong.events'},
			{'test': 'html/text-level-semantics/strong.xhtml', 'result': 'html/text-level-semantics/strong.events'},
			{'test': 'html/text-level-semantics/sub.html', 'result': 'html/text-level-semantics/sub.events'},
			{'test': 'html/text-level-semantics/sub.xhtml', 'result': 'html/text-level-semantics/sub.events'},
			{'test': 'html/text-level-semantics/sup.html', 'result': 'html/text-level-semantics/sup.events'},
			{'test': 'html/text-level-semantics/sup.xhtml', 'result': 'html/text-level-semantics/sup.events'},
			{'test': 'html/text-level-semantics/tt.html', 'result': 'html/text-level-semantics/tt.events'},
			{'test': 'html/text-level-semantics/tt.xhtml', 'result': 'html/text-level-semantics/tt.events'},
			{'test': 'html/text-level-semantics/u.html', 'result': 'html/text-level-semantics/u.events'},
			{'test': 'html/text-level-semantics/u.xhtml', 'result': 'html/text-level-semantics/u.events'},
			{'test': 'html/text-level-semantics/var.html', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/var.xhtml', 'result': 'html/text-level-semantics/em.events'},
		]},
		{'name': 'table', 'type': 'events', 'tests': [
			{'test': 'html/table/simple.html', 'result': 'html/table/simple.events'},
			{'test': 'html/table/simple.xhtml', 'result': 'html/table/simple.events'},
		]},
		{'name': 'phrasing content', 'type': 'events', 'tests': [
			{'test': 'html/phrasing-content/br.html', 'result': 'html/phrasing-content/br.events'},
			{'test': 'html/phrasing-content/br.xhtml', 'result': 'html/phrasing-content/br.events'},
		]},
		{'name': 'entities', 'type': 'events', 'tests': [
			{'test': 'html/entities.html', 'result': 'html/entities.events'},
			{'test': 'html/entities.xhtml', 'result': 'html/entities.events'},
		]},
		{'name': 'encoding', 'replace': ['title'], 'type': 'events', 'tests': [
			{'test': 'html/encoding/meta-charset-utf8.html', 'result': 'html/encoding/encoding.events', 'title': 'meta-charset-utf8.html'},
			{'test': 'html/encoding/meta-charset-win1252.html', 'result': 'html/encoding/encoding.events', 'title': 'meta-charset-win1252.html'},
			{'test': 'html/encoding/meta-content-type-utf8.html', 'result': 'html/encoding/encoding.events', 'title': 'meta-content-type-utf8.html'},
			{'test': 'html/encoding/meta-content-type-win1252.html', 'result': 'html/encoding/encoding.events', 'title': 'meta-content-type-win1252.html'},
		]},
	]})
	test.run({'name': 'MIME', 'replace': ['title'], 'groups': [
		{'name': 'headers', 'type': 'events', 'tests': [
			{'test': 'mime/headers/basic.txt', 'result': 'mime/headers/basic.events', 'title': 'basic.txt'},
			{'test': 'mime/headers/leading-whitespace.txt', 'result': 'mime/headers/basic.events', 'title': 'leading-whitespace.txt'},
			{'test': 'mime/headers/wrapped-header-item.txt', 'result': 'mime/headers/basic.events', 'title': 'wrapped-header-item.txt'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'mime/headers/basic-lfonly.txt', 'result': 'mime/headers/basic.events', 'title': 'basic-lfonly.txt'},
			{'test': 'mime/headers/leading-whitespace-lfonly.txt', 'result': 'mime/headers/basic.events', 'title': 'leading-whitespace-lfonly.txt'},
			{'test': 'mime/headers/wrapped-header-item-lfonly.txt', 'result': 'mime/headers/basic.events', 'title': 'wrapped-header-item-lfonly.txt'},
		]},
		{'name': 'email', 'type': 'events', 'tests': [
			{'test': 'mime/email/subject.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from.txt', 'result': 'mime/email/content.events', 'title': 'from.txt'},
			{'test': 'mime/email/from-quoted.txt', 'result': 'mime/email/content.events', 'title': 'from-quoted.txt'},
			{'test': 'mime/email/from-inverted.txt', 'result': 'mime/email/content.events', 'title': 'from-inverted.txt'},
			{'test': 'mime/email/from-escaped-email.txt', 'result': 'mime/email/content.events', 'title': 'from-escaped-email.txt'},
			{'test': 'mime/email/from-no-email.txt', 'result': 'mime/email/content.events', 'title': 'from-no-email.txt'},
			{'test': 'mime/email/from-email-only.txt', 'result': 'mime/email/content.events', 'title': 'from-email-only.txt'},
			{'test': 'mime/email/from-email-email.txt', 'result': 'mime/email/content.events', 'title': 'from-email-email.txt'},
			{'test': 'mime/email/from-subject.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from-header.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from-archived.txt', 'result': 'mime/email/content.events', 'title': 'from-archived.txt'},
			{'test': 'mime/email/date-jan.txt', 'result': 'mime/email/content.events', 'title': 'date-jan.txt'},
			{'test': 'mime/email/mdate-jan.txt', 'result': 'mime/email/content.events', 'title': 'mdate-jan.txt'},
			{'test': 'mime/email/newsgroups.txt', 'result': 'mime/email/content.events', 'title': 'newsgroups.txt'},
			{'test': 'mime/email/article-subject.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'mime/email/subject-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-lfonly.txt'},
			{'test': 'mime/email/from-quoted-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-quoted-lfonly.txt'},
			{'test': 'mime/email/from-inverted-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-inverted-lfonly.txt'},
			{'test': 'mime/email/from-escaped-email-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-escaped-email-lfonly.txt'},
			{'test': 'mime/email/from-no-email-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-no-email-lfonly.txt'},
			{'test': 'mime/email/from-email-only-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-email-only-lfonly.txt'},
			{'test': 'mime/email/from-email-email-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-email-email-lfonly.txt'},
			{'test': 'mime/email/from-subject-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from-header-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/email/from-archived-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'from-archived-lfonly.txt'},
			{'test': 'mime/email/date-jan-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'date-jan-lfonly.txt'},
			{'test': 'mime/email/mdate-jan-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'mdate-jan-lfonly.txt'},
			{'test': 'mime/email/newsgroups-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'newsgroups-lfonly.txt'},
			{'test': 'mime/email/article-subject-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
		]},
		{'name': 'story-metadata', 'type': 'events', 'tests': [
			{'test': 'mime/story-metadata/title.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/story-metadata/story.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/story-metadata/author.txt', 'result': 'mime/email/content.events', 'title': 'author.txt'},
			{'test': 'mime/story-metadata/keywords.txt', 'result': 'mime/email/content.events', 'title': 'keywords.txt'},
			{'test': 'mime/story-metadata/title-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/story-metadata/story-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'Test Case'},
			{'test': 'mime/story-metadata/author-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'author-lfonly.txt'},
			{'test': 'mime/story-metadata/keywords-lfonly.txt', 'result': 'mime/email/content.events', 'title': 'keywords-lfonly.txt'},
		]},
		{'name': 'multipart', 'type': 'events', 'tests': [
			{'test': 'mime/multipart/mixed-1.txt', 'result': 'mime/multipart/mixed.events', 'title': 'mixed-1.txt'},
			{'test': 'mime/multipart/mixed-2.txt', 'result': 'mime/multipart/mixed.events', 'title': 'mixed-2.txt'},
			{'test': 'mime/multipart/related-1.txt', 'result': 'mime/multipart/related.events', 'title': 'related-1.txt'},
			{'test': 'mime/multipart/related-2.txt', 'result': 'mime/multipart/related.events', 'title': 'related-2.txt'},
			{'test': 'mime/multipart/related-3.txt', 'result': 'mime/multipart/related.events', 'title': 'related-3.txt'},
			{'test': 'mime/multipart/related-4.txt', 'result': 'mime/multipart/related.events', 'title': 'related-4.txt'},
			{'test': 'mime/multipart/related-5.txt', 'result': 'mime/multipart/related-html.events', 'title': 'related-5.txt'},
			{'test': 'mime/multipart/related-6.txt', 'result': 'mime/multipart/related-html.events', 'title': 'related-6.txt'},
			{'test': 'mime/multipart/alternative-1.txt', 'result': 'mime/multipart/mixed.events', 'title': 'alternative-1.txt'},
			{'test': 'mime/multipart/alternative-2.txt', 'result': 'mime/multipart/mixed.events', 'title': 'alternative-2.txt'},
			{'test': 'mime/multipart/no-boundary-1.txt', 'result': 'mime/multipart/no-boundary.events', 'title': 'no-boundary-1.txt'},
			{'test': 'mime/multipart/no-boundary-2.txt', 'result': 'mime/multipart/no-boundary.events', 'title': 'no-boundary-2.txt'},
			{'test': 'mime/multipart/mail-archive.txt', 'result': 'mime/multipart/mail-archive.events', 'title': 'mail-archive.txt', 'expect': 'fail'},
		]},
		{'name': 'quoted-printable', 'type': 'events', 'tests': [
			{'test': 'mime/quoted-printable/ascii.txt', 'result': 'mime/quoted-printable/ascii.events', 'title': 'ascii.txt'},
			{'test': 'mime/quoted-printable/escaped-upper.txt', 'result': 'mime/quoted-printable/escaped.events', 'title': 'escaped-upper.txt'},
			{'test': 'mime/quoted-printable/escaped-lower.txt', 'result': 'mime/quoted-printable/escaped.events', 'title': 'escaped-lower.txt'},
			{'test': 'mime/quoted-printable/eol.txt', 'result': 'mime/quoted-printable/eol.events', 'title': 'eol.txt'},
		]},
		{'name': 'base64', 'type': 'events', 'tests': [
			{'test': 'mime/base64/simple.txt',  'result': 'mime/base64/simple.events',  'title': 'simple.txt'},
			{'test': 'mime/base64/padding.txt', 'result': 'mime/base64/padding.events', 'title': 'padding.txt'},
			{'test': 'mime/base64/wrapped.txt', 'result': 'mime/base64/wrapped.events', 'title': 'wrapped.txt'},
		]},
		{'name': 'wrapped in html', 'type': 'events', 'tests': [
			{'test': 'mime/wrapped-in-html/subject.txt', 'result': 'mime/wrapped-in-html/basic.events', 'title': 'Test Case'},
			{'test': 'mime/wrapped-in-html/with-comment.txt', 'result': 'mime/wrapped-in-html/basic.events', 'title': 'Test Case'},
			{'test': 'mime/wrapped-in-html/from.txt', 'result': 'mime/wrapped-in-html/basic.events', 'title': 'from.txt'},
		]},
		{'name': 'encoding', 'type': 'events', 'tests': [
			{'test': 'mime/encoding-utf8.txt', 'result': 'mime/encoding.events', 'title': 'encoding-utf8.txt'},
			{'test': 'mime/encoding-win1252.txt', 'result': 'mime/encoding.events', 'title': 'encoding-win1252.txt'},
		]},
	]})
	test.run({'name': 'HTTP', 'replace': ['title'], 'groups': [
		{'name': '1.0', 'type': 'events', 'tests': [
			{'test': 'http/header-only-1.0.txt', 'result': 'http/header-only-1.0.events', 'title': 'header-only-1.0.txt'},
			{'test': 'http/basic-1.0.txt', 'result': 'http/basic.events', 'title': 'basic-1.0.txt'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'http/basic-1.0-lfonly.txt', 'result': 'http/basic.events', 'title': 'basic-1.0-lfonly.txt'},
		]},
		{'name': '1.1', 'type': 'events', 'tests': [
			{'test': 'http/header-only-1.1.txt', 'result': 'http/header-only-1.1.events', 'title': 'header-only-1.1.txt'},
			{'test': 'http/basic-1.1.txt', 'result': 'http/basic.events', 'title': 'basic-1.1.txt'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'http/basic-1.1-lfonly.txt', 'result': 'http/basic.events', 'title': 'basic-1.1-lfonly.txt'},
		]},
	]})
	test.run({'name': 'NCX', 'groups': [
		{'name': 'toc', 'type': 'events', 'tests': [
			{'test': 'ncx/navMap/linear.ncx', 'result': 'ncx/navMap/linear.events'},
			{'test': 'ncx/navMap/linear-content-before-label.ncx', 'result': 'ncx/navMap/linear.events'},
			{'test': 'ncx/navMap/linear-whitespace.ncx', 'result': 'ncx/navMap/linear.events'},
			{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'ncx/navMap/in-subdir.events'},
			{'test': 'ncx/navMap/named-uri.ncx', 'result': 'ncx/navMap/named-uri.events'},
			{'test': 'ncx/navMap/nested.ncx', 'result': 'ncx/navMap/nested.events'},
		]},
	]})
	test.run({'name': 'RTF', 'groups': [
		{'name': 'document', 'type': 'events', 'tests': [
			{'test': 'rtf/simple.rtf', 'result': 'rtf/simple.events'},
			{'test': 'rtf/garbage-at-end.rtf', 'result': 'rtf/garbage-at-end.events'},
			{'test': 'rtf/incomplete.rtf', 'result': 'rtf/incomplete.events'},
			{'test': 'rtf/paragraph.rtf', 'result': 'rtf/paragraph.events'},
			{'test': 'rtf/escaped.rtf', 'result': 'rtf/escaped.events'},
			{'test': 'rtf/hex-ansi.rtf', 'result': 'rtf/hex-ansi.events'},
			{'test': 'rtf/libreoffice.rtf', 'result': 'rtf/libreoffice.events'},
		]},
	]})
	test.run({'name': 'SSML', 'groups': [
		{'name': 'document', 'type': 'events', 'tests': [
			{'test': 'ssml/speak.ssml', 'result': 'ssml/speak.events'},
			{'test': 'ssml/speak-with-schema.ssml', 'result': 'ssml/speak.events'},
			{'test': 'ssml/p.ssml', 'result': 'ssml/p.events'},
			{'test': 'ssml/s.ssml', 'result': 'ssml/s.events'},
			{'test': 'ssml/emphasis.ssml', 'result': 'ssml/emphasis.events'},
		]},
	]})
	test.run({ 'name': 'ePub',
		'archive':
			[
				('mimetype', 'application/epub+zip'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml'),
			],
		'groups': [
			{'name': 'with-content', 'type': 'events', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'epub/simple.events'},
				{'test': 'epub/missing-html-file.opf', 'result': 'epub/missing.events'},
			]},
		]
	})
	test.run({ 'name': 'ePub',
		'archive':
			[
				('mimetype', 'application/epub+zip\n'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml'),
			],
		'groups': [
			{'name': 'mimetype with extra content at the end', 'type': 'events', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'epub/simple.events'},
				{'test': 'epub/missing-html-file.opf', 'result': 'epub/missing.events'},
			]},
		]
	})
	test.run({ 'name': 'ePub',
		'archive':
			[
				# incorrectly constructed ePUb document -- the mimetype
				# file is the last entry, not the first:
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml'),
				('mimetype', 'application/epub+zip'),
			],
		'groups': [
			{'name': 'mimetype at end', 'type': 'events', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'epub/simple.events'},
				{'test': 'epub/missing-html-file.opf', 'result': 'epub/missing.events'},
			]},
		]
	})
	test.run({ 'name': 'ZIP',
		'archive':
			[
				('test.xhtml', 'html/semantics/simple.xhtml'),
			],
		'groups': [
			{'name': 'single file', 'type': 'events', 'tests': [
				{'test': 'single-file.zip', 'result': 'zip/single-file.events'},
			]},
		]
	})
	test.run({ 'name': 'ZIP',
		'archive':
			[
				('test.xhtml', 'html/semantics/simple.xhtml'),
				('test.html',  'html/semantics/simple.html'),
			],
		'groups': [
			{'name': 'multiple files', 'type': 'events', 'tests': [
				{'test': 'multiple-files.zip', 'result': 'zip/multiple-files.events'},
			]},
		]
	})
	test.run({ 'name': 'ZIP',
		'archive':
			[
				('testdir/', ''), # Don't set the directory attribute
				('testdir/test.xhtml', 'html/semantics/simple.xhtml'),
			],
		'groups': [
			{'name': 'single file in directory', 'type': 'events', 'tests': [
				{'test': 'single-file-in-dir.zip', 'result': 'zip/single-file-in-dir.events'},
			]},
		]
	})
	test.run({ 'name': 'ZIP',
		'archive':
			[
				('testdir/', None), # Set the directory attribute
				('testdir/test.xhtml', 'html/semantics/simple.xhtml'),
			],
		'groups': [
			{'name': 'single file in directory with directory attribute set', 'type': 'events', 'tests': [
				{'test': 'single-file-in-dir.zip', 'result': 'zip/single-file-in-dir.events'},
			]},
		]
	})
	test.run({'name': 'Compressed', 'groups': [
		{'name': 'gzip', 'type': 'events', 'compress': 'gzip', 'tests': [
			{'test': 'html/semantics/simple.html', 'result': 'html/semantics/simple.events'},
		]},
		{'name': 'bzip2', 'type': 'events', 'compress': 'bzip2', 'tests': [
			{'test': 'html/semantics/simple.html', 'result': 'html/semantics/simple.events', 'expect': 'fail'},
		]},
		{'name': 'lzma', 'type': 'events', 'compress': 'lzma', 'tests': [
			{'test': 'html/semantics/simple.html', 'result': 'html/semantics/simple.events', 'expect': 'fail'},
		]},
	]})
	test.summary()
