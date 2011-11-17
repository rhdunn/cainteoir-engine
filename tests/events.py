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
			{'test': 'html/semantics/simple-no-namespace.xhtml', 'result': 'html/semantics/simple.events'},
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
			{'test': 'html/sections/headings.html', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/headings.xhtml', 'result': 'html/sections/headings.events'},
			{'test': 'html/sections/headings-with-id.html', 'result': 'html/sections/headings-with-id.events'},
			{'test': 'html/sections/headings-with-id.xhtml', 'result': 'html/sections/headings-with-id.events'},
			{'test': 'html/sections/paragraphs.html', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/paragraphs.xhtml', 'result': 'html/sections/paragraphs.events'},
			{'test': 'html/sections/no-body.html', 'result': 'html/sections/no-body.events'},
			{'test': 'html/sections/no-body.xhtml', 'result': 'html/sections/no-body.events'},
			{'test': 'html/sections/pre.html', 'result': 'html/sections/pre.events'},
			{'test': 'html/sections/pre.xhtml', 'result': 'html/sections/pre.events'},
			{'test': 'html/sections/div.html', 'result': 'html/sections/div.events'},
			{'test': 'html/sections/div.xhtml', 'result': 'html/sections/div.events'},
			{'test': 'html/sections/ul.html', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/ul.xhtml', 'result': 'html/sections/ul.events'},
			{'test': 'html/sections/ol.html', 'result': 'html/sections/ol.events'},
			{'test': 'html/sections/ol.xhtml', 'result': 'html/sections/ol.events'},
		]},
		{'name': 'text-level semantics', 'type': 'events', 'tests': [
			{'test': 'html/text-level-semantics/em.html', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/em.xhtml', 'result': 'html/text-level-semantics/em.events'},
			{'test': 'html/text-level-semantics/strong.html', 'result': 'html/text-level-semantics/strong.events'},
			{'test': 'html/text-level-semantics/strong.xhtml', 'result': 'html/text-level-semantics/strong.events'},
			{'test': 'html/text-level-semantics/cite.html', 'result': 'html/text-level-semantics/cite.events'},
			{'test': 'html/text-level-semantics/cite.xhtml', 'result': 'html/text-level-semantics/cite.events'},
			{'test': 'html/text-level-semantics/i.html', 'result': 'html/text-level-semantics/i.events'},
			{'test': 'html/text-level-semantics/i.xhtml', 'result': 'html/text-level-semantics/i.events'},
			{'test': 'html/text-level-semantics/b.html', 'result': 'html/text-level-semantics/b.events'},
			{'test': 'html/text-level-semantics/b.xhtml', 'result': 'html/text-level-semantics/b.events'},
			{'test': 'html/text-level-semantics/code.html', 'result': 'html/text-level-semantics/code.events'},
			{'test': 'html/text-level-semantics/code.xhtml', 'result': 'html/text-level-semantics/code.events'},
			{'test': 'html/text-level-semantics/sub.html', 'result': 'html/text-level-semantics/sub.events'},
			{'test': 'html/text-level-semantics/sub.xhtml', 'result': 'html/text-level-semantics/sub.events'},
			{'test': 'html/text-level-semantics/sup.html', 'result': 'html/text-level-semantics/sup.events'},
			{'test': 'html/text-level-semantics/sup.xhtml', 'result': 'html/text-level-semantics/sup.events'},
		]},
	]})
	test.run({'name': 'MIME', 'groups': [
		{'name': 'headers', 'type': 'events', 'tests': [
			{'test': 'mime/headers/basic.txt', 'result': 'mime/headers/basic.events'},
			{'test': 'mime/headers/leading-whitespace.txt', 'result': 'mime/headers/basic.events'},
			{'test': 'mime/headers/wrapped-header-item.txt', 'result': 'mime/headers/basic.events'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'mime/headers/basic-lfonly.txt', 'result': 'mime/headers/basic.events'},
			{'test': 'mime/headers/leading-whitespace-lfonly.txt', 'result': 'mime/headers/basic.events'},
			{'test': 'mime/headers/wrapped-header-item-lfonly.txt', 'result': 'mime/headers/basic.events'},
		]},
		{'name': 'multipart', 'type': 'events', 'tests': [
			{'test': 'mime/multipart/mixed-1.txt', 'result': 'mime/multipart/mixed.events'},
			{'test': 'mime/multipart/mixed-2.txt', 'result': 'mime/multipart/mixed.events'},
			{'test': 'mime/multipart/related-1.txt', 'result': 'mime/multipart/related.events'},
			{'test': 'mime/multipart/related-2.txt', 'result': 'mime/multipart/related.events'},
			{'test': 'mime/multipart/related-3.txt', 'result': 'mime/multipart/related.events'},
			{'test': 'mime/multipart/related-4.txt', 'result': 'mime/multipart/related.events'},
		]},
		{'name': 'quoted-printable', 'type': 'events', 'tests': [
			{'test': 'mime/quoted-printable/ascii.txt', 'result': 'mime/quoted-printable/ascii.events'},
			{'test': 'mime/quoted-printable/escaped-upper.txt', 'result': 'mime/quoted-printable/escaped.events'},
			{'test': 'mime/quoted-printable/escaped-lower.txt', 'result': 'mime/quoted-printable/escaped.events'},
			{'test': 'mime/quoted-printable/eol.txt', 'result': 'mime/quoted-printable/eol.events'},
		]},
		{'name': 'base64', 'type': 'events', 'tests': [
			{'test': 'mime/base64/simple.txt',  'result': 'mime/base64/simple.events'},
			{'test': 'mime/base64/padding.txt', 'result': 'mime/base64/padding.events'},
			{'test': 'mime/base64/wrapped.txt', 'result': 'mime/base64/wrapped.events'},
		]},
	]})
	test.run({'name': 'HTTP', 'groups': [
		{'name': '1.0', 'type': 'events', 'tests': [
			{'test': 'http/header-only-1.0.txt', 'result': 'http/header-only-1.0.events'},
			{'test': 'http/basic-1.0.txt', 'result': 'http/basic.events'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'http/basic-1.0-lfonly.txt', 'result': 'http/basic.events'},
		]},
		{'name': '1.1', 'type': 'events', 'tests': [
			{'test': 'http/header-only-1.1.txt', 'result': 'http/header-only-1.1.events'},
			{'test': 'http/basic-1.1.txt', 'result': 'http/basic.events'},
			# some editors will convert the CR LF to LF, so LF only line endings need to be supported ...
			{'test': 'http/basic-1.1-lfonly.txt', 'result': 'http/basic.events'},
		]},
	]})
	test.run({'name': 'NCX', 'groups': [
		{'name': 'toc', 'type': 'events', 'tests': [
			{'test': 'ncx/navMap/linear.ncx', 'result': 'ncx/navMap/linear.events'},
			{'test': 'ncx/navMap/in-subdir.ncx', 'result': 'ncx/navMap/in-subdir.events'},
			{'test': 'ncx/navMap/named-uri.ncx', 'result': 'ncx/navMap/named-uri.events'},
			{'test': 'ncx/navMap/nested.ncx', 'result': 'ncx/navMap/nested.events'},
		]},
	]})
	test.run({'name': 'RTF', 'groups': [
		{'name': 'document', 'type': 'events', 'tests': [
			{'test': 'rtf/simple.rtf', 'result': 'rtf/simple.events'},
			{'test': 'rtf/paragraph.rtf', 'result': 'rtf/paragraph.events'},
			{'test': 'rtf/escaped.rtf', 'result': 'rtf/escaped.events'},
			{'test': 'rtf/hex-ansi.rtf', 'result': 'rtf/hex-ansi.events'},
			{'test': 'rtf/libreoffice.rtf', 'result': 'rtf/simple.events'},
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
				('OEBPS/test.html', 'html/semantics/simple.xhtml')
			],
		'groups': [
			{'name': 'with-content', 'type': 'events', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'epub/simple.events'},
				{'test': 'epub/missing-html-file.opf', 'result': 'epub/simple.events'},
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
