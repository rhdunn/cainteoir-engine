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

import epub
import harness as test

def check_metadata(filename, expect):
	filename = os.path.join(sys.path[0], filename)
	prefixes = {
		'http://www.idpf.org/2007/opf': 'opf',
		'http://purl.org/dc/elements/1.1/': 'dc',
		'%s#' % filename: 'doc',
	}

	opf = epub.OpfMetadata(filename, filename)

	with open(os.path.join(sys.path[0], expect), 'r') as f:
		expected = [ unicode(x) for x in f.read().split('\n') if not x == '' ]

	got = [ x for x in opf.format(prefixes) ]

	test.equal(got, expected, 'opf.format(file=%s)' % filename)

if __name__ == '__main__':
	check_metadata('opf-metadata/dublincore.opf', 'opf-metadata/dublincore.n3')
	check_metadata('opf-metadata/dublincore-with-language.opf', 'opf-metadata/dublincore-with-language.n3')

