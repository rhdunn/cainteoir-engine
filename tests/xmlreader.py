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
	test = harness.TestSuite('XMLReader', sys.argv)
	test.run({'name': 'XMLReader', 'groups': [
		{'name': 'text', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/text/basic.xml', 'result': 'xmlparser/text/basic.tags'},
			{'test': 'xmlparser/text/multiline.xml', 'result': 'xmlparser/text/multiline.tags'},
			{'test': 'xmlparser/text/with-lessthan.xml', 'result': 'xmlparser/text/with-lessthan.tags'},
			{'test': 'xmlparser/text/in-tags.xml', 'result': 'xmlparser/text/in-tags.tags'},
		]},
		{'name': 'comment', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/comments/comment.xml', 'result': 'xmlparser/comments/comment.tags'},
			{'test': 'xmlparser/comments/comment-in-a-tag.xml', 'result': 'xmlparser/comments/comment-in-a-tag.tags'},
			{'test': 'xmlparser/comments/comment-with-dash.xml', 'result': 'xmlparser/comments/comment-with-dash.tags'},
		]},
		{'name': 'CDATA section', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/cdata-section/cdata-section.xml', 'result': 'xmlparser/cdata-section/cdata-section.tags'},
			{'test': 'xmlparser/cdata-section/cdata-section-in-a-tag.xml', 'result': 'xmlparser/cdata-section/cdata-section-in-a-tag.tags'},
			{'test': 'xmlparser/cdata-section/cdata-section-with-amp.xml', 'result': 'xmlparser/cdata-section/cdata-section-with-amp.tags'},
			{'test': 'xmlparser/cdata-section/cdata-section-with-tags.xml', 'result': 'xmlparser/cdata-section/cdata-section-with-tags.tags'},
		]},
		{'name': 'element', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/element/no-args.xml', 'result': 'xmlparser/element/no-args.tags'},
			{'test': 'xmlparser/element/args.xml', 'result': 'xmlparser/element/args.tags'},
			{'test': 'xmlparser/element/args-with-dash.xml', 'result': 'xmlparser/element/args-with-dash.tags'},
			{'test': 'xmlparser/element/args-singlequote.xml', 'result': 'xmlparser/element/args.tags'},
			{'test': 'xmlparser/element/args-noquote.xml', 'result': 'xmlparser/element/args.tags'},
			{'test': 'xmlparser/element/args-novalue.xml', 'result': 'xmlparser/element/args-novalue.tags'},
			{'test': 'xmlparser/element/space-before.xml', 'result': 'xmlparser/element/space-before.tags'},
			{'test': 'xmlparser/element/partial.xml', 'result': 'xmlparser/element/partial.tags'},
			# namespaces:
			{'test': 'xmlparser/element/args-xmllang.xml', 'result': 'xmlparser/element/args-xmllang.tags'},
			{'test': 'xmlparser/element/namespace.xml', 'result': 'xmlparser/element/namespace.tags'},
			{'test': 'xmlparser/element/namespace-default.xml', 'result': 'xmlparser/element/namespace-default.tags'},
			{'test': 'xmlparser/element/namespace-nested.xml', 'result': 'xmlparser/element/namespace-nested.tags'},
			{'test': 'xmlparser/element/namespace-scoped.xml', 'result': 'xmlparser/element/namespace-scoped.tags'},
		]},
		{'name': 'empty element', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/empty-element/no-args.xml', 'result': 'xmlparser/empty-element/no-args.tags'},
			{'test': 'xmlparser/empty-element/no-args-with-space.xml', 'result': 'xmlparser/empty-element/no-args.tags'},
		]},
		{'name': 'doctype', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/doctype/html40.xml', 'result': 'xmlparser/doctype/html40.tags'},
			{'test': 'xmlparser/doctype/html40-with-dtd.xml', 'result': 'xmlparser/doctype/html40-with-dtd.tags'},
			{'test': 'xmlparser/doctype/html401.xml', 'result': 'xmlparser/doctype/html401.tags'},
			{'test': 'xmlparser/doctype/html401-with-dtd.xml', 'result': 'xmlparser/doctype/html401-with-dtd.tags'},
			{'test': 'xmlparser/doctype/xhtml10-strict.xml', 'result': 'xmlparser/doctype/xhtml10-strict.tags'},
			{'test': 'xmlparser/doctype/xhtml11.xml', 'result': 'xmlparser/doctype/xhtml11.tags'},
			{'test': 'xmlparser/doctype/html5-lower.xml', 'result': 'xmlparser/doctype/html5-lower.tags'},
			{'test': 'xmlparser/doctype/html5-upper.xml', 'result': 'xmlparser/doctype/html5-upper.tags'},
			{'test': 'xmlparser/doctype/html5-compat-double-quote.xml', 'result': 'xmlparser/doctype/html5-compat-double-quote.tags'},
			{'test': 'xmlparser/doctype/html5-compat-single-quote.xml', 'result': 'xmlparser/doctype/html5-compat-single-quote.tags'},
			{'test': 'xmlparser/doctype/namespace.xml', 'result': 'xmlparser/doctype/namespace.tags'},
			{'test': 'xmlparser/doctype/with-dtd-content.xml', 'result': 'xmlparser/doctype/with-dtd-content.tags'},
		]},
		{'name': 'entity', 'type': 'xmlreader', 'tests': [
			{'test': 'xmlparser/entity/1byte-utf8-hex.xml', 'result': 'xmlparser/entity/1byte-utf8-hex.tags'},
			{'test': 'xmlparser/entity/2byte-utf8-hex.xml', 'result': 'xmlparser/entity/2byte-utf8-hex.tags'},
			{'test': 'xmlparser/entity/3byte-utf8-hex.xml', 'result': 'xmlparser/entity/3byte-utf8-hex.tags'},
			{'test': 'xmlparser/entity/4byte-utf8-hex.xml', 'result': 'xmlparser/entity/4byte-utf8-hex.tags'},
			{'test': 'xmlparser/entity/1byte-utf8-int.xml', 'result': 'xmlparser/entity/1byte-utf8-int.tags'},
			{'test': 'xmlparser/entity/2byte-utf8-int.xml', 'result': 'xmlparser/entity/2byte-utf8-int.tags'},
			{'test': 'xmlparser/entity/3byte-utf8-int.xml', 'result': 'xmlparser/entity/3byte-utf8-int.tags'},
			{'test': 'xmlparser/entity/4byte-utf8-int.xml', 'result': 'xmlparser/entity/4byte-utf8-int.tags'},
			{'test': 'xmlparser/entity/invalid-char.xml', 'result': 'xmlparser/entity/invalid-char.tags'},
			{'test': 'xmlparser/entity/unknown.xml', 'result': 'xmlparser/entity/unknown.tags'},
			{'test': 'xmlparser/entity/xml.xml', 'result': 'xmlparser/entity/xml.tags'},
			{'test': 'xmlparser/entity/in-attr.xml', 'result': 'xmlparser/entity/in-attr.tags'},
			{'test': 'xmlparser/entity/dtd-entity.xml', 'result': 'xmlparser/entity/dtd-entity.tags'},
			{'test': 'xmlparser/entity/dtd-entity-in-attr.xml', 'result': 'xmlparser/entity/dtd-entity-in-attr.tags'},
			{'test': 'xmlparser/entity/dtd-entity-in-xmlns.xml', 'result': 'xmlparser/entity/dtd-entity-in-xmlns.tags'},
		]},
	]})
	test.summary()
