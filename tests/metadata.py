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
	test = harness.TestSuite('metadata')
	test.run_tests(name='RDF/XML -- syntax', testtype='ntriple', tests=[
		('rdfxml/syntax/example02.rdf', 'rdfxml/syntax/example02.nt', 'expect-fail'),
		('rdfxml/syntax/example05.rdf', 'rdfxml/syntax/example05.nt', 'expect-pass'),
		('rdfxml/syntax/example06.rdf', 'rdfxml/syntax/example06.nt', 'expect-pass'),
		('rdfxml/syntax/example07.rdf', 'rdfxml/syntax/example07.nt', 'expect-pass'),
		('rdfxml/syntax/example08.rdf', 'rdfxml/syntax/example08.nt', 'expect-fail'),
		('rdfxml/syntax/example09.rdf', 'rdfxml/syntax/example09.nt', 'expect-fail'),
		('rdfxml/syntax/example10.rdf', 'rdfxml/syntax/example10.nt', 'expect-pass'),
		('rdfxml/syntax/example11.rdf', 'rdfxml/syntax/example11.nt', 'expect-pass'),
		('rdfxml/syntax/example12.rdf', 'rdfxml/syntax/example12.nt', 'expect-pass'),
		('rdfxml/syntax/example13.rdf', 'rdfxml/syntax/example13.nt', 'expect-fail'),
		('rdfxml/syntax/example14.rdf', 'rdfxml/syntax/example14.nt', 'expect-pass'),
		('rdfxml/syntax/example15.rdf', 'rdfxml/syntax/example15.nt', 'expect-pass'),
		('rdfxml/syntax/example16.rdf', 'rdfxml/syntax/example16.nt', 'expect-pass'),
		('rdfxml/syntax/example17.rdf', 'rdfxml/syntax/example17.nt', 'expect-pass'),
		('rdfxml/syntax/example18.rdf', 'rdfxml/syntax/example18.nt', 'expect-pass'),
		('rdfxml/syntax/example19.rdf', 'rdfxml/syntax/example19.nt', 'expect-pass'),
		('rdfxml/syntax/example20.rdf', 'rdfxml/syntax/example20.nt', 'expect-fail'),
	])
	test.run_tests(name='RDF/XML -- schemas', testtype='turtle', tests=[
		('rdfxml/schema/dcam.rdf', 'rdfxml/schema/dcam.n3', 'expect-pass'),
		('rdfxml/schema/dcelements.rdf', 'rdfxml/schema/dcelements.n3', 'expect-pass'),
		('rdfxml/schema/dcterms.rdf', 'rdfxml/schema/dcterms.n3', 'expect-pass'),
		('rdfxml/schema/owl.rdf', 'rdfxml/schema/owl.n3', 'expect-pass'),
		('rdfxml/schema/rdf-schema.rdf', 'rdfxml/schema/rdf-schema.n3', 'expect-pass'),
		('rdfxml/schema/rdf-syntax-ns.rdf', 'rdfxml/schema/rdf-syntax-ns.n3', 'expect-pass'),
		('rdfxml/schema/skos.rdf', 'rdfxml/schema/skos.n3', 'expect-pass'),
	])
	test.run_tests(name='HTML -- metadata (title)', testtype='turtle', tests=[
		('html/metadata/title.html',  'html/metadata/title.n3', 'expect-fail'),
		('html/metadata/title.xhtml', 'html/metadata/title.n3', 'expect-fail'),
		('html/metadata/empty-title.html',  'html/metadata/empty-title.n3', 'expect-pass'),
		('html/metadata/empty-title.xhtml', 'html/metadata/empty-title.n3', 'expect-pass'),
	])
	test.run_tests(name='HTML -- metadata (lang)', testtype='turtle', tests=[
		('html/metadata/lang.html',  'html/metadata/lang.n3', 'expect-fail'),
		('html/metadata/lang.xhtml', 'html/metadata/lang.n3', 'expect-fail'),
		('html/metadata/xmllang.xhtml', 'html/metadata/xmllang.n3', 'expect-fail'),
	])
	test.run_tests(name='SMIL -- metadata', testtype='turtle', tests=[
		('smil/metadata/embedded-rdf.smil', 'smil/metadata/embedded-rdf.n3', 'expect-pass'),
		('smil/metadata/xmllang.smil', 'smil/metadata/xmllang.n3', 'expect-pass'),
	])
	test.run_tests(name='OPF -- metadata', testtype='turtle', tests=[
		('opf/metadata/dublincore-contributor.opf', 'opf/metadata/dublincore-contributor.n3', 'expect-pass'),
		('opf/metadata/dublincore-coverage.opf', 'opf/metadata/dublincore-coverage.n3', 'expect-pass'),
		('opf/metadata/dublincore-creator.opf', 'opf/metadata/dublincore-creator.n3', 'expect-pass'),
		('opf/metadata/dublincore-date.opf', 'opf/metadata/dublincore-date.n3', 'expect-pass'),
		('opf/metadata/dublincore-description.opf', 'opf/metadata/dublincore-description.n3', 'expect-pass'),
		('opf/metadata/dublincore-format.opf', 'opf/metadata/dublincore-format.n3', 'expect-pass'),
		('opf/metadata/dublincore-identifier.opf', 'opf/metadata/dublincore-identifier.n3', 'expect-pass'),
		('opf/metadata/dublincore-language.opf', 'opf/metadata/dublincore-language.n3', 'expect-pass'),
		('opf/metadata/dublincore-publisher.opf', 'opf/metadata/dublincore-publisher.n3', 'expect-pass'),
		('opf/metadata/dublincore-relation.opf', 'opf/metadata/dublincore-relation.n3', 'expect-pass'),
		('opf/metadata/dublincore-rights.opf', 'opf/metadata/dublincore-rights.n3', 'expect-pass'),
		('opf/metadata/dublincore-source.opf', 'opf/metadata/dublincore-source.n3', 'expect-pass'),
		('opf/metadata/dublincore-subject.opf', 'opf/metadata/dublincore-subject.n3', 'expect-pass'),
		('opf/metadata/dublincore-title.opf', 'opf/metadata/dublincore-title.n3', 'expect-pass'),
		('opf/metadata/dublincore-type.opf', 'opf/metadata/dublincore-type.n3', 'expect-pass'),
		('opf/metadata/whitespace.opf', 'opf/metadata/whitespace.n3', 'expect-pass'),
		('opf/metadata/xmllang.opf', 'opf/metadata/xmllang.n3', 'expect-pass'),
	])
	test.run_tests(name='OPF -- metadata', testtype='turtle', tests=[
		('opf/dc-metadata/dublincore-contributor.opf', 'opf/dc-metadata/dublincore-contributor.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-coverage.opf', 'opf/dc-metadata/dublincore-coverage.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-creator.opf', 'opf/dc-metadata/dublincore-creator.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-date.opf', 'opf/dc-metadata/dublincore-date.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-description.opf', 'opf/dc-metadata/dublincore-description.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-format.opf', 'opf/dc-metadata/dublincore-format.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-identifier.opf', 'opf/dc-metadata/dublincore-identifier.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-language.opf', 'opf/dc-metadata/dublincore-language.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-publisher.opf', 'opf/dc-metadata/dublincore-publisher.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-relation.opf', 'opf/dc-metadata/dublincore-relation.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-rights.opf', 'opf/dc-metadata/dublincore-rights.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-source.opf', 'opf/dc-metadata/dublincore-source.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-subject.opf', 'opf/dc-metadata/dublincore-subject.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-title.opf', 'opf/dc-metadata/dublincore-title.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-type.opf', 'opf/dc-metadata/dublincore-type.n3', 'expect-pass'),
		('opf/dc-metadata/xmllang.opf', 'opf/dc-metadata/xmllang.n3', 'expect-pass'),
	])
	test.run_epub_tests(name='OPF -- metadata', testtype='turtle', tests=[
		('opf/metadata/dublincore-contributor.opf', 'opf/metadata/dublincore-contributor.n3', 'expect-pass'),
		('opf/metadata/dublincore-coverage.opf', 'opf/metadata/dublincore-coverage.n3', 'expect-pass'),
		('opf/metadata/dublincore-creator.opf', 'opf/metadata/dublincore-creator.n3', 'expect-pass'),
		('opf/metadata/dublincore-date.opf', 'opf/metadata/dublincore-date.n3', 'expect-pass'),
		('opf/metadata/dublincore-description.opf', 'opf/metadata/dublincore-description.n3', 'expect-pass'),
		('opf/metadata/dublincore-format.opf', 'opf/metadata/dublincore-format.n3', 'expect-pass'),
		('opf/metadata/dublincore-identifier.opf', 'opf/metadata/dublincore-identifier.n3', 'expect-pass'),
		('opf/metadata/dublincore-language.opf', 'opf/metadata/dublincore-language.n3', 'expect-pass'),
		('opf/metadata/dublincore-publisher.opf', 'opf/metadata/dublincore-publisher.n3', 'expect-pass'),
		('opf/metadata/dublincore-relation.opf', 'opf/metadata/dublincore-relation.n3', 'expect-pass'),
		('opf/metadata/dublincore-rights.opf', 'opf/metadata/dublincore-rights.n3', 'expect-pass'),
		('opf/metadata/dublincore-source.opf', 'opf/metadata/dublincore-source.n3', 'expect-pass'),
		('opf/metadata/dublincore-subject.opf', 'opf/metadata/dublincore-subject.n3', 'expect-pass'),
		('opf/metadata/dublincore-title.opf', 'opf/metadata/dublincore-title.n3', 'expect-pass'),
		('opf/metadata/dublincore-type.opf', 'opf/metadata/dublincore-type.n3', 'expect-pass'),
		('opf/metadata/whitespace.opf', 'opf/metadata/whitespace.n3', 'expect-pass'),
		('opf/metadata/xmllang.opf', 'opf/metadata/xmllang.n3', 'expect-pass'),
	])
	test.run_epub_tests(name='OPF -- metadata', testtype='turtle', tests=[
		('opf/dc-metadata/dublincore-contributor.opf', 'opf/dc-metadata/dublincore-contributor.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-coverage.opf', 'opf/dc-metadata/dublincore-coverage.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-creator.opf', 'opf/dc-metadata/dublincore-creator.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-date.opf', 'opf/dc-metadata/dublincore-date.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-description.opf', 'opf/dc-metadata/dublincore-description.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-format.opf', 'opf/dc-metadata/dublincore-format.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-identifier.opf', 'opf/dc-metadata/dublincore-identifier.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-language.opf', 'opf/dc-metadata/dublincore-language.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-publisher.opf', 'opf/dc-metadata/dublincore-publisher.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-relation.opf', 'opf/dc-metadata/dublincore-relation.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-rights.opf', 'opf/dc-metadata/dublincore-rights.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-source.opf', 'opf/dc-metadata/dublincore-source.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-subject.opf', 'opf/dc-metadata/dublincore-subject.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-title.opf', 'opf/dc-metadata/dublincore-title.n3', 'expect-pass'),
		('opf/dc-metadata/dublincore-type.opf', 'opf/dc-metadata/dublincore-type.n3', 'expect-pass'),
		('opf/dc-metadata/xmllang.opf', 'opf/dc-metadata/xmllang.n3', 'expect-pass'),
	])
	test.run_epub_tests(name='ePub -- metadata', testtype='turtle', tests=[
		('epub/metadata/htmlfile-in-spine.opf', 'epub/metadata/htmlfile-in-spine.n3', 'expect-pass'),
	])
	test.summary()
