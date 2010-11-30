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

sys.path.append(os.path.join(sys.path[0], '../src/cainteoir-engine'))

import metadata
import harness as test

def check_resource(res, value):
	test.equal(res.ref, value.ref, 'res.ref')
	test.equal(res.base, value.base, 'res.base')
	test.equal(res.uri, value.uri, 'res.uri')
	test.equal(str(res), str(value), 'str(res)')

def check_literal(literal, value):
	test.equal(literal.text, value.text, 'res.text')
	test.equal(literal.language, value.language, 'res.language')
	test.equal(literal.type, value.type, 'res.type')

def check_triple_with_resource():
	prefixes = {'http://www.w3.org/2001/XMLSchema#': 'xsd', 'http://purl.org/dc/elements/1.1/': 'dc'}

	subject   = metadata.RDFResource('http://example.com/test.html')
	predicate = metadata.RDFResource('http://purl.org/dc/elements/1.1/creator')
	object    = metadata.RDFResource('http://example.com/users/jane-bloggs')

	triple = metadata.RDFTriple(subject, predicate, object)
	check_resource(triple.subject, subject)
	check_resource(triple.predicate, predicate)
	check_resource(triple.object, object)
	test.equal(triple.format(), '<http://example.com/test.html> <http://purl.org/dc/elements/1.1/creator> <http://example.com/users/jane-bloggs> .', 'triple.format()')
	test.equal(triple.format(prefixes), '<http://example.com/test.html> dc:creator <http://example.com/users/jane-bloggs> .', 'triple.format()')

def check_triple_with_literal():
	prefixes = {'http://www.w3.org/2001/XMLSchema#': 'xsd', 'http://purl.org/dc/elements/1.1/': 'dc'}

	subject   = metadata.RDFResource('http://example.com/test.html')
	predicate = metadata.RDFResource('http://purl.org/dc/elements/1.1/title')
	object    = metadata.RDFLiteral('Test page')

	triple = metadata.RDFTriple(subject, predicate, object)
	check_resource(triple.subject, subject)
	check_resource(triple.predicate, predicate)
	check_literal(triple.object, object)
	test.equal(triple.format(), '<http://example.com/test.html> <http://purl.org/dc/elements/1.1/title> "Test page" .', 'triple.format()')
	test.equal(triple.format(prefixes), '<http://example.com/test.html> dc:title "Test page" .', 'triple.format()')

if __name__ == '__main__':
	check_triple_with_resource()
	check_triple_with_literal()

