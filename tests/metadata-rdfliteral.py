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

def ok(cond, message):
	if (not cond):
		raise Exception(message)

def equal(a, b, message):
	ok(a == b, '%s : expected "%s", got "%s"' % (message, b, a))

def check_basic_literal():
	literal = metadata.RDFLiteral('This is a test.')
	equal(literal.text, 'This is a test.', 'literal.text')
	equal(literal.language, None, 'literal.language')
	equal(literal.type, None, 'literal.type')

def check_literal_text(text, expected):
	literal = metadata.RDFLiteral(text)
	equal(literal.text, expected, 'literal.text')

def check_literal_text_normalisation():
	check_literal_text('This    is  a test.', 'This is a test.')
	check_literal_text('a\rb\r\rc', 'a b c')
	check_literal_text('a\nb\n\nc', 'a b c')
	check_literal_text('a\r\nb\r\n\r\nc', 'a b c')
	check_literal_text('a\tb\t\tc', 'a b c')
	check_literal_text(' a b c ', 'a b c')
	check_literal_text(' \n\ta\t\t\n\tb\t  \r\n c', 'a b c')

def check_literal_with_language():
	literal = metadata.RDFLiteral('English text', 'en')
	equal(literal.text, 'English text', 'literal.text')
	equal(literal.language, 'en', 'literal.language')
	equal(literal.type, None, 'literal.type')

	literal = metadata.RDFLiteral('English (US) text', 'en-US')
	equal(literal.text, 'English (US) text', 'literal.text')
	equal(literal.language, 'en-US', 'literal.language')
	equal(literal.type, None, 'literal.type')

	literal = metadata.RDFLiteral('English (British) text', language='en-GB')
	equal(literal.text, 'English (British) text', 'literal.text')
	equal(literal.language, 'en-GB', 'literal.language')
	equal(literal.type, None, 'literal.type')

def check_typed_literal():
	literal = metadata.RDFLiteral('27', None, 'http://www.w3.org/2001/XMLSchema#integer')
	equal(literal.text, '27', 'literal.text')
	equal(literal.language, None, 'literal.language')
	equal(literal.type.ref, 'integer', 'literal.type.ref')
	equal(literal.type.base, 'http://www.w3.org/2001/XMLSchema#', 'literal.type.base')
	equal(literal.type.uri, 'http://www.w3.org/2001/XMLSchema#integer', 'literal.type.uri')
	equal(str(literal.type), 'http://www.w3.org/2001/XMLSchema#integer', 'str(literal.type)')

	literal = metadata.RDFLiteral('27', type='http://www.w3.org/2001/XMLSchema#string')
	equal(literal.text, '27', 'literal.text')
	equal(literal.language, None, 'literal.language')
	equal(literal.type.ref, 'string', 'literal.type.ref')
	equal(literal.type.base, 'http://www.w3.org/2001/XMLSchema#', 'literal.type.base')
	equal(literal.type.uri, 'http://www.w3.org/2001/XMLSchema#string', 'literal.type.uri')
	equal(str(literal.type), 'http://www.w3.org/2001/XMLSchema#string', 'str(literal.type)')

if __name__ == '__main__':
	check_basic_literal()
	check_literal_text_normalisation()
	check_literal_with_language()
	check_typed_literal()

