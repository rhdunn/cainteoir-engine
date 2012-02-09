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
		{'name': 'metadata', 'type': 'ntriple', 'tests': [
			{'test': 'rdfxml/metadata/empty.rdf', 'result': 'rdfxml/metadata/empty.nt'},
			{'test': 'rdfxml/metadata/description-empty.rdf', 'result': 'rdfxml/metadata/description-empty.nt'},
			{'test': 'rdfxml/metadata/about-datatype.rdf', 'result': 'rdfxml/metadata/about-datatype.nt'},
			{'test': 'rdfxml/metadata/about-predicate_attribute-last_attribute.rdf',  'result': 'rdfxml/metadata/about-predicate_attribute.nt'},
			{'test': 'rdfxml/metadata/about-predicate_attribute-first_attribute.rdf', 'result': 'rdfxml/metadata/about-predicate_attribute.nt'},
		]},
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
		]},
		{'name': 'schema', 'type': 'turtle', 'tests': [
			{'test': '../src/schema/dcam.rdf', 'result': 'rdfxml/schema/dcam.n3'},
			{'test': '../src/schema/dcelements.rdf', 'result': 'rdfxml/schema/dcelements.n3'},
			{'test': '../src/schema/dcterms.rdf', 'result': 'rdfxml/schema/dcterms.n3'},
			{'test': '../src/schema/owl.rdf', 'result': 'rdfxml/schema/owl.n3'},
			{'test': '../src/schema/rdf-schema.rdf', 'result': 'rdfxml/schema/rdf-schema.n3'},
			{'test': '../src/schema/rdf-syntax-ns.rdf', 'result': 'rdfxml/schema/rdf-syntax-ns.n3'},
			{'test': '../src/schema/skos.rdf', 'result': 'rdfxml/schema/skos.n3'},
		]},
	]})
	test.run({'name': 'HTML', 'replace': ['mimetype'], 'groups': [
		{'name': 'title metadata', 'type': 'turtle', 'tests': [
			{'test': 'html/metadata/title.html', 'result': 'html/metadata/title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/title.xhtml', 'result': 'html/metadata/title.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/title-cdata.xhtml', 'result': 'html/metadata/title.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/empty-title.html', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/empty-title.xhtml', 'result': 'html/metadata/empty-title.n3', 'mimetype': 'application/xhtml+xml'},
		]},
		{'name': 'lang metadata', 'type': 'turtle', 'tests': [
			{'test': 'html/metadata/lang.html', 'result': 'html/metadata/lang.n3', 'mimetype': 'text/html'},
			{'test': 'html/metadata/lang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/xmllang.xhtml', 'result': 'html/metadata/xmllang.n3', 'mimetype': 'application/xhtml+xml'},
			{'test': 'html/metadata/lang-and-xmllang.xhtml', 'result': 'html/metadata/lang.n3', 'mimetype': 'application/xhtml+xml'},
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
			{'test': 'smil/metadata/embedded-rdf.smil', 'result': 'smil/metadata/embedded-rdf.n3', 'expect': 'fail'},
			{'test': 'smil/metadata/xmllang.smil', 'result': 'smil/metadata/xmllang.n3', 'expect': 'fail'},
		]},
	]})
	test.run({'name': 'SSML', 'groups': [
		{'name': 'metadata', 'type': 'turtle', 'tests': [
			{'test': 'ssml/metadata/xmllang.ssml', 'result': 'ssml/metadata/xmllang.n3'},
			{'test': 'ssml/metadata/meta-see-also.ssml', 'result': 'ssml/metadata/meta-see-also.n3'},
			{'test': 'ssml/metadata/meta-http-equiv.ssml', 'result': 'ssml/metadata/meta-http-equiv.n3'},
			{'test': 'ssml/metadata/meta-other.ssml', 'result': 'ssml/metadata/meta-other.n3'},
			{'test': 'ssml/metadata/metadata-rdfxml.ssml', 'result': 'ssml/metadata/metadata-rdfxml.n3', 'expect': 'fail'},
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
	test.run({'name': 'OPF', 'replace': ['mimetype'], 'mimetype': 'application/oebps-package+xml', 'groups': [
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
			# metadata patterns
			{'test': 'opf/meta/name-content.opf', 'result': 'opf/meta/name-content.n3'},
			{'test': 'opf/meta/property.opf', 'result': 'opf/meta/property.n3'},
			{'test': 'opf/meta/property-id.opf', 'result': 'opf/meta/property-id.n3'},
			{'test': 'opf/meta/property-about.opf', 'result': 'opf/meta/property-about.n3'},
			{'test': 'opf/meta/property-refines.opf', 'result': 'opf/meta/property-about.n3'},
			{'test': 'opf/meta/property-datatype.opf', 'result': 'opf/meta/property-datatype.n3'},
			{'test': 'opf/meta/property-scheme.opf', 'result': 'opf/meta/property-datatype.n3'},
			# @prefix
			{'test': 'opf/meta/package-prefix.opf', 'result': 'opf/meta/prefix.n3'},
			{'test': 'opf/meta/metadata-prefix.opf', 'result': 'opf/meta/prefix.n3'},
		]},
		{'name': 'link', 'type': 'turtle', 'tests': [
			{'test': 'opf/link/rel.opf', 'result': 'opf/link/rel.n3'},
			{'test': 'opf/link/rel-multicurie.opf', 'result': 'opf/link/rel-multicurie.n3'},
			{'test': 'opf/link/rel-id.opf', 'result': 'opf/link/rel-id.n3'},
			{'test': 'opf/link/rel-about.opf', 'result': 'opf/link/rel-about.n3'},
			{'test': 'opf/link/rel-refines.opf', 'result': 'opf/link/rel-about.n3'},
			{'test': 'opf/link/rel-mediatype.opf', 'result': 'opf/link/rel-mediatype.n3'},
		]},
		{'name': 'metadata 3.0', 'type': 'turtle', 'tests': [
			{'test': 'opf/metadata30/contributor-role.opf', 'result': 'opf/metadata30/contributor-role.n3'},
			{'test': 'opf/metadata30/contributor-file-as.opf', 'result': 'opf/metadata30/contributor-file-as.n3'},
			{'test': 'opf/metadata30/contributor-alternate-script.opf', 'result': 'opf/metadata30/contributor-alternate-script.n3'},
			{'test': 'opf/metadata30/creator-role.opf', 'result': 'opf/metadata30/creator-role.n3'},
			{'test': 'opf/metadata30/creator-file-as.opf', 'result': 'opf/metadata30/creator-file-as.n3'},
			{'test': 'opf/metadata30/creator-alternate-script.opf', 'result': 'opf/metadata30/creator-alternate-script.n3'},
			{'test': 'opf/metadata30/identifier.opf', 'result': 'opf/metadata30/identifier.n3'},
			{'test': 'opf/metadata30/meta-auth.opf', 'result': 'opf/metadata30/meta-auth.n3'},
			{'test': 'opf/metadata30/publication-type.opf', 'result': 'opf/metadata30/publication-type.n3'},
			{'test': 'opf/metadata30/source-identifier.opf', 'result': 'opf/metadata30/source-identifier.n3'},
			{'test': 'opf/metadata30/title-title-type.opf', 'result': 'opf/metadata30/title-title-type.n3'},
			{'test': 'opf/metadata30/title-display-seq.opf', 'result': 'opf/metadata30/title-display-seq.n3'},
			{'test': 'opf/metadata30/title-file-as.opf', 'result': 'opf/metadata30/title-file-as.n3'},
			{'test': 'opf/metadata30/title-group-position.opf', 'result': 'opf/metadata30/title-group-position.n3'},
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
