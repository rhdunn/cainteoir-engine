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
	subject   = metadata.RDFResource('http://example.com/test.html', None)
	predicate = metadata.RDFResource('http://purl.org/dc/elements/1.1/creator', None)
	object    = metadata.RDFResource('http://example.com/users/jane-bloggs', None)

	triple = metadata.RDFTriple(subject, predicate, object)
	check_resource(triple.subject, subject)
	check_resource(triple.predicate, predicate)
	check_resource(triple.object, object)

def check_triple_with_literal():
	subject   = metadata.RDFResource('http://example.com/test.html', None)
	predicate = metadata.RDFResource('http://purl.org/dc/elements/1.1/title', None)
	object    = metadata.RDFLiteral('Test page')

	triple = metadata.RDFTriple(subject, predicate, object)
	check_resource(triple.subject, subject)
	check_resource(triple.predicate, predicate)
	check_literal(triple.object, object)

if __name__ == '__main__':
	check_triple_with_resource()
	check_triple_with_literal()

