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

import sys
import os
import zipfile
import harness

test = harness.TestSuite('metadata')

def test_dir(path, name, prefix):
	rootdir = os.path.join(sys.path[0], path)
	testcases = sorted(os.listdir(rootdir))

	print 'testing metadata for %s files in directory %s:' % (name, path)
	for filename in testcases:
		if filename.endswith(prefix):
			testfile = os.path.join(rootdir, filename)
			if name == 'ePub':
				epubfile = '/tmp/test.epub'
				tmpfile = '/tmp/metadata.n3'
				n3file = os.path.join(rootdir, filename.replace(prefix, '.n3'))

				epubfiles=[
					('META-INF/container.xml', 'ocf/simple.ocf'),
					('OEBPS/content.opf', testfile),
					('OEBPS/toc.ncx', 'ncx/empty-toc-with-title.ncx'),
					('OEBPS/test.html', 'xhtml1/simple.xhtml')
				]

				zf = zipfile.ZipFile(epubfile, mode='w', compression=zipfile.ZIP_STORED)
				zf.writestr('mimetype', 'application/epub+zip')
				for location, filename in epubfiles:
					zf.write(os.path.join(sys.path[0], filename), location, compress_type=zipfile.ZIP_DEFLATED)
				zf.close()

				test.check_metadata(epubfile, n3file, 'turtle', displayas='%s in %s' % (testfile, epubfile))
			else:
				ntfile = os.path.join(rootdir, filename.replace(prefix, '.nt'))
				if os.path.exists(ntfile):
					test.check_metadata(testfile, ntfile, 'ntriple')

				n3file = os.path.join(rootdir, filename.replace(prefix, '.n3'))
				if os.path.exists(n3file):
					test.check_metadata(testfile, n3file, 'turtle')

def run_tests(name, tests=[]):
	print 'testing metadata for %s:' % name
	for doc, nt, expect in tests:
		test.check_metadata(os.path.join(sys.path[0], doc), os.path.join(sys.path[0], nt), 'ntriple', test_expect=expect)

if __name__ == '__main__':
	run_tests(name='RDF/XML -- syntax', tests=[
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

	test_dir(path='rdfxml/schema',   name='RDF/XML', prefix='.rdf')
	test_dir(path='xhtml1/metadata', name='XHTML',   prefix='.xhtml')
	test_dir(path='smil/metadata',   name='SMIL',    prefix='.smil')
	test_dir(path='opf/metadata',    name='OPF',     prefix='.opf')
	test_dir(path='opf/dc-metadata', name='OPF',     prefix='.opf')
	test_dir(path='opf/metadata',    name='ePub',    prefix='.opf')
	test_dir(path='opf/dc-metadata', name='ePub',    prefix='.opf')
	test_dir(path='epub/metadata',   name='ePub',    prefix='.opf')
	test.summary()
