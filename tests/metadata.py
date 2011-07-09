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
	test = harness.TestSuite('metadata', sys.argv)
	test.run({'name': 'RDF/XML', 'groups': [
		{'name': 'syntax', 'type': 'ntriple', 'tests': [
			{'test': 'rdfxml/syntax/example02.rdf', 'result': 'rdfxml/syntax/example02.nt', 'expect': 'fail'},
			{'test': 'rdfxml/syntax/example05.rdf', 'result': 'rdfxml/syntax/example05.nt'},
			{'test': 'rdfxml/syntax/example06.rdf', 'result': 'rdfxml/syntax/example06.nt'},
			{'test': 'rdfxml/syntax/example07.rdf', 'result': 'rdfxml/syntax/example07.nt'},
			{'test': 'rdfxml/syntax/example08.rdf', 'result': 'rdfxml/syntax/example08.nt'},
			{'test': 'rdfxml/syntax/example09.rdf', 'result': 'rdfxml/syntax/example09.nt', 'expect': 'fail'},
			{'test': 'rdfxml/syntax/example10.rdf', 'result': 'rdfxml/syntax/example10.nt'},
			{'test': 'rdfxml/syntax/example11.rdf', 'result': 'rdfxml/syntax/example11.nt'},
			{'test': 'rdfxml/syntax/example12.rdf', 'result': 'rdfxml/syntax/example12.nt'},
			{'test': 'rdfxml/syntax/example13.rdf', 'result': 'rdfxml/syntax/example13.nt', 'expect': 'fail'},
			{'test': 'rdfxml/syntax/example14.rdf', 'result': 'rdfxml/syntax/example14.nt'},
			{'test': 'rdfxml/syntax/example15.rdf', 'result': 'rdfxml/syntax/example15.nt'},
			{'test': 'rdfxml/syntax/example16.rdf', 'result': 'rdfxml/syntax/example16.nt'},
			{'test': 'rdfxml/syntax/example17.rdf', 'result': 'rdfxml/syntax/example17.nt'},
			{'test': 'rdfxml/syntax/example18.rdf', 'result': 'rdfxml/syntax/example18.nt'},
			{'test': 'rdfxml/syntax/example19.rdf', 'result': 'rdfxml/syntax/example19.nt'},
			{'test': 'rdfxml/syntax/example20.rdf', 'result': 'rdfxml/syntax/example20.nt', 'expect': 'fail'},
		]},
		{'name': 'schema', 'type': 'turtle', 'tests': [
			{'test': 'rdfxml/schema/dcam.rdf', 'result': 'rdfxml/schema/dcam.n3'},
			{'test': 'rdfxml/schema/dcelements.rdf', 'result': 'rdfxml/schema/dcelements.n3'},
			{'test': 'rdfxml/schema/dcterms.rdf', 'result': 'rdfxml/schema/dcterms.n3'},
			{'test': 'rdfxml/schema/owl.rdf', 'result': 'rdfxml/schema/owl.n3'},
			{'test': 'rdfxml/schema/rdf-schema.rdf', 'result': 'rdfxml/schema/rdf-schema.n3'},
			{'test': 'rdfxml/schema/rdf-syntax-ns.rdf', 'result': 'rdfxml/schema/rdf-syntax-ns.n3'},
			{'test': 'rdfxml/schema/skos.rdf', 'result': 'rdfxml/schema/skos.n3'},
		]},
	]})
	test.run({'name': 'HTML', 'replace': ['mimetype'], 'groups': [
		{'name': 'title metadata', 'type': 'turtle', 'tests': [
			{'test': 'html/metadata/title.html', 'result': 'html/metadata/title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/title.xhtml', 'result': 'html/metadata/title.n3', 'mimetype': 'application/xml'},
			{'test': 'html/metadata/empty-title.html', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/empty-title.xhtml', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'application/xml'},
		]},
		{'name': 'lang metadata', 'type': 'turtle', 'tests': [
			{'test': 'html/metadata/lang.html', 'result': 'html/metadata/lang.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/lang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xml'},
			{'test': 'html/metadata/xmllang.xhtml', 'result': 'html/metadata/xmllang.n3', 'mimetype': 'application/xml'},
			{'test': 'html/metadata/lang-and-xmllang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xml'},
		]},
	]})
	test.run({'name': 'NCX', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'ncx/metadata/meta.ncx', 'result': 'ncx/metadata/meta.n3'},
			{'test': 'ncx/metadata/author.ncx', 'result': 'ncx/metadata/author.n3'},
			{'test': 'ncx/metadata/title.ncx', 'result': 'ncx/metadata/title.n3'},
		]},
	]})
	test.run({'name': 'MIME', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'mime/metadata/subject.txt', 'result': 'mime/metadata/subject.n3'},
			{'test': 'mime/metadata/subject-lfonly.txt', 'result': 'mime/metadata/subject.n3'},
			{'test': 'mime/metadata/from.txt', 'result': 'mime/metadata/from.n3'},
			{'test': 'mime/metadata/from-lfonly.txt', 'result': 'mime/metadata/from.n3'},
		]},
	]})
	test.run({'name': 'SMIL', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'smil/metadata/embedded-rdf.smil', 'result': 'smil/metadata/embedded-rdf.n3'},
			{'test': 'smil/metadata/xmllang.smil', 'result': 'smil/metadata/xmllang.n3'},
		]},
	]})
	test.run({'name': 'RTF', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'rtf/info/author.rtf', 'result': 'rtf/info/author.n3'},
			{'test': 'rtf/info/comment.rtf', 'result': 'rtf/info/comment.n3'},
			{'test': 'rtf/info/keywords.rtf', 'result': 'rtf/info/keywords.n3'},
			{'test': 'rtf/info/operator.rtf', 'result': 'rtf/info/operator.n3'},
			{'test': 'rtf/info/subject.rtf', 'result': 'rtf/info/subject.n3'},
			{'test': 'rtf/info/title.rtf', 'result': 'rtf/info/title.n3'},
		]},
	]})
	test.run({'name': 'OPF', 'replace': ['mimetype'], 'mimetype': 'application/xml', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'opf/metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
			{'test': 'opf/metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
			{'test': 'opf/metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
			{'test': 'opf/metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
			{'test': 'opf/metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
			{'test': 'opf/metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
			{'test': 'opf/metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
			{'test': 'opf/metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
			{'test': 'opf/metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
			{'test': 'opf/metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
			{'test': 'opf/metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
			{'test': 'opf/metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
			{'test': 'opf/metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
			{'test': 'opf/metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
			{'test': 'opf/metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
			{'test': 'opf/metadata/whitespace.opf', 'result': 'opf/metadata/whitespace.n3'},
			{'test': 'opf/metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
		]},
		{'name': 'dc-metadata', 'type': 'turtle', 'tests': [
			{'test': 'opf/dc-metadata/dublincore-contributor.opf', 'result': 'opf/metadata/dublincore-contributor.n3'},
			{'test': 'opf/dc-metadata/dublincore-coverage.opf', 'result': 'opf/metadata/dublincore-coverage.n3'},
			{'test': 'opf/dc-metadata/dublincore-creator.opf', 'result': 'opf/metadata/dublincore-creator.n3'},
			{'test': 'opf/dc-metadata/dublincore-date.opf', 'result': 'opf/metadata/dublincore-date.n3'},
			{'test': 'opf/dc-metadata/dublincore-description.opf', 'result': 'opf/metadata/dublincore-description.n3'},
			{'test': 'opf/dc-metadata/dublincore-format.opf', 'result': 'opf/metadata/dublincore-format.n3'},
			{'test': 'opf/dc-metadata/dublincore-identifier.opf', 'result': 'opf/metadata/dublincore-identifier.n3'},
			{'test': 'opf/dc-metadata/dublincore-language.opf', 'result': 'opf/metadata/dublincore-language.n3'},
			{'test': 'opf/dc-metadata/dublincore-publisher.opf', 'result': 'opf/metadata/dublincore-publisher.n3'},
			{'test': 'opf/dc-metadata/dublincore-relation.opf', 'result': 'opf/metadata/dublincore-relation.n3'},
			{'test': 'opf/dc-metadata/dublincore-rights.opf', 'result': 'opf/metadata/dublincore-rights.n3'},
			{'test': 'opf/dc-metadata/dublincore-source.opf', 'result': 'opf/metadata/dublincore-source.n3'},
			{'test': 'opf/dc-metadata/dublincore-subject.opf', 'result': 'opf/metadata/dublincore-subject.n3'},
			{'test': 'opf/dc-metadata/dublincore-title.opf', 'result': 'opf/metadata/dublincore-title.n3'},
			{'test': 'opf/dc-metadata/dublincore-type.opf', 'result': 'opf/metadata/dublincore-type.n3'},
			{'test': 'opf/dc-metadata/xmllang.opf', 'result': 'opf/metadata/xmllang.n3'},
		]},
		{'name': 'meta', 'type': 'turtle', 'tests': [
			{'test': 'opf/meta/name-content.opf', 'result': 'opf/meta/name-content.n3'},
			{'test': 'opf/meta/property.opf', 'result': 'opf/meta/property.n3'},
		]},
	]})
	test.run({ 'name': 'ePub', 'replace': ['mimetype'], 'mimetype': 'application/epub+zip',
		'archive':
			[
				('mimetype', 'application/epub+zip'),
				('META-INF/container.xml', 'ocf/simple.ocf'),
				('OEBPS/content.opf', '@test'), # replaced with 'test' file in the group tests
				('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
				('OEBPS/test.html', 'html/semantics/simple.xhtml')
			],
		'groups': [
			{'name': 'metadata', 'type': 'turtle', 'tests': [
				{'test': 'opf/metadata/dublincore-contributor.opf', 'result': 'epub/metadata/dublincore-contributor.n3'},
				{'test': 'opf/metadata/dublincore-coverage.opf', 'result': 'epub/metadata/dublincore-coverage.n3'},
				{'test': 'opf/metadata/dublincore-creator.opf', 'result': 'epub/metadata/dublincore-creator.n3'},
				{'test': 'opf/metadata/dublincore-date.opf', 'result': 'epub/metadata/dublincore-date.n3'},
				{'test': 'opf/metadata/dublincore-description.opf', 'result': 'epub/metadata/dublincore-description.n3'},
				{'test': 'opf/metadata/dublincore-format.opf', 'result': 'epub/metadata/dublincore-format.n3'},
				{'test': 'opf/metadata/dublincore-identifier.opf', 'result': 'epub/metadata/dublincore-identifier.n3'},
				{'test': 'opf/metadata/dublincore-language.opf', 'result': 'epub/metadata/dublincore-language.n3'},
				{'test': 'opf/metadata/dublincore-publisher.opf', 'result': 'epub/metadata/dublincore-publisher.n3'},
				{'test': 'opf/metadata/dublincore-relation.opf', 'result': 'epub/metadata/dublincore-relation.n3'},
				{'test': 'opf/metadata/dublincore-rights.opf', 'result': 'epub/metadata/dublincore-rights.n3'},
				{'test': 'opf/metadata/dublincore-source.opf', 'result': 'epub/metadata/dublincore-source.n3'},
				{'test': 'opf/metadata/dublincore-subject.opf', 'result': 'epub/metadata/dublincore-subject.n3'},
				{'test': 'opf/metadata/dublincore-title.opf', 'result': 'epub/metadata/dublincore-title.n3'},
				{'test': 'opf/metadata/dublincore-type.opf', 'result': 'epub/metadata/dublincore-type.n3'},
				{'test': 'opf/metadata/whitespace.opf', 'result': 'epub/metadata/whitespace.n3'},
				{'test': 'opf/metadata/xmllang.opf', 'result': 'epub/metadata/xmllang.n3'},
			]},
			{'name': 'dc-metadata', 'type': 'turtle', 'tests': [
				{'test': 'opf/dc-metadata/dublincore-contributor.opf', 'result': 'epub/metadata/dublincore-contributor.n3'},
				{'test': 'opf/dc-metadata/dublincore-coverage.opf', 'result': 'epub/metadata/dublincore-coverage.n3'},
				{'test': 'opf/dc-metadata/dublincore-creator.opf', 'result': 'epub/metadata/dublincore-creator.n3'},
				{'test': 'opf/dc-metadata/dublincore-date.opf', 'result': 'epub/metadata/dublincore-date.n3'},
				{'test': 'opf/dc-metadata/dublincore-description.opf', 'result': 'epub/metadata/dublincore-description.n3'},
				{'test': 'opf/dc-metadata/dublincore-format.opf', 'result': 'epub/metadata/dublincore-format.n3'},
				{'test': 'opf/dc-metadata/dublincore-identifier.opf', 'result': 'epub/metadata/dublincore-identifier.n3'},
				{'test': 'opf/dc-metadata/dublincore-language.opf', 'result': 'epub/metadata/dublincore-language.n3'},
				{'test': 'opf/dc-metadata/dublincore-publisher.opf', 'result': 'epub/metadata/dublincore-publisher.n3'},
				{'test': 'opf/dc-metadata/dublincore-relation.opf', 'result': 'epub/metadata/dublincore-relation.n3'},
				{'test': 'opf/dc-metadata/dublincore-rights.opf', 'result': 'epub/metadata/dublincore-rights.n3'},
				{'test': 'opf/dc-metadata/dublincore-source.opf', 'result': 'epub/metadata/dublincore-source.n3'},
				{'test': 'opf/dc-metadata/dublincore-subject.opf', 'result': 'epub/metadata/dublincore-subject.n3'},
				{'test': 'opf/dc-metadata/dublincore-title.opf', 'result': 'epub/metadata/dublincore-title.n3'},
				{'test': 'opf/dc-metadata/dublincore-type.opf', 'result': 'epub/metadata/dublincore-type.n3'},
				{'test': 'opf/dc-metadata/xmllang.opf', 'result': 'epub/metadata/xmllang.n3'},
			]},
			{'name': 'with-content', 'type': 'turtle', 'tests': [
				{'test': 'epub/metadata/htmlfile-in-spine.opf', 'result': 'epub/metadata/htmlfile-in-spine.n3'},
			]},
		]
	})
	test.summary()
