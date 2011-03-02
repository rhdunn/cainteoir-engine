#!/usr/bin/python

# Copyright (C) 2010 Reece H. Dunn
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

if __name__ == '__main__':
	test = harness.TestSuite('XMLReader')
	test.run_tests(name='xml reader (text)', testtype='xmlreader', tests=[
		('xmlparser/text/basic.xml', 'xmlparser/text/basic.tags', 'expect-pass'),
		('xmlparser/text/multiline.xml', 'xmlparser/text/multiline.tags', 'expect-pass'),
		('xmlparser/text/with-lessthan.xml', 'xmlparser/text/with-lessthan.tags', 'expect-pass'),
		('xmlparser/text/in-tags.xml', 'xmlparser/text/in-tags.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (comments)', testtype='xmlreader', tests=[
		('xmlparser/comments/comment.xml', 'xmlparser/comments/comment.tags', 'expect-pass'),
		('xmlparser/comments/comment-in-a-tag.xml', 'xmlparser/comments/comment-in-a-tag.tags', 'expect-pass'),
		('xmlparser/comments/comment-with-dash.xml', 'xmlparser/comments/comment-with-dash.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (cdata section)', testtype='xmlreader', tests=[
		('xmlparser/cdata-section/cdata-section.xml', 'xmlparser/cdata-section/cdata-section.tags', 'expect-pass'),
		('xmlparser/cdata-section/cdata-section-in-a-tag.xml', 'xmlparser/cdata-section/cdata-section-in-a-tag.tags', 'expect-pass'),
		('xmlparser/cdata-section/cdata-section-with-amp.xml', 'xmlparser/cdata-section/cdata-section-with-amp.tags', 'expect-pass'),
		('xmlparser/cdata-section/cdata-section-with-tags.xml', 'xmlparser/cdata-section/cdata-section-with-tags.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (element)', testtype='xmlreader', tests=[
		('xmlparser/element/no-args.xml', 'xmlparser/element/no-args.tags', 'expect-pass'),
		('xmlparser/element/args.xml', 'xmlparser/element/args.tags', 'expect-pass'),
		('xmlparser/element/space-before.xml', 'xmlparser/element/space-before.tags', 'expect-pass'),
		('xmlparser/element/partial.xml', 'xmlparser/element/partial.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (empty element)', testtype='xmlreader', tests=[
		('xmlparser/empty-element/no-args.xml', 'xmlparser/empty-element/no-args.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (doctype)', testtype='xmlreader', tests=[
		('xmlparser/doctype/html40.xml', 'xmlparser/doctype/html40.tags', 'expect-pass'),
		('xmlparser/doctype/html40-with-dtd.xml', 'xmlparser/doctype/html40-with-dtd.tags', 'expect-pass'),
		('xmlparser/doctype/html401.xml', 'xmlparser/doctype/html401.tags', 'expect-pass'),
		('xmlparser/doctype/html401-with-dtd.xml', 'xmlparser/doctype/html401-with-dtd.tags', 'expect-pass'),
		('xmlparser/doctype/xhtml10-strict.xml', 'xmlparser/doctype/xhtml10-strict.tags', 'expect-pass'),
		('xmlparser/doctype/xhtml11.xml', 'xmlparser/doctype/xhtml11.tags', 'expect-pass'),
		('xmlparser/doctype/html5-lower.xml', 'xmlparser/doctype/html5-lower.tags', 'expect-pass'),
		('xmlparser/doctype/html5-upper.xml', 'xmlparser/doctype/html5-upper.tags', 'expect-pass'),
		('xmlparser/doctype/html5-compat-double-quote.xml', 'xmlparser/doctype/html5-compat-double-quote.tags', 'expect-pass'),
		('xmlparser/doctype/html5-compat-single-quote.xml', 'xmlparser/doctype/html5-compat-single-quote.tags', 'expect-pass'),
	])
	test.run_tests(name='xml reader (entity)', testtype='xmlreader', tests=[
		('xmlparser/entity/1byte-utf8-hex.xml', 'xmlparser/entity/1byte-utf8-hex.tags', 'expect-pass'),
		('xmlparser/entity/2byte-utf8-hex.xml', 'xmlparser/entity/2byte-utf8-hex.tags', 'expect-pass'),
		('xmlparser/entity/3byte-utf8-hex.xml', 'xmlparser/entity/3byte-utf8-hex.tags', 'expect-pass'),
		('xmlparser/entity/4byte-utf8-hex.xml', 'xmlparser/entity/4byte-utf8-hex.tags', 'expect-pass'),
		('xmlparser/entity/1byte-utf8-int.xml', 'xmlparser/entity/1byte-utf8-int.tags', 'expect-pass'),
		('xmlparser/entity/2byte-utf8-int.xml', 'xmlparser/entity/2byte-utf8-int.tags', 'expect-pass'),
		('xmlparser/entity/3byte-utf8-int.xml', 'xmlparser/entity/3byte-utf8-int.tags', 'expect-pass'),
		('xmlparser/entity/4byte-utf8-int.xml', 'xmlparser/entity/4byte-utf8-int.tags', 'expect-pass'),
		('xmlparser/entity/invalid-char.xml', 'xmlparser/entity/invalid-char.tags', 'expect-pass'),
		('xmlparser/entity/unknown.xml', 'xmlparser/entity/unknown.tags', 'expect-pass'),
		('xmlparser/entity/xml.xml', 'xmlparser/entity/xml.tags', 'expect-pass'),
		('xmlparser/entity/html.xml', 'xmlparser/entity/html.tags', 'expect-pass'),
	])
	test.summary()
